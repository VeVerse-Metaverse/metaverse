// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "Download1Task.h"

#include "HttpModule.h"
#include "VeApi2.h"
#include "VeShared.h"
#include "VeDownload.h"
#include "Interfaces/IHttpResponse.h"
#include "FileDownloadTask.h"
#include "Api2AuthSubsystem.h"


namespace {
	/** Max content length supported by UE HTTP requests. */
	constexpr uint64 MaxContentLength = 1ull << 31;
}

FVeDownload1Task::FVeDownload1Task(const FDownload1RequestMetadata& InRequestMetadata) {
	VeLogTestFunc("enter");

	RequestMetadata = InRequestMetadata;
	ResultMetadata = {};
	StartTime = FPlatformTime::Seconds();
}

void FVeDownload1Task::ResetHttpRequest() {
	VeLogTestFunc("enter");

	if (HttpRequest) {
		VeLogTestFunc("resetting existing http request");
		HttpRequest->OnProcessRequestComplete().Unbind();
		HttpRequest->OnRequestProgress().Unbind();
		HttpRequest->CancelRequest();
		HttpRequest.Reset();
	}

	HttpRequest = FHttpModule::Get().CreateRequest();
#if DEPRECATED
	HttpRequest->OnRequestProgress().BindRaw(this, &FVeDownloadTask::HttpRequestProgress);
	HttpRequest->OnProcessRequestComplete().BindRaw(this, &FVeDownloadTask::HttpRequestComplete);
	HttpRequest->SetVerb(Http::Method::Get);
	HttpRequest->SetURL(RequestMetadata.PreSignedUrl.IsEmpty() ? RequestMetadata.Url : RequestMetadata.PreSignedUrl);
#endif
}

EDownload1Code FVeDownload1Task::Process() {
	VeLogTestFunc("enter");

	if (RequestMetadata.bUsePreSignedUrl) {
		return RequestPreSignedDownloadLink();
	}

	return RequestHead();
}

void FVeDownload1Task::SetRangeHeader(const int64 Start, const int64 End) const {
	VeLogTestFunc("range header: %lld-%lld", Start, End);

	FString RangeHeaderValue{};

	if (Start >= 0 && End >= 0) {
		RangeHeaderValue = FString::Printf(TEXT("bytes=%lld-%lld"), Start, End);
	} else if (Start >= 0) {
		RangeHeaderValue = FString::Printf(TEXT("bytes=%lld-"), Start);
	} else if (End >= 0) {
		RangeHeaderValue = FString::Printf(TEXT("bytes=-%lld"), End);
	}

	if (HttpRequest && !RangeHeaderValue.IsEmpty()) {
		HttpRequest->SetHeader(Http::Header::Range, RangeHeaderValue);
	}
}

void FVeDownload1Task::Complete(const EDownload1Code Code, const FString& InError) {
	VeLogTestFunc("%d: %s", Code, *InError);

	if (bCompleted) {
		return;
	}

	bCompleted = true;

	CompleteTime = FPlatformTime::Seconds();
	VeLogTestFunc("completed in %.3f seconds", CompleteTime - StartTime);

	ResultMetadata.RequestMetadata = RequestMetadata;
	ResultMetadata.Time = CompleteTime - StartTime;
	ResultMetadata.ContentLength = DownloadedContentLength;
	ResultMetadata.DownloadCode = Code;
	ResultMetadata.Error = InError;

	VeLogTestFunc("progress callback check");
	if (DownloadTaskProgressCallback.IsBound()) {
		VeLogTestFunc("progress callback");
		DownloadTaskProgressCallback.Execute({RequestMetadata, ContentLength, Progress});
	}

	VeLogTestFunc("complete callback check");
	if (DownloadTaskCompleteCallback.IsBound()) {
		VeLogTestFunc("complete callback");
		DownloadTaskCompleteCallback.Execute(ResultMetadata);
	}
}

