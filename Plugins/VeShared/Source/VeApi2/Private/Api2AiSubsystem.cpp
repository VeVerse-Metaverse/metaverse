// Authors: Khusan Yadgarov, Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "Api2AiSubsystem.h"

#include "Api2CogAiRequest.h"
#include "Api2AiSimpleFsmRequest.h"
#include "VeShared.h"
#include "VeApi2.h"

FName FApi2AiSubsystem::Name = TEXT("Api2AISubsystem");

void FApi2AiSubsystem::Initialize() {
	if (!IConsoleManager::Get().IsNameRegistered(TEXT("aireq"))) {
		IConsoleManager::Get().RegisterConsoleCommand(TEXT("aireq"), TEXT("Make a test AI request"),
			FConsoleCommandDelegate::CreateLambda([=]() {
				FApi2CogAiRequestMetadata Metadata;

				{
					FApi2CogAiMessage Message;
					Message.From = TEXT("npc");
					FString ContentString = TEXT("{\"query\":\"whoami\"}");
					TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ContentString);
					TSharedPtr<FJsonObject> ContentJson;
					if (FJsonSerializer::Deserialize(Reader, ContentJson)) {
						Message.Content = ContentJson;
						Metadata.Messages.Add(Message);
					} else {
						VeLogErrorFunc("Failed to serialize content object");
					}
				}
				{
					FApi2CogAiMessage Message;
					Message.From = TEXT("system");
					auto ContentString = TEXT("{\"query\": \"whoami\",\"name\": \"Ava Reyes\",\"desc\": \"Woman, 30yo, blonde, beautiful, 175cm tall, yuo wear white blouse, black skirt and black shoes. Your eyes are blue. Artist. From small town, middle-class family, had good grades in school, didn't graduate the university\",\"faction\": \"good\",\"personality\": { \"aggression\": \"low\", \"greed\": \"low\", \"loyalty\": \"high\", \"courage\": \"medium\", \"intelligence\": \"high\", \"compassion\": \"medium\", \"honor\": \"medium\", \"stealth\": \"low\"}}");
					TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ContentString);
					TSharedPtr<FJsonObject> ContentJson;
					if (FJsonSerializer::Deserialize(Reader, ContentJson)) {
						Message.Content = ContentJson;
						Metadata.Messages.Add(Message);
					} else {
						VeLogErrorFunc("Failed to serialize content object");
					}
				}

				{
					FApi2CogAiMessage Message;
					Message.From = TEXT("npc");
					auto ContentString = TEXT("{\"query\":\"context\"}");
					TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ContentString);
					TSharedPtr<FJsonObject> ContentJson;
					if (FJsonSerializer::Deserialize(Reader, ContentJson)) {
						Message.Content = ContentJson;
						Metadata.Messages.Add(Message);
					} else {
						VeLogErrorFunc("Failed to serialize content object");
					}
				}
				{
					FApi2CogAiMessage Message;
					Message.From = TEXT("system");
					auto ContentString = TEXT("{\"query\":\"context\",\"desc\":\"Unreal Engine editor test level\",\"location\":\"Unreal editor test level, blue sky with volumetric clouds, default checkerboard floor, platform floating in the middle of nowhere\",\"time\":\"noon\",\"weather\":\"sunny\",\"mood\":\"neutral\"}");
					TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ContentString);
					TSharedPtr<FJsonObject> ContentJson;
					if (FJsonSerializer::Deserialize(Reader, ContentJson)) {
						Message.Content = ContentJson;
						Metadata.Messages.Add(Message);
					} else {
						VeLogErrorFunc("Failed to serialize content object");
					}
				}

				{
					FApi2CogAiMessage Message;
					Message.From = TEXT("npc");
					auto ContentString = TEXT("{\"query\":\"perception\"}");
					TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ContentString);
					TSharedPtr<FJsonObject> ContentJson;
					if (FJsonSerializer::Deserialize(Reader, ContentJson)) {
						Message.Content = ContentJson;
						Metadata.Messages.Add(Message);
					} else {
						VeLogErrorFunc("Failed to serialize content object");
					}
				}
				{
					FApi2CogAiMessage Message;
					Message.From = TEXT("system");
					auto ContentString = TEXT("{\"query\":\"perception\",\"perception\":{\"visual\":[{\"name\":\"Cube\",\"desc\":\"White cube, rigid, static\"},{\"name\":\"Sun\",\"desc\":\"Bright\"}],\"audio\":[{\"name\":\"Wind\",\"desc\":\"very light wind is blowing\"}],\"other\":[{\"name\":\"Sun\",\"desc\":\"warm sunlight\"}]}}");
					TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ContentString);
					TSharedPtr<FJsonObject> ContentJson;
					if (FJsonSerializer::Deserialize(Reader, ContentJson)) {
						Message.Content = ContentJson;
						Metadata.Messages.Add(Message);
					} else {
						VeLogErrorFunc("Failed to serialize content object");
					}
				}

				{
					FApi2CogAiMessage Message;
					Message.From = TEXT("npc");
					auto ContentString = TEXT("{\"query\":\"actions\"}");
					TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ContentString);
					TSharedPtr<FJsonObject> ContentJson;
					if (FJsonSerializer::Deserialize(Reader, ContentJson)) {
						Message.Content = ContentJson;
						Metadata.Messages.Add(Message);
					} else {
						VeLogErrorFunc("Failed to serialize content object");
					}
				}
				{
					FApi2CogAiMessage Message;
					Message.From = TEXT("system");
					auto ContentString = TEXT("{\"query\":\"actions\",\"actions\":[{\"type\":\"say\",\"target\":\"whom to say to\",\"message\":\"what to say\",\"emotion\":\"what do you feel\",\"thoughts\":\"your thoughts\",\"voice\":\"whisper, normal, loud\"}]}");
					TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ContentString);
					TSharedPtr<FJsonObject> ContentJson;
					if (FJsonSerializer::Deserialize(Reader, ContentJson)) {
						Message.Content = ContentJson;
						Metadata.Messages.Add(Message);
					} else {
						VeLogErrorFunc("Failed to serialize content object");
					}
				}

				{
					FApi2CogAiMessage Message;
					Message.From = TEXT("npc");
					auto ContentString = TEXT("{\"query\":\"inspect\",\"target\":\"Cube\"}");
					TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ContentString);
					TSharedPtr<FJsonObject> ContentJson;
					if (FJsonSerializer::Deserialize(Reader, ContentJson)) {
						Message.Content = ContentJson;
						Metadata.Messages.Add(Message);
					} else {
						VeLogErrorFunc("Failed to serialize content object");
					}
				}
				{
					FApi2CogAiMessage Message;
					Message.From = TEXT("system");
					auto ContentString = TEXT("{\"query\":\"inspect\",\"target\":{\"name\":\"Cube\",\"desc\":\"White cube, rigid, static. Can't be moved. Fixed on the floor.\"}}");
					TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ContentString);
					TSharedPtr<FJsonObject> ContentJson;
					if (FJsonSerializer::Deserialize(Reader, ContentJson)) {
						Message.Content = ContentJson;
						Metadata.Messages.Add(Message);
					} else {
						VeLogErrorFunc("Failed to serialize content object");
					}
				}

				const auto Callback = MakeShared<FOnCogAiRequestCompleted>();
				Callback->BindLambda([=](const FApi2CogAiResponseMetadata& InMetadata, EApi2ResponseCode InResponseCode, const FString& InError) {
					VeLogFunc("Response code: %d, error: %s", static_cast<int>(InResponseCode), *InError);
				});
				SendCogAiRequest(Metadata, Callback);
			}));
	}
}

