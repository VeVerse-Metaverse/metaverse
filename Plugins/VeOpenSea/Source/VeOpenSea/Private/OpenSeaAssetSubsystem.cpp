// Author: Egor A. Pristavka. Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#include "OpenSeaAssetSubsystem.h"
#include "OpenSeaAssetRequest.h"
#include "OpenSeaCommon.h"
#include "VeDownload.h"
#include "VeOpenSea/VeOpenSeaModule.h"
#include "VeShared.h"
#include "FileHeaderRequest.h"
#include "DownloadChunkSubsystem.h"

const FName FOpenSeaAssetSubsystem::Name = FName("OpenSeaAssetSubsystem");

void FOpenSeaAssetSubsystem::Initialize() {
}

void FOpenSeaAssetSubsystem::Shutdown() {}

bool FOpenSeaAssetSubsystem::GetOwnerAssets(const FString& OwnerAddress, TSharedPtr<FOnOpenSeaAssetBatchRequestCompleted> Callback) {
	FOpenSeaAssetBatchRequestMetadata Metadata;
	Metadata.Owner = OwnerAddress;
	return GetBatch(Metadata, Callback);
}

bool FOpenSeaAssetSubsystem::GetBatch(const FOpenSeaAssetBatchRequestMetadata& InMetadata, const TSharedPtr<FOnOpenSeaAssetBatchRequestCompleted> InCallback) {
	if (InCallback && InCallback->IsBound()) {
		TSharedPtr<FOpenSeaAssetBatchRequest> BatchRequest{MakeShareable(new FOpenSeaAssetBatchRequest)};
		BatchRequest->RequestMetadata = InMetadata;

		BatchRequest->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest>& InRequest, const bool bInSuccessful) {
			FOpenSeaAssetBatchMetadata ResponseMetadata{};
			if (bInSuccessful) {
				if (const auto AssetBatchRequest{StaticCastSharedPtr<FOpenSeaAssetBatchRequest>(InRequest)}) {
					ResponseMetadata = AssetBatchRequest->ResponseMetadata;
				}
			}

			if (InCallback->IsBound()) {
				InCallback->Execute(ResponseMetadata, bInSuccessful, TEXT(""));
			} else {
				LogErrorF("callback is not bound");
			}
		});

		return BatchRequest->Process();
	}
	return false;
}

void FOpenSeaAssetSubsystem::GetAsset(const FString& ContractAddress, const FString& TokenId, TSharedRef<FOnOpenSeaAssetRequestCompleted> InCallback) {
	// Create request.
	const auto Request = MakeShared<FOpenSeaAssetGetRequest>();

	// Set request metadata.
	Request->ContractAddress = ContractAddress;
	Request->TokenId = TokenId;

	// Bind request complete lambda.
	Request->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest>& InRequest, const bool bInSuccessful) {
		if (!bInSuccessful) {
			InCallback->ExecuteIfBound({}, false, InRequest->GetErrorString());
			return;
		}

		if (Request->ResponseMetadata.MimeType.IsEmpty()) {
			GET_MODULE_SUBSYSTEM(ChunkDownloadSubsystem, Download, Chunk);
			if (!ChunkDownloadSubsystem) {
				InCallback->ExecuteIfBound({}, false, TEXT(""));
				return;
			}

			const auto GetFileHeadersCallback = MakeShared<FOnFileHeaderRequestComplete>();
			GetFileHeadersCallback->BindLambda([=](const FFileHeaderResult& Result, const bool bSuccessful, const FString& Error) {
				if (!bSuccessful) {
					VeLogWarningFunc("Failed to get Mime Type: %s", *Error);
					Request->ResponseMetadata.MimeType = TEXT("application/octet-stream");
					InCallback->ExecuteIfBound(Request->ResponseMetadata, true, TEXT(""));
					return;
				}

				Request->ResponseMetadata.MimeType = Result.GetContentMimeType();
				InCallback->ExecuteIfBound(Request->ResponseMetadata, true, TEXT(""));
			});

			ChunkDownloadSubsystem->GetFileHeaders(Request->ResponseMetadata.ImageUrl, GetFileHeadersCallback);
			return;
		}

		InCallback->ExecuteIfBound(Request->ResponseMetadata, bInSuccessful, TEXT(""));
	});

	// Process request.
	if (!Request->Process()) {
		InCallback->ExecuteIfBound({}, false, TEXT("not processed"));
	}
}