void FVeDownload1Task::Cancel(const bool bDeleteFile) {
	if (bDeleteFile) {
		IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
		if (PlatformFile.FileExists(*RequestMetadata.Path)) {
			PlatformFile.DeleteFile(*RequestMetadata.Path);
		}
	}

	if (bCompleted) {
		return;
	}

	// Cancel the HTTP request
	if (HttpRequest && HttpRequest->GetStatus() == EHttpRequestStatus::Processing) {
		HttpRequest->CancelRequest();
	}

	// Reset the task progress that it will require new request for the file size
	ContentLength = -1;

	Complete(EDownload1Code::Cancelled, TEXT("download has been cancelled"));
}

void FVeDownload1Task::HttpContentRequestProgress(FHttpRequestPtr /*InRequest*/, const int32 /*BytesSent*/, const int32 BytesReceived) {
	if (ContentLength == -1) {
		return;
	}

	if (const float NewProgress = (BytesReceived + BlockOffset + BlockIndex * BlockSize) * 1.0f / ContentLength; NewProgress - Progress > ProgressThreshold) {
		Progress = NewProgress;
		VeLogTestFunc("progress: %.2f%%", 100 * Progress);

		VeLogFunc("progress callback check");
		if (DownloadTaskProgressCallback.IsBound()) {
			VeLogTestFunc("progress callback");
			DownloadTaskProgressCallback.Execute({RequestMetadata, ContentLength, Progress});
		}
	}
}

