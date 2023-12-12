// Author: Egor A. Pristavka. Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#include "TextureLoadAsyncTask.h"

#include "ApiCommon.h"
#include "VeApi2.h"
#include "Api2AuthSubsystem.h"
#include "DDSLoader.h"
#include "FileDownloadTask.h"
#include "HttpModule.h"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "Engine/TextureCube.h"
#include "GenericPlatform/GenericPlatformHttp.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "VeShared.h"
#include "AnimatedTexture2D.h"

static FString GetCachedFilePath(const FString& InUrl) {
	FSHAHash OutHash;
	FSHA1 HashState;
	HashState.UpdateWithString(*InUrl, InUrl.Len());
	HashState.Final();
	HashState.GetHash(&OutHash.Hash[0]);
	return FPaths::Combine(FPaths::ProjectDir(), TEXT("Cache"), OutHash.ToString());
}

FTextureLoadAsyncTask::FTextureLoadAsyncTask(const TSharedPtr<FTextureLoadRequest> InRequest)
	: Request(InRequest), Texture(nullptr), AsyncTaskCompleteSyncEvent(nullptr),
	  DownloadCompleteEvent(nullptr) {}

FTextureLoadAsyncTask::~FTextureLoadAsyncTask() {
	if (OnComplete.IsBound()) {
		OnComplete.Unbind();
	}
}

bool FTextureLoadAsyncTask::CanAbandon() {
	return true;
}

void FTextureLoadAsyncTask::Abandon() {
	bShouldAbandon = true;

	if (DownloadCompleteEvent) {
		DownloadCompleteEvent->Trigger();
	}

	if (AsyncTaskCompleteSyncEvent) {
		AsyncTaskCompleteSyncEvent->Trigger();
	}

	if (PreSignedUrlRequestCompleteEvent) {
		PreSignedUrlRequestCompleteEvent->Trigger();
	}

	if (HttpRequest) {
		HttpRequest->CancelRequest();
	}
}

void FTextureLoadAsyncTask::PreSignedUrlHttpRequestComplete(FHttpRequestPtr InRequest, FHttpResponsePtr InResponse, const bool bSuccess) {
	const int32 ResponseCode = InResponse->GetResponseCode();

	if (PreSignedUrlHttpRequest->OnProcessRequestComplete().IsBound()) {
		PreSignedUrlHttpRequest->OnProcessRequestComplete().Unbind();
	}

	if (PreSignedUrlHttpRequest->OnRequestProgress().IsBound()) {
		PreSignedUrlHttpRequest->OnRequestProgress().Unbind();
	}

	if (!bSuccess || !(ResponseCode == Http::Code::OK || ResponseCode == Http::Code::Created)) {
		Result = FTextureLoadResult(Request->Url, nullptr, false, TEXT("failed to download the texture, invalid pre-signed URL HTTP request response code"));
		Result.bResolved = true;
	}

	if (InResponse.IsValid()) {
		PreSignedUrl = InResponse->GetHeader("Location");
		if (PreSignedUrl.IsEmpty()) {
			PreSignedUrl = InResponse->GetContentAsString();
		}
	} else {
		Result = FTextureLoadResult(Request->Url, nullptr, false, TEXT("failed to download the texture, invalid pre-signed URL HTTP request response"));
		Result.bResolved = true;
	}

	// Continue processing;
	PreSignedUrlRequestCompleteEvent->Trigger();
}

void FTextureLoadAsyncTask::DoWork() {
	if (bShouldAbandon) {
		return;
	}

	if (!Request) {
		LogErrorF("no request at async task");
		return;
	}

	// Load image wrapper module for image processing.
	IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));

	// Create image wrappers.
	TSharedPtr<IImageWrapper> ImageWrappers[3] =
	{
		ImageWrapperModule.CreateImageWrapper(EImageFormat::JPEG),
		ImageWrapperModule.CreateImageWrapper(EImageFormat::PNG),
		ImageWrapperModule.CreateImageWrapper(EImageFormat::EXR),
	};

	// Uncompressed texture bytes.
	auto RawData = MakeShared<TArray64<uint8>>();