void FApi2AiSubsystem::Shutdown() {}

void FApi2AiSubsystem::GetSimpleFsm(const FApi2AiSimpleFsmRequestMetadata& InMetadata, TSharedRef<FOnAiSimpleFsmRequestCompleted2> InCallback) const {
	const auto Request = MakeShared<FApi2AiSimpleFsmRequest>();

	Request->RequestMetadata = InMetadata;

	Request->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest>& InRequest, const bool bInSuccessful) {
		InCallback->ExecuteIfBound(Request->ResponseMetadata, Request->GetApiResponseCode(), InRequest->GetErrorString());
	});

	if (!Request->Process()) {
		InCallback->ExecuteIfBound({}, EApi2ResponseCode::Failed, TEXT("not processed"));
	}
}

void FApi2AiSubsystem::GetTextToSpeech(const FApi2AiTtsRequestMetadata& InMetadata, TSharedRef<FOnAiTtsRequestCompleted2> InCallback) const {
	const auto Request = MakeShared<FApi2AiTtsRequest>();

	Request->RequestMetadata = InMetadata;

	Request->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest>& InRequest, const bool bInSuccessful) {
		InCallback->ExecuteIfBound(Request->ResponseMetadata, Request->GetApiResponseCode(), InRequest->GetErrorString());
	});

	if (!Request->Process()) {
		InCallback->ExecuteIfBound({}, EApi2ResponseCode::Failed, TEXT("not processed"));
	}
}