#if 0
void FVeDownloadTask::HttpRequestComplete(const FHttpRequestPtr InRequest, const FHttpResponsePtr InResponse, const bool bSuccessful) {
	VeLogTestFunc("reset processing");
	// Reset processing status
	{
		VeLogTestFunc("ProcessingLock W");
		[[maybe_unused]] FRWScopeLock Lock(ProcessingLock, SLT_Write);
		bProcessing = false;
	}

	if (!bSuccessful) {
		VeLogTestFunc("http request failed");
		Complete(EDownloadCode::Error, FString::Printf(TEXT("failed to download a file block: %d, %s"), InResponse->GetResponseCode(), *InResponse->GetContentAsString()));
		return;
	}

	if (InResponse) {
		const int32 ResponseCode = InResponse->GetResponseCode();
		VeLogTestFunc("http response code: %d", ResponseCode);
		if (ResponseCode == Http::Code::PartialContent || ResponseCode == Http::Code::OK) {
			if (ContentLength == -1) {
				// Size request completed
				const auto ContentRangeHeader = InResponse->GetHeader(Http::Header::ContentRange);
				FString ContentLengthStr;
				ContentRangeHeader.Split(TEXT("/"), nullptr, &ContentLengthStr);
				ContentLength = FCString::Atoi64(*ContentLengthStr);
				BlockNum = (ContentLength - 1l) / BlockSize + 1l;

				VeLogTestFunc("size request completed, total: %s", *ContentLengthStr);

				if (RequestMetadata.Path.IsEmpty()) {
					FSHA1 Hash;
					Hash.UpdateWithString(*RequestMetadata.Url, RequestMetadata.Url.Len());
					RequestMetadata.Path = FString::Printf(TEXT("%s/dlcache/%s"), *FPaths::ProjectPersistentDownloadDir(), *Hash.Finalize().ToString());
				}
				VeLogTestFunc("download path: %s", *RequestMetadata.Path);

				// Download the file to disk
				IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
				if (PlatformFile.FileExists(*RequestMetadata.Path)) {
					VeLogTestFunc("file exists");
					if (RequestMetadata.Mode == EDownloadMode::Overwrite) {
						VeLogTestFunc("overwriting file");
						if (!PlatformFile.DeleteFile(*RequestMetadata.Path)) {
							VeLogWarningFunc("failed to delete file");
						}
						BlockIndex = 0;
						BlockOffset = 0;
					} else {
						VeLogTestFunc("continue download");

						// Continue download
						const auto FileSize = PlatformFile.FileSize(*RequestMetadata.Path);
						VeLogTestFunc("File size: %lld; Server size: %lld", FileSize, ContentLength);
						if (FileSize > ContentLength) {
							VeLogFunc("file size > total size, redownload");
							if (!PlatformFile.DeleteFile(*RequestMetadata.Path)) {
								VeLogWarningFunc("failed to delete file");
								Complete(EDownloadCode::Error, "failed to delete file");
								return;
							}
							BlockIndex = 0;
							BlockOffset = 0;
						} else {
							if (FileSize == ContentLength) {
								VeLogFunc("file present, size match, skip download");
								Complete(EDownloadCode::Completed, {});
								return;
							}
							BlockIndex = FileSize / BlockSize;
							BlockOffset = FileSize % BlockSize;
						}
					}
				} else {
					VeLogFunc("file is missing, proceed with download");

					// File does not exist yet, download and write to a new file
					BlockIndex = 0;
					BlockOffset = 0;
				}

				Progress = static_cast<float>(BlockOffset + BlockIndex * BlockSize) / static_cast<float>(ContentLength);
				VeLogFunc("download progress %.3f", Progress);

				VeLogFunc("block callback check");
				if (DownloadTaskBlockCallback.IsBound()) {
					VeLogFunc("block callback");
					DownloadTaskBlockCallback.Execute({RequestMetadata, ContentLength, Progress});
				}

			} else {
				// Content block request completed

				VeLogFunc("content request completed");

				// Create the directory if it didn't exist.
				auto Dir = FPaths::GetPath(RequestMetadata.Path);
				Dir = FPaths::ConvertRelativePathToFull(Dir);

				IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
				if (!PlatformFile.DirectoryExists(*Dir)) {
					VeLogFunc("creating directory tree");
					if (!PlatformFile.CreateDirectoryTree(*Dir)) {
						VeLogWarningFunc("failed to create directory");
					}
				}

				VeLogFunc("opening a file");
				if (IFileHandle* FileHandle = PlatformFile.OpenWrite(*RequestMetadata.Path, true, false)) {
					auto& Content = InResponse->GetContent();
					VeLogFunc("writing to the file");
					if (!FileHandle->Write(Content.GetData(), Content.Num())) {
						Complete(EDownloadCode::Error, FString::Printf(TEXT("failed to write to file: %s"), *RequestMetadata.Path));
						delete FileHandle;
						return;
					}
					delete FileHandle;
				} else {
					Complete(EDownloadCode::Error, FString::Printf(TEXT("failed to open a file for writing: %s"), *RequestMetadata.Path));
					return;
				}

				BlockIndex++;
				BlockOffset = 0;

				VeLogFunc("block callback check");
				if (DownloadTaskBlockCallback.IsBound()) {
					VeLogFunc("block callback");
					DownloadTaskBlockCallback.Execute({RequestMetadata, ContentLength, Progress});
				}

				if (BlockIndex > BlockNum) {
					VeLogErrorFunc("current block > total block num");
				}

				if (BlockIndex >= BlockNum) {
					VeLogFunc("all blocks downloaded");
					Progress = 1.0f;
					Complete(EDownloadCode::Completed, {});
				}
			}
		} else {
			VeLogWarningFunc("request to %s failed with code %d: %s", *InRequest->GetURL(), ResponseCode, *InResponse->GetContentAsString());
			Complete(EDownloadCode::Error, FString::Printf(TEXT("%s: %d, %s"), ContentLength ? TEXT("failed to request file length") : TEXT("failed to request a next block"), InResponse->GetResponseCode(), *InResponse->GetContentAsString()));
		}
	}
}
#endif

bool FVeDownload1Task::SetPreSignedLinkAuthorizationHeaders() const {
	if (!HttpRequest) {
		VeLogTestErrorFunc("error: no http request");
		return false;
	}

	HttpRequest->SetHeader("X-Metaverse-Id", "00000000-0000-0000-0000-000000000000");
	HttpRequest->SetHeader("X-Metaverse-Key", "00000000-0000-0000-0000-000000000000");

	if (HttpRequest->GetHeader(TEXT("Authorization")).IsEmpty()) {
		GET_MODULE_SUBSYSTEM(AuthSubsystem, Api2, Auth);
		if (AuthSubsystem) {
			const auto Token = AuthSubsystem->GetSessionToken();
			if (!Token.IsEmpty()) {
				HttpRequest->SetHeader(TEXT("Authorization"), FString::Printf(TEXT("Bearer %s"), *Token));
			}
		}
	}

	return true;
}

