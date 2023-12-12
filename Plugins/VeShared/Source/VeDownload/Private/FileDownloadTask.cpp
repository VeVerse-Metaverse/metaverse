// Author: Egor A. Pristavka. Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#include "FileDownloadTask.h"
#include "VeDownload.h"
#include "VeApi2.h"
#include "Api2AuthSubsystem.h"
#include "GenericPlatform/GenericPlatformHttp.h"
#include "HAL/PlatformFileManager.h"
#include "Interfaces/IHttpResponse.h"
#include "VeShared.h"

void CleanHttpRequest(FHttpRequestPtr HttpRequest) {
	if (HttpRequest) {
		HttpRequest->OnProcessRequestComplete().Unbind();
		HttpRequest->OnRequestProgress().Unbind();
		HttpRequest->CancelRequest();
		HttpRequest = nullptr;
	}
}

void FFileDownloadTask::SetRangeHeader(const int64 Start, const int64 End) {
	FString RangeHeaderValue = FString();

	if (Start >= 0 && End >= 0) {
		RangeHeaderValue = FString::Printf(TEXT("bytes=%lld-%lld"), Start, End);
	} else if (Start >= 0) {
		RangeHeaderValue = FString::Printf(TEXT("bytes=%lld-"), Start);
	} else if (End >= 0) {
		RangeHeaderValue = FString::Printf(TEXT("bytes=-%lld"), End);
	}

	if (HttpRequest.IsValid()) {
		for (auto& i : HttpRequest->GetAllHeaders()) {
			FString l, r;
			i.Split(FString(TEXT(":")), &l, &r);
			HttpRequest->SetHeader(l, FString());
		}
		if (!RangeHeaderValue.IsEmpty()) {

			HttpRequest->SetHeader(Http::Header::Range, RangeHeaderValue);
		}
	}
}

FFileDownloadTask::FFileDownloadTask(const FFileDownloadTaskRequest& request)
	: lTotalChunks(0), Result{} {
	Request = request;
	lMaxChunkSize = 1024 * 1024 * 1024; // 1 GB size
	lCurrentChunk = -1;                 // not even first chunk
	lTotalSize = -1;
	HttpRequest = nullptr;
}

bool FFileDownloadTask::RequestNextSegment() {
	if (bCompleted) {
		// all requests completed
		return true;
	}

	if (PreSignedUrl.IsEmpty()) {
		if (Request.Url.StartsWith("https://veverse-") && Request.Url.Contains(".s3") && Request.Url.Contains("amazonaws.com/")) {
			PreSignedUrlHttpRequest = FHttpModule::Get().CreateRequest();
			check(PreSignedUrlHttpRequest != nullptr);

			if (!PreSignedUrlHttpRequest->OnProcessRequestComplete().IsBound()) {
				PreSignedUrlHttpRequest->OnProcessRequestComplete().BindRaw(this, &FFileDownloadTask::PreSignedUrlHttpRequestComplete);
			}

			const FString Url = Api::GetApiRootUrl() / TEXT("files") / TEXT("download") + FString::Printf(TEXT("?id=%s"), *FGenericPlatformHttp::UrlEncode(Request.Id.ToString(EGuidFormats::DigitsWithHyphens)));
			PreSignedUrlHttpRequest->SetURL(Url);

			GET_MODULE_SUBSYSTEM(AuthSubsystem, Api2, Auth);
			if (AuthSubsystem) {
				if (const auto& Key = AuthSubsystem->GetUserApiKey(); !Key.IsEmpty()) {
					PreSignedUrlHttpRequest->SetHeader(Api::Headers::Key, Key);
				}
			}

			PreSignedUrlHttpRequest->SetHeader(Api::Headers::ContentType, Api::ContentTypes::ApplicationJson);

			PreSignedUrlHttpRequest->ProcessRequest();

			return false;
		}

		PreSignedUrl = Request.Url;
	}

	// unable to reuse request, getting no OnProgress events
	CleanHttpRequest(HttpRequest);
	HttpRequest = FHttpModule::Get().CreateRequest();
	check(HttpRequest != nullptr);

	if (!HttpRequest->OnProcessRequestComplete().IsBound()) {
		HttpRequest->OnProcessRequestComplete().BindRaw(this, &FFileDownloadTask::HttpRequestComplete);
	}
	if (!HttpRequest->OnRequestProgress().IsBound()) {
		HttpRequest->OnRequestProgress().BindRaw(this, &FFileDownloadTask::HttpRequestProgress);
	}

	HttpRequest->SetURL(PreSignedUrl);
	HttpRequest->SetVerb(Http::Method::Get);
	if (lTotalSize == -1) {
		// start new download, request for size
		SetRangeHeader(0, 0);
	} else {
		// continue downloading segments
		auto end = (lCurrentChunk + 1) * lMaxChunkSize - 1;
		SetRangeHeader(lCurrentChunk * lMaxChunkSize + lChunkOffset, end < lTotalSize ? end : lTotalSize - 1);
	}

	HttpRequest->ProcessRequest();

	return false;
}