#pragma region Local File

	// Check if URL is a local file path.
	if (Request->Url.StartsWith("file:///")) {

		constexpr auto ProtocolLen = 8; // strlen("file:///")

		auto FilePath = Request->Url.RightChop(ProtocolLen);

		// Check if file exists.
		if (FPaths::FileExists(FilePath)) {

			// Try to load file from the cache.
			TArray<uint8> DiskCacheFileBytes;

			// Try to load the file.
			if (FFileHelper::LoadFileToArray(DiskCacheFileBytes, *FilePath)) {

				// Find a matching image wrapper for the image data.
				for (auto ImageWrapper : ImageWrappers) {
					// Try to set compressed image bytes to the image wrapper. Will fail if image wrapper does not match the file format.
					if (ImageWrapper.IsValid() && ImageWrapper->SetCompressed(DiskCacheFileBytes.GetData(), DiskCacheFileBytes.Num())) {

						// Try to extract raw image bytes.
						if (ImageWrapper->GetRaw(ERGBFormat::BGRA, 8, *RawData)) {

							const auto Width = ImageWrapper->GetWidth();
							const auto Height = ImageWrapper->GetHeight();

							// Create a new transient texture.
							if (!IsValid(Texture)) {
								if (IsGarbageCollecting()) {
									LogErrorF("cannot create a texture while garbage collecting");
									return;
								}

								// LogVerboseF("creating texture in the asynchronous thread");
								Texture = UTexture2D::CreateTransient(Width, Height);
								Texture->AddToRoot();
							}

							// Copy raw bytes to the texture.
							if (IsValid(Texture)) {
								FTaskTagScope Scope(ETaskTag::EParallelRenderingThread);

								Texture->CompressionSettings = TC_Default;
								Texture->SRGB = true;

								// Cast to UTexture2D to have access to the platform data 
								UTexture2D* Texture2D = Cast<UTexture2D>(Texture);
								if (!Texture2D) {
									LogErrorF("invalid texture class for mipmaps generation");
									return;
								}

								// Copy raw image data to the texture.
								const auto TextureData = Texture2D->GetPlatformData()->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
								FMemory::Memcpy(TextureData, RawData->GetData(), RawData->Num());
								Texture2D->GetPlatformData()->Mips[0].BulkData.Unlock();

								GenerateMipmaps();

								if (IsValid(Texture)) {
									Texture->UpdateResource();
								}

								Result = FTextureLoadResult(Request->Url, Texture);
								Result.bResolved = true;
								Result.bDiskCached = true;

								// Notify that the texture has been loaded.
								ExecuteCompleteDelegate();

								// Stop the thread as we loaded the texture from the cache.
								return;
							}

							LogWarningF("failed to create a texture in async thread");
						}
					}
				}
			}
		}
	}

#pragma endregion

#pragma region Cache

	// Disk cache file path.
	const FString CachedFilePath = GetCachedFilePath(Request->Url);

	// Check if file exists.
	if (FPaths::FileExists(CachedFilePath)) {

		// Try to load file from the cache.
		TArray<uint8> DiskCacheFileBytes;

		// Try to load the file.
		if (FFileHelper::LoadFileToArray(DiskCacheFileBytes, *CachedFilePath)) {

			// Find a matching image wrapper for the image data.
			for (auto ImageWrapper : ImageWrappers) {
				// Try to set compressed image bytes to the image wrapper. Will fail if image wrapper does not match the file format.
				if (ImageWrapper.IsValid() && ImageWrapper->SetCompressed(DiskCacheFileBytes.GetData(), DiskCacheFileBytes.Num())) {

					// Try to extract raw image bytes.
					if (ImageWrapper->GetRaw(ERGBFormat::BGRA, 8, *RawData)) {

						const auto Width = ImageWrapper->GetWidth();
						const auto Height = ImageWrapper->GetHeight();

						// Create a new transient texture.
						if (!IsValid(Texture)) {
							if (IsGarbageCollecting()) {
								LogErrorF("cannot create a texture while garbage collecting");
								return;
							}

							// LogVerboseF("creating texture in the asynchronous thread");
							Texture = UTexture2D::CreateTransient(Width, Height);
							Texture->AddToRoot();
						}

						// Copy raw bytes to the texture.
						if (IsValid(Texture)) {
							FTaskTagScope Scope(ETaskTag::EParallelRenderingThread);

							Texture->CompressionSettings = TC_Default;
							Texture->SRGB = true;

							// Cast to UTexture2D to have access to the platform data 
							UTexture2D* Texture2D = Cast<UTexture2D>(Texture);
							if (!Texture2D) {
								LogErrorF("invalid texture class for mipmaps generation");
								return;
							}

							// Copy raw image data to the texture.
							const auto TextureData = Texture2D->GetPlatformData()->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
							FMemory::Memcpy(TextureData, RawData->GetData(), RawData->Num());
							Texture2D->GetPlatformData()->Mips[0].BulkData.Unlock();

							GenerateMipmaps();

							if (IsValid(Texture)) {
								Texture->UpdateResource();
							}

							Result = FTextureLoadResult(Request->Url, Texture);
							Result.bResolved = true;
							Result.bDiskCached = true;

							// Notify that the texture has been loaded.
							ExecuteCompleteDelegate();

							// Stop the thread as we loaded the texture from the cache.
							return;
						}

						LogWarningF("failed to create a texture in async thread");
					}
				}
			}
		}
	}

