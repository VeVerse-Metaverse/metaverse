// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "HttpDownloadTask.h"

#include "HttpModule.h"
#include "VeShared.h"
#include "VeApi2.h"
#include "Api2AuthSubsystem.h"
#include "VeDownload.h"
#include "Interfaces/IHttpResponse.h"
#include "FileDownloadTask.h"

#undef VE_LOG_MODULE
#define VE_LOG_MODULE HttpDownloadSubsystem


namespace {
	/** Max content length supported by UE HTTP requests. */
	constexpr uint64 MaxContentLength = 1ull << 31;
}

FHttpDownloadTask::FHttpDownloadTask(const FStorageItemId& InStorageItemId)
	: StorageItemId(InStorageItemId) {
	bLogHttpContent = FVeConfigLibrary::GetLogHttpContent();
}

FHttpDownloadTask::~FHttpDownloadTask() {
	HttpRequest_Stop();
}

void FHttpDownloadTask::Process(const FHttpDownloadTaskOptions& InOptions) {
	VeLog("Start downloading");

	if (InOptions.Filename.IsEmpty()) {
		VeLogErrorFunc("Failed to download file: filename is empty.");
		NativeOnError("filename is empty");
		return;
	}

	ProcessOptions = InOptions;

	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();

	Original_ContentFilename = ProcessOptions.Filename;
	Cache_ContentFilename = ProcessOptions.Filename + TEXT(".tmp");
	Cache_MetadataFilename = ProcessOptions.Filename + TEXT(".tmp.meta");

	// Check directory
	{
		FString Dir = FPaths::GetPath(ProcessOptions.Filename);
		if (!PlatformFile.DirectoryExists(*Dir)) {
			if (!PlatformFile.CreateDirectoryTree(*Dir)) {
				VeLogErrorFunc("Failed to create directory: %s", *Dir);
				NativeOnError("failed to create directory");
				return;
			}
		}
	}

	ResetSession();

	if (ProcessOptions.DeleteCache) {
		DeleteCacheFiles();
	}

	if (PlatformFile.FileExists(*Cache_ContentFilename) && PlatformFile.FileExists(*Cache_MetadataFilename) && Session_DownloadMetadata.LoadFromFile(Cache_MetadataFilename)) {
		if (Session_DownloadMetadata.GetDownloadedSize() != Session_DownloadMetadata.GetContentLength()) {
			// Continue downloading
			Session_BytesReceived_Start = Session_DownloadMetadata.GetDownloadedSize();
			Session_BlockOffset = Session_DownloadMetadata.GetDownloadedSize();
		}
	} else if (PlatformFile.FileExists(*Original_ContentFilename) && !InOptions.ContentMetadata.IsEmpty()) {
		DeleteCacheFiles();
		Session_DownloadMetadata.SetContentMetadata(InOptions.ContentMetadata);
		Session_DownloadMetadata.SetDownloadedSize(InOptions.ContentMetadata.ContentLength);
	} else {
		DeleteCacheFiles();
	}

	const FString PreSignedUrl = GetPreSignedUrl(StorageItemId);
	if (PreSignedUrl.IsEmpty()) {
		RequestContent(StorageItemId.GetUrl());
	} else {
		RequestPreSigned(PreSignedUrl);
	}
}

void FHttpDownloadTask::ResetSession() {
	Session_DownloadMetadata.Empty();
	Session_ContentUrl.Empty();
	Session_BytesReceived_Start = 0;
	Session_BlockOffset = 0;
	Session_BlockSize = BlockSize;
	Session_RestartNum = 0;
	Session_BytesReceived = 0;
}

FString FHttpDownloadTask::PrintfSeparator(int64 Value) {
	auto Source(FString::Printf(TEXT("%lld"), Value));
	auto StrLen = Source.Len();

	int32 SeparatorNum;
	int32 SeparatorPos;

	if (Value < 0) {
		if (StrLen <= 4) {
			return Source;
		}
		SeparatorNum = (StrLen - 1) / 3;
		SeparatorPos = (StrLen - 1) % 3 + 1;
		if (SeparatorPos == 1) {
			--SeparatorNum;
			SeparatorPos += 3;
		}
	} else {
		if (StrLen <= 3) {
			return Source;
		}
		SeparatorNum = StrLen / 3;
		SeparatorPos = StrLen % 3;
		if (SeparatorPos == 0) {
			--SeparatorNum;
			SeparatorPos += 3;
		}
	}

	FString Result;
	Result.Empty(StrLen + SeparatorNum);

	for (int32 s = 0; s < StrLen; ++s) {
		if (s == SeparatorPos) {
			Result += TCHAR('\'');
			SeparatorPos += 3;
		}
		Result += Source[s];
	}

	return Result;
}