void FFileDownloadTask::Stop() {
	// cancel request
	if (HttpRequest.IsValid() && HttpRequest->GetStatus() == EHttpRequestStatus::Processing) {
		HttpRequest->CancelRequest();
	}
	// reset progress (resets to the state where it can be restored)
	lTotalSize = -1;
	// execute delegates
	Complete(false, Errors::Cancelled);
}

FFileDownloadTask::~FFileDownloadTask() {
	CleanHttpRequest(HttpRequest);
}

void FFileDownloadTask::PreSignedUrlHttpRequestComplete(FHttpRequestPtr InRequest, FHttpResponsePtr InResponse, const bool bSuccess) {
	const int32 ResponseCode = InResponse->GetResponseCode();

	if (PreSignedUrlHttpRequest->OnProcessRequestComplete().IsBound()) {
		PreSignedUrlHttpRequest->OnProcessRequestComplete().Unbind();
	}

	if (PreSignedUrlHttpRequest->OnRequestProgress().IsBound()) {
		PreSignedUrlHttpRequest->OnRequestProgress().Unbind();
	}

	VeLogVerbose("HTTP Response: code %d, body %s", ResponseCode, *InResponse->GetContentAsString());

	if (!bSuccess || ResponseCode != Http::Code::OK) {
		Complete(false, GetDetailMessage(InResponse->GetContentAsString()));
		return;
	}

	if (InResponse.IsValid()) {
		PreSignedUrl = InResponse->GetContentAsString();
		RequestNextSegment();
	} else {
		Complete(false, GetDetailMessage(InResponse->GetContentAsString()));
	}
}

FString FFileDownloadTask::GetDetailMessage(const FString& ResponseMessasge) const {
	const auto JsonReader = TJsonReaderFactory<>::Create(ResponseMessasge);
	TSharedPtr<FJsonObject> JsonObject;
	if (!FJsonSerializer::Deserialize(JsonReader, JsonObject) || !JsonObject.IsValid()) {
		VeLogErrorFunc("Failed to deserialize the message: {%s}", *ResponseMessasge);
		return ResponseMessasge;
	}

	if (JsonObject->HasTypedField<EJson::String>(Le7elDownloadJsonFields::Detail)) {
		return JsonObject->GetStringField(Le7elDownloadJsonFields::Detail);
	}
	return ResponseMessasge;
}

void FFileDownloadTask::HttpRequestProgress(FHttpRequestPtr httpRequest, int32 bytesSend, int32 inBytesReceived) {
	if (lTotalSize == -1) {
		// ignore progress of size request
		return;
	}
	auto newp = (inBytesReceived + lChunkOffset + lCurrentChunk * lMaxChunkSize) * 1.0f / lTotalSize;

	if (newp - fProgress > fProgressThreshold) {
		fProgress = newp;

		OnProgress.ExecuteIfBound({Request.Id, lTotalSize, fProgress});
	}
}

void FFileDownloadTask::Complete(bool bSuccessful, const FString& Error) {
	if (bCompleted) return;
	Result.Id = Request.Id;
	Result.Path = Request.Path;
	Result.Url = Request.Url;
	Result.Error = Error;
	Result.bSuccessful = bSuccessful;
	bCompleted = true;

	OnChunk.ExecuteIfBound({Request.Id, lTotalSize, fProgress});
	OnComplete.ExecuteIfBound(Result);
}