#pragma endregion

#pragma region Download

	if (PreSignedUrl.IsEmpty()) {
		PreSignedUrlHttpRequest = FHttpModule::Get().CreateRequest();

		if (Request->Url.StartsWith("https://veverse-") && Request->Url.Contains(".s3") && Request->Url.Contains("amazonaws.com/")) {

			PreSignedUrlRequestCompleteEvent = FGenericPlatformProcess::GetSynchEventFromPool(false);
			if (PreSignedUrlRequestCompleteEvent == nullptr) {
				Result = FTextureLoadResult(Request->Url, nullptr, false, TEXT("failed to download the texture, failed to create a sync event"));
				Result.bResolved = true;
				return;
			}

			PreSignedUrlHttpRequest = FHttpModule::Get().CreateRequest();
			check(PreSignedUrlHttpRequest != nullptr);

			if (!PreSignedUrlHttpRequest->OnProcessRequestComplete().IsBound()) {
				PreSignedUrlHttpRequest->OnProcessRequestComplete().BindRaw(this, &FTextureLoadAsyncTask::PreSignedUrlHttpRequestComplete);
			}

			PreSignedUrlHttpRequest->SetURL(Api::GetApi2RootUrl() / TEXT("files") / TEXT("download-pre-signed-url") + TEXT("?url=") + FGenericPlatformHttp::UrlEncode(Request->Url));

			// Client authorization
			if (PreSignedUrlHttpRequest->GetHeader(TEXT("Authorization")).IsEmpty()) {
				GET_MODULE_SUBSYSTEM(AuthSubsystem, Api2, Auth);
				if (AuthSubsystem) {
					const auto Token = AuthSubsystem->GetSessionToken();
					if (!Token.IsEmpty()) {
						PreSignedUrlHttpRequest->SetHeader(TEXT("Authorization"), FString::Printf(TEXT("Bearer %s"), *Token));
					}
				}
			}

			if (PreSignedUrlHttpRequest->GetHeader(Api::Headers::ContentType).IsEmpty()) {
				PreSignedUrlHttpRequest->SetHeader(Api::Headers::ContentType, Api::ContentTypes::ApplicationJson);
			}

			PreSignedUrlHttpRequest->ProcessRequest();

			PreSignedUrlRequestCompleteEvent->Wait();
			FGenericPlatformProcess::ReturnSynchEventToPool(PreSignedUrlRequestCompleteEvent);
		} else {
			PreSignedUrl = Request->Url;
		}
	}

	// Create the download complete event.
	DownloadCompleteEvent = FGenericPlatformProcess::GetSynchEventFromPool(false);
	if (DownloadCompleteEvent == nullptr) {
		Result = FTextureLoadResult(Request->Url, nullptr, false, TEXT("failed to download the texture, failed to create a sync event"));
		Result.bResolved = true;
		return;
	}

	if (PreSignedUrl.IsEmpty()) {
		Result = FTextureLoadResult(Request->Url, nullptr, false, TEXT("failed to download the texture, failed to get a pre-signed url"));
		Result.bResolved = true;
		return;
	}

	// Send an http request.
	HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->OnProcessRequestComplete().BindRaw(this, &FTextureLoadAsyncTask::OnDownloadRequestComplete);
	HttpRequest->OnRequestProgress().BindRaw(this, &FTextureLoadAsyncTask::OnDownloadRequestProgress);
	HttpRequest->SetURL(PreSignedUrl);
	HttpRequest->SetVerb(TEXT("GET"));
	HttpRequest->ProcessRequest();

	// Wait for download to complete, will unlock in OnDownloadImageRequestComplete method.
	DownloadCompleteEvent->Wait();
	FGenericPlatformProcess::ReturnSynchEventToPool(DownloadCompleteEvent);

	// Abandon async task.
	if (bShouldAbandon) {
		Result = FTextureLoadResult(Request->Url, nullptr, false, TEXT("task cancelled"));
		Result.bResolved = true;
		return;
	}

	if (HttpResponse.IsValid()) {

		// Handle animated GIF textures
		if (HttpResponse->GetContentAsString().StartsWith(TEXT("GIF8"))) {
			if (!FFileHelper::SaveArrayToFile(HttpResponse->GetContent(), *CachedFilePath)) {
				LogWarningF("failed to save the animated texture file to %s", *CachedFilePath);
			} else {
				UAnimatedTexture2D* AnimTexture = Cast<UAnimatedTexture2D>(NewObject<UAnimatedTexture2D>(GetTransientPackage(), NAME_None, RF_Transient));
				if (IsValid(AnimTexture)) {
					AnimTexture->ImportFile(HttpResponse->GetContent().GetData(), HttpResponse->GetContentLength());
					/*const FTextureResource* Resource =*/
					AnimTexture->CreateResource();
					Texture = AnimTexture;

					Result = FTextureLoadResult(Request->Url, AnimTexture);
					Result.bResolved = true;
					Result.bDownloaded = true;

					// Engine checks that the update resource is called in the game thread, if we try to do it in background, checks will cause exceptions in debug builds.
					AsyncTask(ENamedThreads::GameThread, [this] {
						Texture->UpdateResource();
					});

					ExecuteCompleteDelegate();

					return;
				}

				LogErrorF("failed to create animated texture");
			}
			// Handle cubemap texture
		} else if (HttpResponse->GetContentAsString().StartsWith(TEXT("#?RADIANCE"))) {
			for (auto ImageWrapper : ImageWrappers) {

				TArray<uint8> Buffer;
				if (FFileHelper::LoadFileToArray(Buffer, *CachedFilePath)) {

					EImageFormat Format = ImageWrapperModule.DetectImageFormat(Buffer.GetData(), Buffer.GetAllocatedSize());

					if (Format != EImageFormat::Invalid) {
						EPixelFormat PixelFormat;
						ERGBFormat RGBFormat;

						const auto BitDepth = ImageWrapper->GetBitDepth();
						const auto Width = ImageWrapper->GetWidth();
						const auto Height = ImageWrapper->GetHeight();

						if (BitDepth == 16) {
							PixelFormat = PF_FloatRGBA;
							RGBFormat = ERGBFormat::BGRA;
						} else if (BitDepth == 8) {
							PixelFormat = PF_B8G8R8A8;
							RGBFormat = ERGBFormat::BGRA;
						} else {
							LogWarningF("Error creating texture. Bit depth is unsupported. (%d)", BitDepth);
							return;
						}

						TArray64<uint8> UncompressedData;
						ImageWrapper->GetRaw(RGBFormat, BitDepth, UncompressedData);

						UTexture2D* TextureCube = UTexture2D::CreateTransient(Width, Height, PixelFormat);
						if (TextureCube) {
							TextureCube->bNotOfflineProcessed = true;
							uint8* MipData = static_cast<uint8*>(TextureCube->GetPlatformData()->Mips[0].BulkData.Lock(LOCK_READ_WRITE));

							// Bulk data was already allocated for the correct size when we called CreateTransient above
							FMemory::Memcpy(MipData, UncompressedData.GetData(), TextureCube->GetPlatformData()->Mips[0].BulkData.GetBulkDataSize());

							TextureCube->GetPlatformData()->Mips[0].BulkData.Unlock();

							TextureCube->UpdateResource();
						} else {
							LogWarningF("Error creating texture. Couldn't determine the file format");
						}
					}
				}
			}
		}
		// Handle common 2d textures
		else {
			// Find a matching image wrapper for the image data.
			for (auto ImageWrapper : ImageWrappers) {
				// Try to set compressed image bytes to the image wrapper. Will fail if image wrapper does not match the file format.
				if (ImageWrapper.IsValid() && ImageWrapper->SetCompressed(HttpResponse->GetContent().GetData(), HttpResponse->GetContentLength())) {

					// Try to extract raw image bytes.
					if (ImageWrapper->GetRaw(ERGBFormat::BGRA, 8, *RawData)) {
						const auto Width = ImageWrapper->GetWidth();
						const auto Height = ImageWrapper->GetHeight();

						if (!FFileHelper::SaveArrayToFile(HttpResponse->GetContent(), *CachedFilePath)) {
							LogWarningF("Failed to save the texture file to: %s", *CachedFilePath);
						}

						UTexture2D* Texture2D = UTexture2D::CreateTransient(Width, Height);

						if (IsValid(Texture2D)) {
							FTaskTagScope Scope(ETaskTag::EParallelRenderingThread);

							// Assign texture 2d as the task result texture 
							Texture = Texture2D;

							Texture2D->CompressionSettings = TC_Default;
							Texture2D->SRGB = true;
							Texture2D->LODGroup = Request->TextureGroup;

							const auto TextureData = Texture2D->GetPlatformData()->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
							FMemory::Memcpy(TextureData, RawData->GetData(), RawData->Num());
							Texture2D->GetPlatformData()->Mips[0].BulkData.Unlock();

							GenerateMipmaps();

							Result = FTextureLoadResult(Request->Url, Texture2D);
							Result.bResolved = true;
							Result.bDownloaded = true;

							if (IsValid(Texture)) {
								Texture->UpdateResource();
							}

							ExecuteCompleteDelegate();

							return;
						}

						LogErrorF("failed to create a texture");
					}
				}
			}
		}
	}