void FHttpDownloadTask::Cancel(bool DeleteCache) {
	VeLog("Cancel");
	NativeOnCancel(TEXT("Downloading canceled"), DeleteCache);
}

void FHttpDownloadTask::RequestPreSigned(const FString& PreSignedUrl) {
	GET_MODULE_SUBSYSTEM(AuthSubsystem, Api2, Auth);
	if (!AuthSubsystem) {
		NativeOnError(TEXT("failed to authorization"));
		return;
	}

	const FString Token = AuthSubsystem->GetSessionToken();
	if (Token.IsEmpty()) {
		VeLogTestFunc("failed to set authentication headers");
		NativeOnError(TEXT("failed to authorization"));
		return;
	}

	// On Response

	auto OnCompleteCallback = [=](FHttpRequestPtr InRequest, FHttpResponsePtr InResponse, const bool bSuccessful) {
		const int32 ResponseCode = InResponse->GetResponseCode();

		if (!bSuccessful || ResponseCode != Http::Code::Created) {
			NativeOnError(InResponse->GetContentAsString());
			return;
		}

		if (!InResponse) {
			NativeOnError(TEXT("invalid signed url response"));
			return;
		}

		// Store the pre-signed url we are going to use.
		const FString Url = InResponse->GetHeader(TEXT("Location"));
		if (Url.IsEmpty()) {
			NativeOnError(TEXT("invalid signed url response"));
			return;
		}

		RequestContent(Url);
	};

	// Request

	VeLog("PREREQUEST: %s", *PreSignedUrl);

	// todo: smart pointer need increment/decrement 
	HttpRequest_Reset();
	HttpRequest->SetVerb(Http::Method::Get);
	HttpRequest->SetURL(PreSignedUrl);
	HttpRequest->SetHeader(TEXT("Authorization"), FString::Printf(TEXT("Bearer %s"), *Token));
	HttpRequest->OnProcessRequestComplete().BindLambda(OnCompleteCallback);

	if (!HttpRequest->ProcessRequest()) {
		if (const auto Response = HttpRequest->GetResponse()) {
			VeLogTestFunc("failed to process HTTP request: %d, %s", Response->GetResponseCode(), *Response->GetContentAsString());
		} else {
			VeLogTestFunc("failed to process HTTP request");
		}
		NativeOnError(TEXT("failed to process HTTP request"));
		return;
	}
}

void FHttpDownloadTask::RequestContent(const FString& Url) {
	VeLog("RequestContent");
	VeLog("REQUEST: %s", *Url);

	Session_ContentUrl = Url;

	Timer.Start();
	Progress.Start(Session_BlockOffset);

	// todo: smart pointer need increment/decrement
	HttpRequest_Reset();
	HttpRequest->SetVerb(Http::Method::Get);
	HttpRequest->SetURL(Session_ContentUrl);
	HttpRequest->OnHeaderReceived().BindSP(AsShared(), &FHttpDownloadTask::HttpRequest_OnHeaderReceived);
	HttpRequest->OnRequestProgress().BindSP(AsShared(), &FHttpDownloadTask::HttpRequest_OnRequestProgress);
	HttpRequest->OnProcessRequestComplete().BindSP(AsShared(), &FHttpDownloadTask::HttpRequest_OnProcessRequestComplete);

	// if (!Remote_ETag.IsEmpty()) {
	// 	HttpRequest->SetHeader(Http::Header::IfNoneMatch, Remote_ETag);
	// }

	// Range
	if (Session_BlockSize) {
		const auto StartOffset = Session_BlockOffset;
		const auto EndOffset = Session_BlockOffset + Session_BlockSize - 1;
		HttpRequest->SetHeader(Http::Header::Range, FString::Printf(TEXT("bytes=%lld-%lld"), StartOffset, EndOffset));
		VeLog("Set download block: %s-%s", *PrintfSeparator(StartOffset), *PrintfSeparator(EndOffset));
	}

	if (!HttpRequest->ProcessRequest()) {
		if (const auto Response = HttpRequest->GetResponse()) {
			VeLogTestFunc("failed to process HTTP request: %d, %s", Response->GetResponseCode(), *Response->GetContentAsString());
		} else {
			VeLogTestFunc("failed to process HTTP request");
		}
		NativeOnError(TEXT("failed to process HTTP request"));
		return;
	}
}

