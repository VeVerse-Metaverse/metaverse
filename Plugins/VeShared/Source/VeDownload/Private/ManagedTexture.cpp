// Author: Egor A. Pristavka. Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#include "ManagedTexture.h"
#include "VeShared.h"

FManagedTexture::FManagedTexture() {
	Url = TEXT("");
	TexturePtr = nullptr;
}

FManagedTexture::FManagedTexture(const FString& InUrl, const TWeakObjectPtr<UTexture> InTexture): Url(InUrl), TexturePtr(InTexture) {
}

FManagedTexture::~FManagedTexture() {
	References.Empty();
	ClearTexture();
}

void FManagedTexture::AddReference(IManagedTextureRequester* Object) {
	References.AddUnique(Object);
}

bool FManagedTexture::RemoveReference(IManagedTextureRequester* Object) {
	References.Remove(Object);

	if (References.Num() <= 0) {
		ClearTexture();
		return true;
	}

	return false;
}

void FManagedTexture::ClearTexture() {
	if (TexturePtr.IsValid()) {
	    auto* Texture = TexturePtr.Get();
		if (IsValid(Texture)) {
			Texture->RemoveFromRoot();
			Texture->MarkAsGarbage();
			Texture->ConditionalBeginDestroy();
			TexturePtr.Reset();
		}
	} else {
		LogVerboseF("texture is already cleared");
	}
}