void FApi2AiSubsystem::StreamTextToSpeech(const FApi2AiTtsRequestMetadata& InMetadata, TSharedRef<FOnAiTtsRequestCompleted2> InCallback) const {
	const auto Request = MakeShared<FApi2AiStreamTtsRequest>();

	Request->RequestMetadata = InMetadata;

	Request->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest>& InRequest, const bool bInSuccessful) {
		InCallback->ExecuteIfBound(Request->ResponseMetadata, Request->GetApiResponseCode(), InRequest->GetErrorString());
	});

	if (!Request->Process()) {
		InCallback->ExecuteIfBound({}, EApi2ResponseCode::Failed, TEXT("not processed"));
	}
}

void FApi2AiSubsystem::SendString(const FString& Url, const FString& InString, TSharedRef<FOnStringRequestCompleted2> InCallback) const {
	const auto Request = MakeShared<FApi2StringRequest>();

	Request->SetVerb(TEXT("POST"));
	Request->SetUrl(Url);
	Request->SetRequestBody(InString);

	Request->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest>& InRequest, const bool bInSuccessful) {
		InCallback->ExecuteIfBound(Request->GetResponseBody(), Request->GetApiResponseCode(), InRequest->GetErrorString());
	});

	if (!Request->Process()) {
		InCallback->ExecuteIfBound({}, EApi2ResponseCode::Failed, TEXT("not processed"));
	}
}

void FApi2AiSubsystem::SendCogAiRequest(const FApi2CogAiRequestMetadata& InMetadata, const TSharedRef<FOnCogAiRequestCompleted> InCallback) {
	// Create a new request.
	const TSharedPtr<FApi2CogAiRequest> Request = MakeShared<FApi2CogAiRequest>();

	// Set the request metadata.
	Request->RequestMetadata = InMetadata;

	// Bind the callback to the request.
	Request->GetOnRequestComplete().BindLambda([InCallback, this, Request](const TSharedPtr<FHttpRequest>& InRequest, const bool /*bInSuccessful*/) {
		// Lock the queue critical section until the end of the function.
		FScopeLock Lock(&CogAiQueueCriticalSection);

		// Reset the processing flag as we've finished processing the request.
		bProcessingCogAiRequest = false;

		// Execute the callback for the current request.
		InCallback->ExecuteIfBound(Request->ResponseMetadata, Request->GetApiResponseCode(), InRequest->GetErrorString());

		// Check if we have any requests left in the queue.
		if (CurrentCogAiQueueSize >= 0) {

			// Decrement the queue size as we've launched a new request.
			--CurrentCogAiQueueSize;

			if (TSharedPtr<FApi2CogAiRequest> PendingRequest; CogAiRequestQueue.Dequeue(PendingRequest)) {
				// We have a request in the queue, process it.
				if (!PendingRequest->Process()) {
					// Failed to process the request, reset the processing flag.
					bProcessingCogAiRequest = false;

					// Execute the callback with the error.
					InCallback->ExecuteIfBound({}, EApi2ResponseCode::Failed, TEXT("not processed"));
				} else {
					// Successfully launched the request, set the processing flag.
					bProcessingCogAiRequest = true;
				}
			} else {
				// No requests in the queue, reset the queue size.
				CurrentCogAiQueueSize = 0;

				// Reset the processing flag.
				bProcessingCogAiRequest = false;
			}
		}
	});

	{
		// Lock the queue critical section until the end of the scope.
		FScopeLock Lock(&CogAiQueueCriticalSection);

		// Check if we're already processing a request.
		if (!bProcessingCogAiRequest) {
			// Not processing a request, process this one.
			bProcessingCogAiRequest = true;

			// Launch the request.
			if (!Request->Process()) {
				// Failed to process the request, reset the processing flag.
				bProcessingCogAiRequest = false;

				// Execute the callback with the error.
				InCallback->ExecuteIfBound({}, EApi2ResponseCode::Failed, TEXT("not processed"));
			} else {
				// Successfully launched the request, set the processing flag.
				bProcessingCogAiRequest = true;
			}
		} else {
			if (CurrentCogAiQueueSize >= MaxQueueSize) {
				// Queue is full, execute the callback with the error.
				InCallback->ExecuteIfBound({}, EApi2ResponseCode::Failed, TEXT("queue full"));
				return;
			}

			// Already processing a request, queue this one to process later.
			CogAiRequestQueue.Enqueue(Request);

			// Increment the queue size.
			++CurrentCogAiQueueSize;
		}
	}
}