void FHttpDownloadTask::HttpRequest_OnHeaderReceived(FHttpRequestPtr Request, const FString& HeaderName, const FString& NewHeaderValue) {
	if (!HttpRequest) {
		return;
	}

	VeLog("HEADER %s: %s", *HeaderName, *NewHeaderValue);
	HttpRequest_Headers.Emplace(HeaderName, NewHeaderValue);
}

void FHttpDownloadTask::HttpRequest_OnRequestProgress(FHttpRequestPtr InRequest, const int32 BytesSent, const int32 BytesReceived) {
	if (!HttpRequest) {
		return;
	}

	// First call: the response headers not received yet.
	// Second call: process the response headers.

	// First call
	if (!HttpRequest_Headers.Num()) {
		return;
	}

	// Process headers
	if (!HttpRequest_HeaderComplete) {
		HttpRequest_HeaderComplete = true;
		NativeOnHeaders(InRequest, nullptr, HttpRequest_Headers);
		if (!HttpRequest) {
			return;
		}
	}

	if (!Progress.IsTimeToProgress()) {
		return;
	}

	const auto DownloadedSize = Session_BlockOffset + BytesReceived;
	Session_BytesReceived = DownloadedSize - Session_BytesReceived_Start;
	const auto ContentLength = HttpRequest_DownloadMetadata.GetContentLength();
	NativeOnProgress(ContentLength, DownloadedSize, Session_BytesReceived);
}

void FHttpDownloadTask::HttpRequest_OnProcessRequestComplete(FHttpRequestPtr InRequest, FHttpResponsePtr InResponse, const bool bSuccessful) {
	if (!HttpRequest || !InResponse) {
		NativeOnError(TEXT("invalid HTTP objects"));
		return;
	}

	// Headers
	if (!HttpRequest_HeaderComplete) {
		HttpRequest_HeaderComplete = true;
		NativeOnHeaders(InRequest, InResponse, HttpRequest_Headers);
		if (!HttpRequest) {
			return;
		}
	}

	// RequestProgress
	const auto DownloadedSize = Session_BlockOffset + InResponse->GetContentLength();
	if (Session_BytesReceived != DownloadedSize - Session_BytesReceived_Start) {
		Session_BytesReceived = DownloadedSize - Session_BytesReceived_Start;
		const auto ContentLength = HttpRequest_DownloadMetadata.GetContentLength();
		if (DownloadedSize == ContentLength || Progress.IsTimeToProgress()) {
			NativeOnProgress(ContentLength, DownloadedSize, Session_BytesReceived);
		}
	}

	NativeOnContent(InRequest, InResponse, bSuccessful);
}

void FHttpDownloadTask::NativeOnHeaders(FHttpRequestPtr InRequest, FHttpResponsePtr InResponse, const TMap<FString, FString>& InHeaders) {
	VeLog("HEADERS COMPLETE: %d", InHeaders.Num());

	if (InResponse.IsValid()) {
		const auto ResponseCode = InResponse->GetResponseCode();
		if (ResponseCode == Http::Code::Forbidden) {
			LogHttpResponse(InResponse);
			NativeOnCancel(TEXT("403 Forbidden"));
			return;
		}
	}

	HttpRequest_DownloadMetadata.LoadFromHeaders(InHeaders);

	if (HttpRequest_DownloadMetadata.GetContentLength() < 0) {
		VeLogErrorFunc("Filed to download file: error file size: %s", *StorageItemId.ToString());
		NativeOnError(TEXT("filed to download file: error file size"));
		return;
	}

	if (HttpRequest_DownloadMetadata.GetContentLength() > INT32_MAX && !HttpRequest_DownloadMetadata.GetAcceptRanges()) {
		VeLogErrorFunc("File too large for downloading: %s", *StorageItemId.ToString());
		NativeOnError(TEXT("file too large for downloading"));
		return;
	}

	// Source exists
	if (!Session_DownloadMetadata.GetContentMetadata().IsEmpty()) {
		// Source changed
		if (Session_DownloadMetadata.IsSourceChanged(HttpRequest_DownloadMetadata.GetContentMetadata())) {
			// Restart downloading
			Session_DownloadMetadata = HttpRequest_DownloadMetadata;
			Session_DownloadMetadata.SetDownloadedSize(0);
			Session_BlockOffset = 0;
			DeleteCacheFiles();
			RequestContent(Session_ContentUrl);
			return;
		}

		// Source not changed and downloading complete
		if (Session_DownloadMetadata.GetDownloadedSize() == HttpRequest_DownloadMetadata.GetContentLength()) {
			VeLogFunc("Downloading file not changed: %s", *StorageItemId.ToString());
			NativeOnComplete(Session_DownloadMetadata.GetDownloadedSize(), Original_ContentFilename);
			return;
		}
	}
}

