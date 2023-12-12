// Authors: Egor A. Pristavka, Nikolay Bulatov. 
// Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "UI/UIEditorOpenSeaUrlPropertiesWidget.h"

#include "OpenSeaAssetMetadata.h"
#include "VeDownload.h"
#include "VeShared.h"
#include "VeGameModule.h"
#include "Components/Image.h"
#include "VeOpenSea/VeOpenSeaModule.h"
#include "AnimatedTexture2D.h"


void UUIEditorOpenSeaUrlPropertiesWidget::NativeOnTextureLoaded(const FTextureLoadResult& InResult) {
	if (!InResult.bSuccessful || !InResult.Texture.IsValid()) {
		return;
	}

	// SetTexture(InResult.Texture);
	ASSIGN_WIDGET_TEXTURE(PreviewImageWidget, InResult);
}

void UUIEditorOpenSeaUrlPropertiesWidget::NativeOnInitialized() {
	// Listen for URL commit events.
	if (IsValid(UrlInputWidget)) {
		if (!UrlInputWidget->OnTextCommitted.IsBound()) {
			UrlInputWidget->OnTextCommitted.AddWeakLambda(this, [this](const FText& InText, const ETextCommit::Type InCommitMethod) {
				OnUrlTextCommittedCallback(InText, InCommitMethod);
			});
		}
	}

	Super::NativeOnInitialized();
}

void UUIEditorOpenSeaUrlPropertiesWidget::OnUrlTextCommittedCallback(const FText& InText, ETextCommit::Type /*InCommitMethod*/) {
	if (!InText.IsEmpty()) {
		Url = InText.ToString();
		if (Url.Contains(TEXT("opensea.io"))) {
			GET_MODULE_SUBSYSTEM(ChunkDownloadSubsystem, Download, Chunk);
			if (ChunkDownloadSubsystem) {
				TArray<FString> Tokens;
				Url.ParseIntoArray(Tokens,TEXT("/"), true);
				if (Tokens.Num() > 2) {
					const int32 ContractAddressIndex = Tokens.Num() - 2;
					const int32 TokenIdIndex = Tokens.Num() - 1;
					const auto ContractAddress = Tokens[ContractAddressIndex];
					if (ContractAddress.StartsWith(TEXT("0x"))) {
						const auto TokenId = Tokens[TokenIdIndex];
						GET_MODULE_SUBSYSTEM(OpenSeaAssetSubsystem, OpenSea, Asset);
						if (OpenSeaAssetSubsystem) {
							// 1. Create a callback.
							const auto OpenSeaAssetCallbackPtr = MakeShared<FOnOpenSeaAssetRequestCompleted>();
							// 2. Bind callback.
							OpenSeaAssetCallbackPtr->BindWeakLambda(this, [=](const FOpenSeaAssetMetadata& InMetadata, const bool bInSuccessful, const FString& InError) {
								if (!bInSuccessful) {
									LogErrorF("failed to request: %s", *InError);
								}

								if (InMetadata.ImageOriginalUrl.IsEmpty()) {
									RequestTexture(this, InMetadata.ImageUrl, TEXTUREGROUP_UI);
								} else {
									RequestTexture(this, InMetadata.ImageOriginalUrl, TEXTUREGROUP_UI);
								}
							});
							// 3. Make the request using the callback.
							OpenSeaAssetSubsystem->GetAsset(ContractAddress, TokenId, OpenSeaAssetCallbackPtr);
						}
					}
				}
			}
		} else {
			RequestTexture(this, Url, TEXTUREGROUP_UI);
		}
		SetSaveEnabled(!InText.IsEmpty());
	}
}


void UUIEditorOpenSeaUrlPropertiesWidget::SetUrl(const FString& InUrl) {
	Url = InUrl;
	UrlInputWidget->SetText(FText::FromString(Url));

	SetSaveEnabled(!Url.IsEmpty());
	if (!Url.IsEmpty()) {
		RequestTexture(this, Url, TEXTUREGROUP_UI);
	}
}

void UUIEditorOpenSeaUrlPropertiesWidget::SetOriginalUrl(const FString& InUrl) {
	OriginalUrl = InUrl;
	UrlInputWidget->SetText(FText::FromString(OriginalUrl));
	
}

FString UUIEditorOpenSeaUrlPropertiesWidget::GetUrl() {
	return Url;
}
