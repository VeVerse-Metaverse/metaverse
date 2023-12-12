// Author: Egor A. Pristavka. Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#include "TextureLoadRequest.h"

#include "AnimatedTexture2D.h"
#include "VeShared.h"

FTextureLoadResult::FTextureLoadResult(const FString& InUrl, const TWeakObjectPtr<UTexture> InTexture, const bool bInSuccessful, const FString& InError): Url(InUrl), Texture(InTexture),
																																						  Error(InError), bSuccessful(bInSuccessful) {
}

UTexture2D* FTextureLoadResult::AsTexture2D() const {
	return Cast<UTexture2D>(Texture.Get());
}

UAnimatedTexture2D* FTextureLoadResult::AsAnimatedTexture2D() const {
	return Cast<UAnimatedTexture2D>(Texture.Get());
}

FTextureLoadRequest::FTextureLoadRequest(const FString& InUrl, const TEnumAsByte<::TextureGroup> InTextureGroup, const uint32 InMipCount, const bool bInOverwrite):
	Url(InUrl), TextureGroup(InTextureGroup), MipCount(InMipCount), bOverwrite(bInOverwrite) {
}

FTextureLoadComplete& FTextureLoadRequest::GetOnComplete() {
	return OnComplete;
}

void FTextureLoadRequest::OnTaskCompleted(const FTextureLoadResult& Result) const {
	if (OnComplete.IsBound()) {
		OnComplete.Execute(Result);
	} else {
		LogF("on complete delegate not bound");
	}
}