void FHttpDownloadTask::NativeOnContent(FHttpRequestPtr InRequest, FHttpResponsePtr InResponse, const bool bSuccessful) {
	VeLog("CONTENT COMPLETE: %s bytes", *PrintfSeparator(Session_BlockOffset + InResponse->GetContentLength()));

	if (!InResponse) {
		VeLogErrorFunc("Invalid HTTP response");
		NativeOnError(TEXT("invalid HTTP response"));
		return;
	}

	if (!bSuccessful) {
		LogHttpResponse(InResponse);
		NativeOnError(TEXT("failed to download a file"));
		return;
	}

	// Check ResponseCode
	const auto ResponseCode = InResponse->GetResponseCode();
	if (ResponseCode == Http::Code::Forbidden) {
		LogHttpResponse(InResponse);
		NativeOnCancel(TEXT("403 Forbidden"));
		return;
	}
	if (ResponseCode != Http::Code::OK && ResponseCode != Http::Code::PartialContent) {
		LogHttpResponse(InResponse);
		NativeOnError(TEXT("failed to process response"));
		return;
	}

	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	const auto& Content = InResponse->GetContent();
	Session_BlockOffset += Content.Num();


	if (HttpRequest_DownloadMetadata.GetContentLength() != 0 && Session_BlockOffset > HttpRequest_DownloadMetadata.GetContentLength()) {
		PlatformFile.DeleteFile(*Cache_ContentFilename);
		NativeOnError(TEXT("filed to download file: content-length error"));
		return;
	}

	// Append content to temporary file
	if (auto* FileHandle = PlatformFile.OpenWrite(*Cache_ContentFilename, true, false)) {
		VeLogVerbose("Writing content to temporary file.");
		if (!FileHandle->Write(Content.GetData(), Content.Num())) {
			VeLogErrorFunc("Failed to write to temporary file: %s", *Cache_ContentFilename);
			NativeOnError(FString::Printf(TEXT("failed to write to file: %s"), *Cache_ContentFilename));
			delete FileHandle; // don't forget
			return;
		}
		delete FileHandle; // don't forget
	} else {
		VeLogErrorFunc("Failed to open temporary file for writing: %s", *Cache_ContentFilename);
		NativeOnError(FString::Printf(TEXT("failed to open a file for writing: %s"), *Cache_ContentFilename));
		return;
	}

	Session_DownloadMetadata = HttpRequest_DownloadMetadata;
	Session_DownloadMetadata.SetDownloadedSize(Session_BlockOffset);
	Session_DownloadMetadata.SaveToFile(Cache_MetadataFilename);

	// Stream complete
	if (Session_DownloadMetadata.GetContentLength() == 0) {
		NativeOnComplete(Session_DownloadMetadata.GetDownloadedSize(), Original_ContentFilename);
		return;
	}

	// Content complete
	if (Session_DownloadMetadata.GetContentLength() == Session_BlockOffset) {
		NativeOnComplete(Session_DownloadMetadata.GetDownloadedSize(), Original_ContentFilename);
		return;
	}

	// Download next block

	RequestContent(Session_ContentUrl);
}