#pragma endregion

	LogErrorF("failed to load");
	if (HttpResponse) {
		Result = FTextureLoadResult(Request->Url, nullptr, false,
			FString::Printf(TEXT("failed to load the texture: %d, %s"), HttpResponse->GetResponseCode(), *HttpResponse->GetContentAsString()));
		Result.bResolved = true;
	} else {
		Result = FTextureLoadResult(Request->Url, nullptr, false, FString::Printf(TEXT("failed to load the texture: %s"), *Request->Url));
		Result.bResolved = true;
	}

	ExecuteCompleteDelegate();
}

void FTextureLoadAsyncTask::GenerateMipmaps() const {
	// LogVerboseF("generating mipmaps");

	const int32 Width = Texture->GetSurfaceWidth();
	const int32 Height = Texture->GetSurfaceHeight();

	// Cast to UTexture2D to have access to the platform data 
	UTexture2D* Texture2D = Cast<UTexture2D>(Texture);
	if (!Texture2D) {
		LogErrorF("invalid texture class for mipmaps generation");
		return;
	}

	// Texture bytes.
	TArray<uint8> TextureByteArray;
	TextureByteArray.AddUninitialized(Texture2D->GetPlatformData()->Mips[0].BulkData.GetElementCount());
	FMemory::Memcpy(TextureByteArray.GetData(), Texture2D->GetPlatformData()->Mips[0].BulkData.Lock(LOCK_READ_ONLY), Texture2D->GetPlatformData()->Mips[0].BulkData.GetBulkDataSize());
	Texture2D->GetPlatformData()->Mips[0].BulkData.Unlock();

	int32 MipsToGenerate = Request->MipCount;

	TArray<uint8> MipDataA;
	TArray<uint8> MipDataB;

	uint8* LastMipData = TextureByteArray.GetData();
	int32 LastMipWidth = Width;
	int32 LastMipHeight = Height;

	while (MipsToGenerate > 0) {
		constexpr int32 BytesPerPixel = 4;
		const int32 MipWidth = LastMipWidth >> 1;
		const int32 MipHeight = LastMipHeight >> 1;

		TArray<uint8>* MipBytes = MipsToGenerate & 1 ? &MipDataA : &MipDataB;

		if (MipWidth <= 0 || MipHeight <= 0) {
			break;
		}

		MipBytes->Reset();
		MipBytes->AddUninitialized(MipWidth * MipHeight * BytesPerPixel);

		const int32 RowDataLen = LastMipWidth * BytesPerPixel;

		uint8* OutData = MipBytes->GetData();

		for (int32 Y = 0; Y < MipHeight; Y++) {
			auto* CurrentRowData = LastMipData + RowDataLen * Y * 2;
			auto* NextRowData = CurrentRowData + RowDataLen;

			for (int32 X = 0; X < MipWidth; X++) {
				int32 TotalB = *CurrentRowData++;
				int32 TotalG = *CurrentRowData++;
				int32 TotalR = *CurrentRowData++;
				int32 TotalA = *CurrentRowData++;

				TotalB += *CurrentRowData++;
				TotalG += *CurrentRowData++;
				TotalR += *CurrentRowData++;
				TotalA += *CurrentRowData++;

				TotalB += *NextRowData++;
				TotalG += *NextRowData++;
				TotalR += *NextRowData++;
				TotalA += *NextRowData++;
				TotalB += *NextRowData++;
				TotalG += *NextRowData++;
				TotalR += *NextRowData++;
				TotalA += *NextRowData++;

				TotalB >>= 2;
				TotalG >>= 2;
				TotalR >>= 2;
				TotalA >>= 2;

				*OutData++ = static_cast<uint8>(TotalB);
				*OutData++ = static_cast<uint8>(TotalG);
				*OutData++ = static_cast<uint8>(TotalR);
				*OutData++ = static_cast<uint8>(TotalA);
			}
		}

		FTexture2DMipMap* Mip = new FTexture2DMipMap();
		Texture2D->GetPlatformData()->Mips.Add(Mip);

		Mip->SizeX = MipWidth;
		Mip->SizeY = MipHeight;

		Mip->BulkData.Lock(LOCK_READ_WRITE);
		void* MipData = Mip->BulkData.Realloc(MipBytes->Num());
		FMemory::Memcpy(MipData, MipBytes->GetData(), MipBytes->Num());
		Mip->BulkData.Unlock();

		LastMipData = MipBytes->GetData();
		LastMipWidth = MipWidth;
		LastMipHeight = MipHeight;
		MipsToGenerate--;
	}
}

