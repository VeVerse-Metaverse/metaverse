// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "OAuthOAuthSubsystem.h"
#include "VeShared.h"
#include "VeApi2.h"
#include "Api2AuthSubsystem.h"
#include "HAL/PlatformApplicationMisc.h"

#define LOCTEXT_NAMESPACE "VeRpc"

const FName FOAuthOAuthSubsystem::Name = FName("TcpHelperSubsystem");

void FOAuthOAuthSubsystem::Initialize() {}

void FOAuthOAuthSubsystem::Shutdown() {}

void FOAuthOAuthSubsystem::CopyOAuthUrlToClipboard(const FString& Buffer) {
	int32 LinkFirst, LinkLast;
	Buffer.FindChar('<', LinkFirst);
	Buffer.FindLastChar('>', LinkLast);
	OAuthLink = Buffer.Mid(LinkFirst + 1, LinkLast - LinkFirst - 1);
	FPlatformApplicationMisc::ClipboardCopy(*OAuthLink);
}

void FOAuthOAuthSubsystem::Login(const FString& Provider, const TSharedRef<FOnOAuthCompleted>& InCallback) {
	// Reset the OAuth link.
	OAuthLink = {};

	// Terminate the previous process if it was running.
	if (ProcessHandle.IsValid()) {
		// Terminate the previous process.
		FPlatformProcess::TerminateProc(ProcessHandle, true);

		// Reset the process handle.
		ProcessHandle.Reset();
	}

	// Close the read pipe if it was opened before.
	if (ReadPipe) {
		FPlatformProcess::ClosePipe(ReadPipe, nullptr);
		ReadPipe = nullptr;
	}

	// Create a temporary write pipe.
	void* WritePipe = nullptr;

	// Create pipes.
	if (!FPlatformProcess::CreatePipe(ReadPipe, WritePipe)) {
		const auto _ = InCallback->ExecuteIfBound(Token, false, FString::Printf(TEXT("failed to create a pipe")));
	}

	// Start the OAuth helper process.
	const auto WorkDir = FPaths::ConvertRelativePathToFull(FPaths::Combine(FPaths::ProjectDir(), FVeConfigLibrary::GetOAuthHelperDir()));;
	const auto Path = FPaths::Combine(WorkDir, FVeConfigLibrary::GetOAuthHelperName());

	// Get the app id to make the API know which app is requesting the OAuth.
	const auto AppId = FVeConfigLibrary::GetAppId();

	// Form the command line parameters, provider and app id.
	const auto Params = FString::Printf(TEXT("-provider=%s -app-id=%s"), *Provider, *AppId.ToString(EGuidFormats::DigitsWithHyphensLower));

	// Start the OAuth helper as a hidden background process.
	ProcessHandle = FPlatformProcess::CreateProc(*Path, *Params, false, true, true, nullptr, 0, *WorkDir, WritePipe, ReadPipe);

	// Close a write pipe as we don't use it.
	FPlatformProcess::ClosePipe(nullptr, WritePipe);

	// Check if the process was started successfully.
	if (!ProcessHandle.IsValid()) {
		// Reset the process handle if it was not started successfully.
		ProcessHandle.Reset();
		// Close the read pipe.
		FPlatformProcess::ClosePipe(ReadPipe, nullptr);
		// Reset the read pipe.
		ReadPipe = nullptr;
		// Report the error and return.
		const auto _ = InCallback->ExecuteIfBound(Token, false, FString::Printf(TEXT("failed to start a process")));
		return;
	}

	// Reset the read pipe buffer as we start a new operation.
	if (ReadPipeBuffer.Len() > 0) {
		ReadPipeBuffer.Empty();
	}

	// Set up the ticker to read the pipe and check the status of the process.
	TickerDelegate = FTickerDelegate::CreateLambda([=](float) {
		int32 ReturnCode = -1;

		// Wait for the process to finish.
		if (const bool bProcessExited = FPlatformProcess::GetProcReturnCode(ProcessHandle, &ReturnCode); !bProcessExited) {
			// Process is still running, read the next chunk from the pipe.
			ReadPipeBuffer += FPlatformProcess::ReadPipe(ReadPipe);
			// Try to find the OAuth link in the buffer.
			if (OAuthLink.IsEmpty()) {
				CopyOAuthUrlToClipboard(ReadPipeBuffer);
			}
			// Return true to continue the ticker expecting more data.
			return true;
		}

		// Process has exited, read the last chunk from the pipe.
		ReadPipeBuffer += FPlatformProcess::ReadPipe(ReadPipe);

		if (ReturnCode != 0 || ReadPipeBuffer.IsEmpty() || ReadPipeBuffer.Contains("error: ")) {
			// Process has finished with an error.
			InCallback->ExecuteIfBound(Token, false, FString::Printf(TEXT("error running OAuth process: %s"), *ReadPipeBuffer));
			// Return false to stop the ticker.
			return false;
		}

		// Try to find the OAuth link in the buffer.
		if (OAuthLink.IsEmpty()) {
			CopyOAuthUrlToClipboard(ReadPipeBuffer);
		}

		int32 First;
		ReadPipeBuffer.FindChar('{', First);

		// Check if the output is doubled, if it is, find the last closing bracket.
		int32 Last = ReadPipeBuffer.Find("}{", ESearchCase::IgnoreCase, ESearchDir::FromStart, First);
		if (Last == INDEX_NONE) {
			// If we didn't receive doubled output, try to find the last closing bracket.
			ReadPipeBuffer.FindLastChar('}', Last);
		}

		// Check if we found the first and last brackets.
		const auto JsonString = ReadPipeBuffer.Mid(First, Last - First + 1);
		const TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(JsonString);
		TSharedPtr<FJsonObject> JsonPayload;
		if (!FJsonSerializer::Deserialize(JsonReader, JsonPayload) || !JsonPayload.IsValid()) {
			// Failed to parse the JSON response.
			InCallback->ExecuteIfBound(Token, false, FString::Printf(TEXT("error parsing JSON response: %s"), *ReadPipeBuffer));
			// Return false to stop the ticker.
			return false;
		}

		// Validate the response, it should contain the status field.
		if (!JsonPayload->HasField("status")) {
			InCallback->ExecuteIfBound(Token, false, FString::Printf(TEXT("invalid JSON response: %s"), *ReadPipeBuffer));
			return false;
		}

		// Check the status field and parse the error message if it's not ok.
		if (JsonPayload->GetStringField("status") != "ok") {
			FString Error;
			JsonPayload->TryGetStringField("message", Error);
			InCallback->ExecuteIfBound(Token, false, FString::Printf(TEXT("error running OAuth process: %s"), *ReadPipeBuffer));
			return false;
		}

		// Check if the response contains the data field.
		if (!JsonPayload->HasField("data")) {
			InCallback->ExecuteIfBound(Token, false, FString::Printf(TEXT("invalid JSON response: %s"), *ReadPipeBuffer));
			return false;
		}

		// Parse the token from the response data field.
		Token = JsonPayload->GetStringField("data");

		// Validate the token.
		if (Token.IsEmpty()) {
			InCallback->ExecuteIfBound(Token, false, FString::Printf(TEXT("invalid token: %s"), *ReadPipeBuffer));
			return false;
		}

		// Notify the authentication subsystem about the login.
		GET_MODULE_SUBSYSTEM(AuthSubsystem, Api2, Auth);
		if (!AuthSubsystem) {
			InCallback->ExecuteIfBound(Token, false, FString::Printf(TEXT("failed to notify the authentication subsystem")));
			return false;
		}

		// Proceed with the login, save the token to the encrypted session file, request the user info and notify required subsystems about the login.
		AuthSubsystem->LoginWithOAuthToken(Token);

		// Notify the caller about the success.
		const bool _ = InCallback->ExecuteIfBound(Token, true, {});

		// Terminate the process if it is still running.
		if (ProcessHandle.IsValid()) {
			// Terminate the previous process.
			FPlatformProcess::TerminateProc(ProcessHandle, true);

			// Reset the process handle.
			ProcessHandle.Reset();
		}

		// Success, return false to stop the ticker.
		return false;
	});

	// Start the ticker with a 1 second interval.
	TickerDelegateHandle = FTSTicker::GetCoreTicker().AddTicker(TickerDelegate, 1.0f);
}

#undef LOCTEXT_NAMESPACE