void FHttpDownloadTask::NativeOnProgress(int64 ContentLength, int64 DownloadedSize, int64 BytesReceived) {
	const auto Time = Timer.GetSeconds();
	const auto BytesPerSec = Progress.GetSpeed(DownloadedSize);

	float ProgressPerc = 0.f;
	if (ContentLength) {
		ProgressPerc = static_cast<float>(DownloadedSize) / static_cast<float>(ContentLength);
	}

	VeLog("PROGRESS: %s/%s (%.2f%%); Speed: %s; Time: %.2f",
		*PrintfSeparator(DownloadedSize),
		*PrintfSeparator(ContentLength),
		ProgressPerc * 100.f,
		*PrintfSeparator(BytesPerSec),
		Time);

	FHttpDownloadProgressMetadata ResultMetadata;
	ResultMetadata.ContentLength = ContentLength;
	ResultMetadata.DownloadedSize = DownloadedSize;
	ResultMetadata.Progress = ProgressPerc;
	ResultMetadata.BytesPerSec = BytesPerSec;
	ResultMetadata.Time = Time;

	OnProgress.Broadcast(this, ResultMetadata);
}

void FHttpDownloadTask::NativeOnComplete(int64 ContentLength, const FString& Filename) {
	VeLog("DOWNLOAD COMPLETE: %s", *Filename);
	VeLog("Complete downloading. Time: %.3f", Timer.GetSeconds());

	HttpRequest_Stop();

	RenameCacheToOriginal();

	FHttpDownloadResultMetadata ResultMetadata;
	ResultMetadata.Code = EHttpDownloadCode::Completed;
	ResultMetadata.ContentMetadata = Session_DownloadMetadata.GetContentMetadata();
	ResultMetadata.Time = Timer.GetSeconds();
	ResultMetadata.Filename = Original_ContentFilename;

	OnComplete.Broadcast(this, ResultMetadata);
}

void FHttpDownloadTask::NativeOnCancel(const FString&& Message, bool InDeleteCache) {
	VeLog("DOWNLOAD CANCEL: %s", *Message);
	VeLog("Cancel downloading. Time: %.3f", Timer.GetSeconds());

	HttpRequest_Stop();

	if (InDeleteCache) {
		DeleteCacheFiles();
	}

	FHttpDownloadResultMetadata ResultMetadata;
	ResultMetadata.Code = EHttpDownloadCode::Cancelled;
	ResultMetadata.Time = Timer.GetSeconds();
	ResultMetadata.Error = Message;

	OnComplete.Broadcast(this, ResultMetadata);
}

void FHttpDownloadTask::NativeOnError(const FString&& Message) {
	VeLog("DOWNLOAD ERROR: %s", *Message);
	VeLog("Error downloading. Time: %.3f", Timer.GetSeconds());

	HttpRequest_Stop();

	DeleteCacheFiles();

	FHttpDownloadResultMetadata ResultMetadata;
	ResultMetadata.Time = Timer.GetSeconds();
	ResultMetadata.Code = EHttpDownloadCode::Error;
	ResultMetadata.Error = Message;

	OnComplete.Broadcast(this, ResultMetadata);
}

void FHttpDownloadTask::HttpRequest_Reset() {
	HttpRequest_Stop();

	HttpRequest = FHttpModule::Get().CreateRequest();
}

void FHttpDownloadTask::HttpRequest_Stop() {
	HttpRequest_HeaderComplete = false;
	HttpRequest_Headers.Empty();

	if (HttpRequest) {
		HttpRequest->OnHeaderReceived().Unbind();
		HttpRequest->OnRequestProgress().Unbind();
		HttpRequest->OnProcessRequestComplete().Unbind();
		HttpRequest->CancelRequest();
		HttpRequest.Reset();
	}
}