bool FVeDownload1Task::SetPreSignedLinkRequestUrl() const {
	FString EntityId, FileId;
	if (RequestMetadata.EntityId.IsValid() && RequestMetadata.FileId.IsValid()) {
		EntityId = RequestMetadata.EntityId.ToString(EGuidFormats::DigitsWithHyphensLower);
		FileId = RequestMetadata.FileId.ToString(EGuidFormats::DigitsWithHyphensLower);
	} else {
		// Try to parse the entity id and file id from the URL of format "https://bucket.region.amazonaws.com/entity-id/file-id"
		TArray<FString> Out;
		RequestMetadata.Url.ParseIntoArray(Out, TEXT("/"), true);

		// Valid URL should give us 4 tokens ["https:", "bucket.region.amazonaws.com", "entity-id", "file-id"]
		if (Out.Num() == 4) {
			FString Token = Out[2];
			if (FGuid Guid; FGuid::Parse(Token, Guid)) {
				EntityId = Guid.ToString(EGuidFormats::DigitsWithHyphensLower);
			} else {
				return false;
			}

			Token = Out[3];
			if (FGuid Guid; FGuid::Parse(Token, Guid)) {
				FileId = Guid.ToString(EGuidFormats::DigitsWithHyphensLower);
			} else {
				return false;
			}
		} else {
			return false;
		}
	}

	HttpRequest->SetURL(FString::Printf(TEXT("%s/files/download-pre-signed?entityId=%s&fileId=%s"), *Api::GetApi2RootUrl(), *EntityId, *FileId));

	return true;
}

EDownload1Code FVeDownload1Task::RequestPreSignedDownloadLink() {
	VeLogTestFunc("enter");
	if (RequestMetadata.Url.IsEmpty() && !(RequestMetadata.EntityId.IsValid() && RequestMetadata.FileId.IsValid())) {
		VeLogTestFunc("error: no url or entityId and fileId set");
		return EDownload1Code::Error;
	}

	if (bCompleted) {
		ensure(0);
		VeLogTestFunc("already completed, you should not be here");
		return EDownload1Code::Completed;
	}

	if (bProcessing) {
		ensure(0);
		VeLogTestFunc("already processing, you should not be here");
		return EDownload1Code::Processing;
	}

	ResetHttpRequest();
	HttpRequest->SetVerb("GET");
	HttpRequest->OnProcessRequestComplete().BindRaw(this, &FVeDownload1Task::HttpPreSignedUrlRequestComplete);
	HttpRequest->SetVerb(Http::Method::Get);

	if (!SetPreSignedLinkRequestUrl()) {
		VeLogTestFunc("failed to set pre-signed link request url");
		return EDownload1Code::Error;
	}

	if (!SetPreSignedLinkAuthorizationHeaders()) {
		VeLogTestFunc("failed to set authentication headers");
		return EDownload1Code::Error;
	}

	if (!HttpRequest->ProcessRequest()) {
		if (const auto Response = HttpRequest->GetResponse()) {
			VeLogTestFunc("failed to process HTTP request");
		} else {
			VeLogTestFunc("failed to process HTTP request: %d, %s", Response->GetResponseCode(), *Response->GetContentAsString());
		}
		return EDownload1Code::Error;
	}

	bProcessing = true;

	return EDownload1Code::Processing;
}

void FVeDownload1Task::HttpPreSignedUrlRequestComplete(FHttpRequestPtr InRequest, FHttpResponsePtr InResponse, const bool bSuccessful) {
	const int32 ResponseCode = InResponse->GetResponseCode();

	if (HttpRequest->OnProcessRequestComplete().IsBound()) {
		HttpRequest->OnProcessRequestComplete().Unbind();
	}

	if (!bSuccessful || ResponseCode != Http::Code::Created) {
		Complete(EDownload1Code::Error, InResponse->GetContentAsString());
		return;
	}

	if (InResponse) {
		// Store the pre-signed url we are going to use.
		RequestMetadata.PreSignedUrl = InResponse->GetHeader(TEXT("Location"));
		// Request headers for the pre-signed file download link. 
		if (const auto Code = RequestHead(); Code != EDownload1Code::Processing) {
			Complete(Code, "failed to request head");
		}
	} else {
		Complete(EDownload1Code::Error, "invalid signed url response");
	}
}