void FFileDownloadTask::HttpRequestComplete(FHttpRequestPtr InRequest, FHttpResponsePtr InResponse, const bool bSuccess) {
	if (!bSuccess) {
		Complete(false, Errors::ChunkResponseError);
		return;
	}

	if (InResponse.IsValid()) {
		const int32 ResponseCode = InResponse->GetResponseCode();
		//LogF("%d", ResponseCode);

		IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
		if (ResponseCode == Http::Code::PartialContent || ResponseCode == Http::Code::OK) {

			if (lTotalSize == -1) {
				// request for size completed
				const FString ContentRangeHeader = InResponse->GetHeader(Http::Header::ContentRange);
				FString ContentLength;
				ContentRangeHeader.Split(TEXT("/"), nullptr, &ContentLength);
				lTotalSize = FCString::Atoi64(*ContentLength);
				lTotalChunks = (lTotalSize - 1l) / lMaxChunkSize + 1l;

				if (OnHeader.Execute(FFileDownloadTaskHeader{Request.Id, InResponse->GetHeader(VersionHeader), Request.bOverwrite})) {
					// force to overwrite if header indicates update
					Request.bOverwrite = true;
				}

				if (PlatformFile.FileExists(*Request.Path)) {
					if (Request.bOverwrite) {
						// remove file and overwrite with new content
						if (!PlatformFile.DeleteFile(*Request.Path))
							LogWarningF("%s", *Errors::FailedToDeleteFile);
						lCurrentChunk = 0;
						lChunkOffset = 0;
					} else {
						// set to downloading from end
						auto fileSize = PlatformFile.FileSize(*Request.Path);
						if (fileSize == lTotalSize) {
							Complete(true, FString{});
							return;
						}
						lCurrentChunk = fileSize / lMaxChunkSize;
						lChunkOffset = fileSize % lMaxChunkSize;
					}
				} else {
					// new download
					lCurrentChunk = 0;
					lChunkOffset = 0;
				}
				fProgress = (lChunkOffset + lCurrentChunk * lMaxChunkSize) * 1.0f / lTotalSize;
			} else {
				// content request completed

				// Create the directory if it didn't exist.
				auto FileDirectory = FPaths::GetPath(Request.Path);
				FileDirectory = FPaths::ConvertRelativePathToFull(FileDirectory);
				if (!PlatformFile.DirectoryExists(*FileDirectory)) {
					if (!PlatformFile.CreateDirectoryTree(*FileDirectory)) {
						LogWarningF("failed to create directory");
					}
				}

				IFileHandle* FileHandle = PlatformFile.OpenWrite(*Request.Path, true, false);
				if (FileHandle) {
					auto& content = InResponse->GetContent();
					if (!FileHandle->Write(content.GetData(), content.Num())) {
						// write was unsuccessful
						Complete(false, Errors::FailedToWriteToFile);
						return;
					}
					delete FileHandle;
				} else {
					// file not opened
					Complete(false, Errors::FailedToOpenFile);
					return;
				}
				lCurrentChunk++;
				lChunkOffset = 0;

				if (lCurrentChunk == lTotalChunks) {
					// add data successfully downloaded
					Complete(true, FString{});
					return;
				}

			}

			OnChunk.ExecuteIfBound({Request.Id, lTotalSize, fProgress});
		} else {
			LogWarningF("Request to '%s' failed with code %d: %s",
				*InRequest->GetURL(), ResponseCode, *InResponse->GetContentAsString());
			Complete(false, lTotalSize == -1l ? Errors::FailedLengthRequest : Errors::FailedToRequestNextChunk);
		}
	}
}


FFileDownloadTask::FFileDownloadTask(FFileDownloadTask&& other) {
	HttpRequest = other.HttpRequest;
	other.HttpRequest = nullptr;
	// proper moving of bound delegates
	if (HttpRequest) {
		if (HttpRequest->OnProcessRequestComplete().IsBound()) {
			HttpRequest->OnProcessRequestComplete().Unbind();
			HttpRequest->OnProcessRequestComplete().BindRaw(this, &FFileDownloadTask::HttpRequestComplete);
		}
		if (HttpRequest->OnRequestProgress().IsBound()) {
			HttpRequest->OnRequestProgress().Unbind();
			HttpRequest->OnRequestProgress().BindRaw(this, &FFileDownloadTask::HttpRequestProgress);
		}
	}

	lCurrentChunk = other.lCurrentChunk;
	lTotalChunks = other.lTotalChunks;
	lTotalSize = other.lTotalSize;
	lMaxChunkSize = other.lMaxChunkSize;
	lChunkOffset = other.lChunkOffset;

	bProgressing = other.bProgressing;
	bCompleted = other.bCompleted;
	Request = MoveTempIfPossible(other.Request);
	Result = MoveTempIfPossible(other.Result);
	OnProgress = MoveTemp(other.OnProgress);
	OnChunk = MoveTemp(other.OnChunk);
	OnComplete = MoveTemp(other.OnComplete);
	OnHeader = MoveTemp(other.OnHeader);
}

FFileDownloadTask& FFileDownloadTask::operator=(FFileDownloadTask&& other) {
	HttpRequest = other.HttpRequest;
	other.HttpRequest = nullptr;
	// proper moving of bound delegates
	if (!HttpRequest->OnProcessRequestComplete().IsBound()) {
		HttpRequest->OnProcessRequestComplete().Unbind();
		HttpRequest->OnProcessRequestComplete().BindRaw(this, &FFileDownloadTask::HttpRequestComplete);
	}
	if (!HttpRequest->OnRequestProgress().IsBound()) {
		HttpRequest->OnRequestProgress().Unbind();
		HttpRequest->OnRequestProgress().BindRaw(this, &FFileDownloadTask::HttpRequestProgress);
	}

	lCurrentChunk = other.lCurrentChunk;
	lTotalChunks = other.lTotalChunks;
	lTotalSize = other.lTotalSize;
	lMaxChunkSize = other.lMaxChunkSize;
	lChunkOffset = other.lChunkOffset;

	bProgressing = other.bProgressing;
	bCompleted = other.bCompleted;
	Request = MoveTempIfPossible(other.Request);
	Result = MoveTempIfPossible(other.Result);
	OnProgress = MoveTemp(other.OnProgress);
	OnChunk = MoveTemp(other.OnChunk);
	OnComplete = MoveTemp(other.OnComplete);
	OnHeader = MoveTemp(other.OnHeader);

	return *this;
}