void FApi2AiSubsystem::SendCogAiUserRequest(const FApi2CogAiUserRequestMetadata& InMetadata, const TSharedRef<FOnCogAiRequestCompleted> InCallback) {
	// Create a new request.
	const TSharedPtr<FApi2CogAiUserRequest> Request = MakeShared<FApi2CogAiUserRequest>();

	// Set the request metadata.
	Request->RequestMetadata = InMetadata;

	// Bind the callback to the request.
	Request->GetOnRequestComplete().BindLambda([InCallback, this, Request](const TSharedPtr<FHttpRequest>& InRequest, const bool /*bInSuccessful*/) {
		// Lock the queue critical section until the end of the function.
		FScopeLock Lock(&CogAiUserQueueCriticalSection);

		// Reset the processing flag as we've finished processing the request.
		bProcessingCogAiUserRequest = false;

		// Execute the callback for the current request.
		InCallback->ExecuteIfBound(Request->ResponseMetadata, Request->GetApiResponseCode(), InRequest->GetErrorString());

		// Check if we have any requests left in the queue.
		if (CurrentCogAiUserQueueSize >= 0) {

			// Decrement the queue size as we've launched a new request.
			--CurrentCogAiUserQueueSize;

			if (TSharedPtr<FApi2CogAiUserRequest> PendingRequest; CogAiUserRequestQueue.Dequeue(PendingRequest)) {
				// We have a request in the queue, process it.
				if (!Request->Process()) {
					// Failed to process the request, reset the processing flag.
					bProcessingCogAiUserRequest = false;

					// Execute the callback with the error.
					InCallback->ExecuteIfBound({}, EApi2ResponseCode::Failed, TEXT("not processed"));
				} else {
					// Successfully launched the request, set the processing flag.
					bProcessingCogAiUserRequest = true;
				}
			} else {
				// No requests in the queue, reset the queue size.
				CurrentCogAiUserQueueSize = 0;

				// Reset the processing flag.
				bProcessingCogAiUserRequest = false;
			}
		}
	});

	{
		// Lock the queue critical section until the end of the scope.
		FScopeLock Lock(&CogAiUserQueueCriticalSection);

		// Check if we're already processing a request.
		if (!bProcessingCogAiUserRequest) {
			// Not processing a request, process this one.
			bProcessingCogAiUserRequest = true;

			// Launch the request.
			if (!Request->Process()) {
				// Failed to process the request, reset the processing flag.
				bProcessingCogAiUserRequest = false;

				// Execute the callback with the error.
				InCallback->ExecuteIfBound({}, EApi2ResponseCode::Failed, TEXT("not processed"));
			} else {
				// Successfully launched the request, set the processing flag.
				bProcessingCogAiUserRequest = true;
			}
		} else {
			if (CurrentCogAiUserQueueSize >= MaxQueueSize) {
				// Queue is full, execute the callback with the error.
				InCallback->ExecuteIfBound({}, EApi2ResponseCode::Failed, TEXT("queue full"));
				return;
			}

			// Already processing a request, queue this one to process later.
			CogAiUserRequestQueue.Enqueue(Request);

			// Increment the queue size.
			++CurrentCogAiUserQueueSize;
		}
	}
}