EDownload1Code FVeDownload1Task::RequestHead() {
	VeLogTestFunc("enter");

	if (bCompleted) {
		ensure(0);
		VeLogTestFunc("already completed, you should not be here");
		return EDownload1Code::Completed;
	}

	ResetHttpRequest();
	HttpRequest->OnProcessRequestComplete().BindRaw(this, &FVeDownload1Task::HttpHeadRequestComplete);
	if (RequestMetadata.bUsePreSignedUrl) {
		// We can't use HEAD verb with the pre-signed URLs, so just use the range header with 0-0 range.
		HttpRequest->SetVerb(Http::Method::Get);
		if (RequestMetadata.PreSignedUrl.IsEmpty()) {
			VeLogTestFunc("error: no pre-signed url");
			return EDownload1Code::Error;
		}
		HttpRequest->SetHeader(Http::Header::Range, TEXT("bytes=0-0"));
		HttpRequest->SetURL(RequestMetadata.PreSignedUrl);
	} else {
		HttpRequest->SetVerb(Http::Method::Head);
		if (RequestMetadata.Url.IsEmpty()) {
			VeLogTestFunc("error: no url");
			return EDownload1Code::Error;
		}

		HttpRequest->SetURL(RequestMetadata.Url);
	}

	if (!HttpRequest->ProcessRequest()) {
		if (const auto Response = HttpRequest->GetResponse()) {
			VeLogTestFunc("failed to process HTTP request");
		} else {
			VeLogTestFunc("failed to process HTTP request: %d, %s", Response->GetResponseCode(), *Response->GetContentAsString());
		}
		return EDownload1Code::Error;
	}

	bProcessing = true;

	return EDownload1Code::Processing;
}