void FTextureLoadAsyncTask::ExecuteCompleteDelegate() {
	// LogVerboseF("exec complete delegate");
	AsyncTask(ENamedThreads::GameThread, [this] {
		// LogVerboseF("indeed exec complete delegate at the game thread");
		if (OnComplete.IsBound()) {
			// LogVerboseF("delegate bound, broadcasting");
			OnComplete.Execute(Result);
		} else {
			// LogVerboseF("delegate not bound");
		}
		if (AsyncTaskCompleteSyncEvent) {
			AsyncTaskCompleteSyncEvent->Trigger();
		}
	});

	AsyncTaskCompleteSyncEvent = FGenericPlatformProcess::GetSynchEventFromPool(false);
	if (!AsyncTaskCompleteSyncEvent) {
		// Result = FTextureLoadResult(Request->Url, nullptr, false, TEXT("failed to download the texture"));
		return;
	}

	AsyncTaskCompleteSyncEvent->Wait();
	FGenericPlatformProcess::ReturnSynchEventToPool(AsyncTaskCompleteSyncEvent);
	// LogVerboseF("completed task");
}

void FTextureLoadAsyncTask::OnDownloadRequestComplete(FHttpRequestPtr /*InHttpRequest*/, const FHttpResponsePtr InHttpResponse, const bool bSucceeded) {
	if (bSucceeded && InHttpResponse.IsValid() && InHttpResponse->GetContentLength() > 0) {
		HttpResponse = InHttpResponse;
	}

	// Continue processing;
	DownloadCompleteEvent->Trigger();
}

// ReSharper disable once CppMemberFunctionMayBeStatic
void FTextureLoadAsyncTask::OnDownloadRequestProgress(FHttpRequestPtr /*HttpRequest*/, int32 /*BytesSent*/, int32 /*InBytesReceived*/) {}