FString FHttpDownloadTask::GetPreSignedUrl(const FStorageItemId& InStorageId) const {
	FString EntityId, FileId;

	if (InStorageId.GetEntityId().IsValid() && InStorageId.GetFileId().IsValid()) {
		EntityId = StorageItemId.GetEntityId().ToString(EGuidFormats::DigitsWithHyphensLower);
		FileId = StorageItemId.GetFileId().ToString(EGuidFormats::DigitsWithHyphensLower);
	} else if (!InStorageId.GetUrl().IsEmpty()) {
		// Try to parse the entity id and file id from the URL of format "https://bucket.region.amazonaws.com/entity-id/file-id"
		TArray<FString> Out;
		InStorageId.GetUrl().ParseIntoArray(Out, TEXT("/"), true);

		// Valid URL should give us 4 tokens ["https:", "bucket.region.amazonaws.com", "entity-id", "file-id"]
		if (Out.Num() != 4) {
			return FString();
		}

		FString Token = Out[2];
		if (FGuid Guid; FGuid::Parse(Token, Guid)) {
			EntityId = Guid.ToString(EGuidFormats::DigitsWithHyphensLower);
		} else {
			return FString();
		}

		Token = Out[3];
		if (FGuid Guid; FGuid::Parse(Token, Guid)) {
			FileId = Guid.ToString(EGuidFormats::DigitsWithHyphensLower);
		} else {
			return FString();
		}
	} else {
		return FString();
	}

	return FString::Printf(TEXT("%s/files/download-pre-signed?entityId=%s&fileId=%s"), *Api::GetApi2RootUrl(), *EntityId, *FileId);
}

void FHttpDownloadTask::DeleteCacheFiles() const {
	VeLog("Delete cache files.");
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	PlatformFile.DeleteFile(*Cache_ContentFilename);
	PlatformFile.DeleteFile(*Cache_MetadataFilename);
}

bool FHttpDownloadTask::RenameCacheToOriginal() const {
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();

	// If the original source file is not changed, then cache files not exists
	if (!PlatformFile.FileExists(*Cache_ContentFilename) && !PlatformFile.FileExists(*Cache_MetadataFilename)) {
		return true;
	}

	if (!PlatformFile.FileExists(*Cache_ContentFilename)) {
		VeLogErrorFunc("Cache file not exists: %s", *Cache_ContentFilename);
		return false;
	}

	if (!PlatformFile.FileExists(*Cache_MetadataFilename)) {
		VeLogErrorFunc("Cache file not exists: %s", *Cache_MetadataFilename);
		return false;
	}

	// Delete Original source file
	if (PlatformFile.FileExists(*Original_ContentFilename)) {
		if (!PlatformFile.DeleteFile(*Original_ContentFilename)) {
			VeLogErrorFunc("Failed to delete file: %s", *Original_ContentFilename);
			return false;
		}
	}

	// Rename Cache source file
	if (!PlatformFile.MoveFile(*Original_ContentFilename, *Cache_ContentFilename)) {
		VeLogErrorFunc("Failed to rename file: %s", *Original_ContentFilename);
		return false;
	}

	// Delete Cache metadata file
	if (!PlatformFile.DeleteFile(*Cache_MetadataFilename)) {
		VeLogErrorFunc("Failed to delete file: %s", *Cache_MetadataFilename);
	}

	return true;
}

bool FHttpDownloadTask::RenameFile(const TCHAR* To, const TCHAR* From) {
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	if (!PlatformFile.FileExists(From)) {
		return false;
	}
	if (PlatformFile.FileExists(To)) {
		if (!PlatformFile.DeleteFile(To)) {
			return false;
		}
	}
	return PlatformFile.MoveFile(To, From);
}

FString FHttpDownloadTask::GetResponseContent(FHttpResponsePtr InResponse) {
	FString Content = InResponse->GetContentAsString();
	Content.ReplaceInline(TEXT("\r"), TEXT(""));
	Content.ReplaceInline(TEXT("\n"), TEXT(""));
	Content.ReplaceInline(TEXT("\t"), TEXT(""));
	return Content;
}

void FHttpDownloadTask::LogHttpResponse(FHttpResponsePtr InResponse) const {
	// if (bLogHttpContent) {
	if (InResponse->GetContentLength()) {
		TArray<FString> Headers;
		for (auto& Header : InResponse->GetAllHeaders()) {
			int32 Index = 0;
			if (Header.FindChar(TEXT(':'), Index) && Header.Len() > Index + 1) {
				Headers.Emplace(FString::Printf(TEXT("\"%s\": \"%s\""), *Header.Left(Index), *Header.RightChop(Index + 2)));
			}
		}

		VeLogVerbose("Request: %s", *InResponse->GetURL());
		VeLogVerbose("Response: %d {%s}", InResponse->GetResponseCode(), *FString::Join(Headers, TEXT(", ")).Left(2000));
		VeLogVerbose("Response: %s", *GetResponseContent(InResponse).Left(2000));
	}
	// }
}