void FVeDownload1Task::HttpHeadRequestComplete(FHttpRequestPtr InRequest, const FHttpResponsePtr InResponse, const bool bSuccessful) {
	VeLogTestFunc("enter");

#pragma region Response Validation

	if (!bSuccessful) {
		VeLogTestErrorFunc("error: HEAD request failed");
		Complete(EDownload1Code::Error, FString::Printf(TEXT("failed to HEAD: %d, %s"), InResponse->GetResponseCode(), *InResponse->GetContentAsString()));
		return;
	}

	if (!InResponse) {
		VeLogTestErrorFunc("error: HEAD request has no response");
		Complete(EDownload1Code::Error, FString::Printf(TEXT("failed to get HEAD response")));
		return;
	}

	const int32 ResponseCode = InResponse->GetResponseCode();
	VeLogTestFunc("HEAD response code: %d", ResponseCode);
	if (!(ResponseCode == Http::Code::OK || ResponseCode == Http::Code::PartialContent)) {
		VeLogTestErrorFunc("error: bad HEAD response code");
		Complete(EDownload1Code::Error, FString::Printf(TEXT("HEAD response error code: %d"), ResponseCode));
		return;
	}

#pragma endregion

#pragma region Content-Length Header
	
	if (RequestMetadata.bUsePreSignedUrl) {
		const auto ContentRangeHeader = InResponse->GetHeader(Http::Header::ContentRange);
		FString ContentLengthStr;
		ContentRangeHeader.Split(TEXT("/"), nullptr, &ContentLengthStr);
		ContentLength = FCString::Atoi64(*ContentLengthStr);
	} else {
		const auto ContentLengthHeader = InResponse->GetHeader(Http::Header::ContentLength);
		ContentLength = FCString::Atoi64(*ContentLengthHeader);

		// Store in the request metadata to be able to compare if actually downloaded content length matches server reported content length header.
		RequestMetadata.ContentLength = ContentLength;
	}

#pragma endregion

#pragma region Accept-Ranges Header

	if (const auto AcceptRangesHeader = InResponse->GetHeader(Http::Header::AcceptRanges); AcceptRangesHeader == "bytes") {
		VeLogTestFunc("server supports range header");
		bRangeHeaderSupported = true;

		// File size is bigger than the single block size, so use the range download. 
		if (RequestMetadata.bUseRange && ContentLength > BlockSize) {
			bUsingRangeHeader = true;

			// Range download supported, so calculate a block number.
			BlockNum = (ContentLength - 1l) / BlockSize + 1l;
		}
	} else {
		VeLogTestFunc("server doesn't support range header");
		bRangeHeaderSupported = false;
	}

	if (!bUsingRangeHeader) {
		// todo: will be fixed with streamed download, can't download big files using UE HTTP
		if (ContentLength > MaxContentLength) {
			VeLogTestFunc("file is too big: %lld", ContentLength);
			Complete(EDownload1Code::Error, FString::Printf(TEXT("server doesn't support block download, file is too big: %lld"), ContentLength));
			return;
		}
	}

#pragma endregion

#pragma region Download Path

	if (RequestMetadata.Path.IsEmpty()) {
		VeLogTestFunc("no download path provided, using hash");

		FSHA1 Hash;
		Hash.UpdateWithString(*RequestMetadata.Url, RequestMetadata.Url.Len());
		RequestMetadata.Path = FString::Printf(TEXT("%s/dlcache/%s"), *FPaths::ProjectPersistentDownloadDir(), *Hash.Finalize().ToString());
	}

	const auto* DownloadFilePath = *RequestMetadata.Path;

	auto DownloadDir = FPaths::GetPath(DownloadFilePath);
	DownloadDir = FPaths::ConvertRelativePathToFull(DownloadDir);

	if (IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile(); !PlatformFile.DirectoryExists(*DownloadDir)) {
		VeLogTestFunc("selected download path: %s", DownloadFilePath);
		if (!PlatformFile.CreateDirectoryTree(*DownloadDir)) {
			VeLogTestFunc("failed to create directory: %s", DownloadFilePath);
			Complete(EDownload1Code::Error, "failed to create directory");
			return;
		}
	}

	VeLogTestFunc("selected download path: %s", DownloadFilePath);

#pragma endregion

#pragma region File

	if (IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile(); PlatformFile.FileExists(DownloadFilePath)) {
		VeLogTestFunc("file exists at %s", DownloadFilePath);

		switch (RequestMetadata.Mode) {
		case EDownload1Mode::Overwrite: {
			VeLogTestFunc("deleting file at %s", DownloadFilePath);

			if (!PlatformFile.DeleteFile(DownloadFilePath)) {
				VeLogTestFunc("failed to delete file at %s", DownloadFilePath);
				Complete(EDownload1Code::Error, FString::Printf(TEXT("failed to delete file at %s"), DownloadFilePath));
				return;
			}
			break;
		}
		case EDownload1Mode::Resume: {
			if (bUsingRangeHeader) {
				VeLogTestFunc("resume download to the existing file: %s", DownloadFilePath);

				const auto FileSize = PlatformFile.FileSize(DownloadFilePath);

				VeLogTestFunc("local file size: %lld vs server file size: %lld", FileSize, ContentLength);

				if (FileSize > ContentLength) {
					VeLogTestFunc("file exists at %s, overwrite disabled", DownloadFilePath);
					Complete(EDownload1Code::Error, FString::Printf(TEXT("file exists at %s, overwrite disabled"), DownloadFilePath));
					return;
				}

				if (FileSize == ContentLength) {
					VeLogTestFunc("file size matches content length, nothing to do");
					Complete(EDownload1Code::Completed, {});
					return;
				}

				BlockIndex = FileSize / BlockSize;
				BlockOffset = FileSize % BlockSize;
			} else {
				VeLogTestFunc("file exists at %s, can't resume", DownloadFilePath);
				Complete(EDownload1Code::Error, FString::Printf(TEXT("file exists at %s, can't resume"), DownloadFilePath));
				return;
			}
			break;
		}
		}
	} else {
		VeLogTestFunc("new file, processing");

		if (bUsingRangeHeader) {
			BlockIndex = 0;
			BlockOffset = 0;
		}
	}

	if (bUsingRangeHeader) {
		RequestContentBlock();
	} else {
		RequestContent();
	}

#pragma endregion
}

EDownload1Code FVeDownload1Task::RequestContent() {
	VeLogTestFunc("enter");

	ResetHttpRequest();

	if (RequestMetadata.bUsePreSignedUrl) {
		if (RequestMetadata.PreSignedUrl.IsEmpty()) {
			VeLogTestErrorFunc("error: empty pre-signed url")
			Complete(EDownload1Code::Error, "invalid pre-signed url");
			return EDownload1Code::Error;
		}

		HttpRequest->SetURL(RequestMetadata.PreSignedUrl);
	} else {
		if (RequestMetadata.Url.IsEmpty()) {
			VeLogTestErrorFunc("error: empty pre-signed url")
			Complete(EDownload1Code::Error, "invalid pre-signed url");
			return EDownload1Code::Error;
		}

		HttpRequest->SetURL(RequestMetadata.Url);
	}

	HttpRequest->SetVerb(Http::Method::Get);
	HttpRequest->OnRequestProgress().BindRaw(this, &FVeDownload1Task::HttpContentRequestProgress);
	HttpRequest->OnProcessRequestComplete().BindRaw(this, &FVeDownload1Task::HttpContentRequestComplete);

	if (!HttpRequest->ProcessRequest()) {
		if (const auto Response = HttpRequest->GetResponse()) {
			VeLogTestFunc("failed to process HTTP request");
		} else {
			VeLogTestFunc("failed to process HTTP request: %d, %s", Response->GetResponseCode(), *Response->GetContentAsString());
		}
		return EDownload1Code::Error;
	}

	bProcessing = true;

	return EDownload1Code::Processing;
}

void FVeDownload1Task::HttpContentRequestComplete(const FHttpRequestPtr InRequest, const FHttpResponsePtr InResponse, const bool bSuccessful) {
	VeLogTestFunc("enter");

	if (!bSuccessful) {
		VeLogTestErrorFunc("http request failed");
		Complete(EDownload1Code::Error, FString::Printf(TEXT("failed to download a file: %d, %s"), InResponse->GetResponseCode(), *InResponse->GetContentAsString()));
		return;
	}

	if (!InResponse) {
		VeLogTestErrorFunc("invalid response");
		Complete(EDownload1Code::Error, FString::Printf(TEXT("invalid response")));
		return;
	}

	const auto ResponseCode = InResponse->GetResponseCode();
	VeLogTestFunc("http response code: %d", ResponseCode);
	if (ResponseCode != Http::Code::OK) {
		VeLogTestErrorFunc("request to %s failed with code %d: %s", *InRequest->GetURL(), ResponseCode, *InResponse->GetContentAsString());
		Complete(EDownload1Code::Error, FString::Printf(TEXT("failed to process response")));
		return;
	}

	const auto& Content = InResponse->GetContent();
	DownloadedContentLength += Content.Num();

	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	if (auto* FileHandle = PlatformFile.OpenWrite(*RequestMetadata.Path, false, false)) {
		VeLogTestFunc("writing content to file");
		if (!FileHandle->Write(Content.GetData(), Content.Num())) {
			Complete(EDownload1Code::Error, FString::Printf(TEXT("failed to write to file: %s"), *RequestMetadata.Path));
			delete FileHandle; // don't forget
			return;
		}
		delete FileHandle; // don't forget
	} else {
		Complete(EDownload1Code::Error, FString::Printf(TEXT("failed to open a file for writing: %s"), *RequestMetadata.Path));
		return;
	}

	Progress = 1.0f;
	Complete(EDownload1Code::Completed, {});
}

EDownload1Code FVeDownload1Task::RequestContentBlock() {
	VeLogTestFunc("enter");

	ResetHttpRequest();

	if (RequestMetadata.bUsePreSignedUrl) {
		if (RequestMetadata.PreSignedUrl.IsEmpty()) {
			VeLogTestErrorFunc("error: empty pre-signed url")
			Complete(EDownload1Code::Error, "invalid pre-signed url");
			return EDownload1Code::Error;
		}

		HttpRequest->SetURL(RequestMetadata.PreSignedUrl);
	} else {
		if (RequestMetadata.Url.IsEmpty()) {
			VeLogTestErrorFunc("error: empty pre-signed url")
			Complete(EDownload1Code::Error, "invalid pre-signed url");
			return EDownload1Code::Error;
		}

		HttpRequest->SetURL(RequestMetadata.Url);
	}

	HttpRequest->SetVerb(Http::Method::Get);
	HttpRequest->OnRequestProgress().BindRaw(this, &FVeDownload1Task::HttpContentRequestProgress);
	HttpRequest->OnProcessRequestComplete().BindRaw(this, &FVeDownload1Task::HttpContentBlockRequestComplete);

	const auto StartOffset = BlockIndex * BlockSize + BlockOffset;
	const auto EndOffset = (BlockIndex + 1) * BlockSize - 1;
	SetRangeHeader(StartOffset, EndOffset < ContentLength ? EndOffset : ContentLength - 1);

	if (!HttpRequest->ProcessRequest()) {
		if (const auto Response = HttpRequest->GetResponse()) {
			VeLogTestFunc("failed to process HTTP request");
		} else {
			VeLogTestFunc("failed to process HTTP request: %d, %s", Response->GetResponseCode(), *Response->GetContentAsString());
		}
		return EDownload1Code::Error;
	}

	bProcessing = true;

	return EDownload1Code::Processing;
}

void FVeDownload1Task::HttpContentBlockRequestComplete(FHttpRequestPtr InRequest, FHttpResponsePtr InResponse, const bool bSuccessful) {
	VeLogTestFunc("enter");

	if (!bSuccessful) {
		VeLogTestErrorFunc("http request failed");
		Complete(EDownload1Code::Error, FString::Printf(TEXT("failed to download a file: %d, %s"), InResponse->GetResponseCode(), *InResponse->GetContentAsString()));
		return;
	}

	if (!InResponse) {
		VeLogTestErrorFunc("invalid response");
		Complete(EDownload1Code::Error, FString::Printf(TEXT("invalid response")));
		return;
	}

	const auto ResponseCode = InResponse->GetResponseCode();
	VeLogTestFunc("http response code: %d", ResponseCode);
	if (!(ResponseCode == Http::Code::OK || ResponseCode == Http::Code::PartialContent)) {
		VeLogTestErrorFunc("request to %s failed with code %d: %s", *InRequest->GetURL(), ResponseCode, *InResponse->GetContentAsString());
		Complete(EDownload1Code::Error, FString::Printf(TEXT("failed to process response")));
		return;
	}

	auto& Content = InResponse->GetContent();

	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	if (auto* FileHandle = PlatformFile.OpenWrite(*RequestMetadata.Path, true, false)) {
		VeLogTestFunc("writing content to file");
		if (!FileHandle->Write(Content.GetData(), Content.Num())) {
			Complete(EDownload1Code::Error, FString::Printf(TEXT("failed to write to file: %s"), *RequestMetadata.Path));
			delete FileHandle; // don't forget
			return;
		}
		delete FileHandle; // don't forget
	} else {
		Complete(EDownload1Code::Error, FString::Printf(TEXT("failed to open a file for writing: %s"), *RequestMetadata.Path));
		return;
	}

	BlockIndex++;
	BlockOffset = 0;
	DownloadedContentLength += Content.Num();

	if (DownloadTaskBlockCallback.IsBound()) {
		DownloadTaskBlockCallback.Execute({RequestMetadata, ContentLength, Progress});
	}

	if (BlockIndex > BlockNum) {
		VeLogTestErrorFunc("current block > total block num: %lld > %lld", BlockIndex, BlockNum);
		Complete(EDownload1Code::Error, FString::Printf(TEXT("current block > total block num: %lld > %lld"), BlockIndex, BlockNum));
		return;
	}

	if (BlockIndex == BlockNum) {
		VeLogTestFunc("all blocks (%lld) downloaded", BlockNum);
		Progress = 1.0f;
		Complete(EDownload1Code::Completed, {});
		return;
	}

	RequestContentBlock();
}
