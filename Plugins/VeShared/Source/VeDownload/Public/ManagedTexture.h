// Author: Egor A. Pristavka. Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#pragma once

#include "ManagedTextureRequester.h"

struct FManagedTexture {
	FManagedTexture();
	explicit FManagedTexture(const FString& InUrl, TWeakObjectPtr<UTexture> InTexture);
	~FManagedTexture();

	/** The texture original URL. Used as the texture identifier. */
	FString Url;

	/** Stored texture. */
	TWeakObjectPtr<UTexture> TexturePtr = nullptr;

	/** Objects referencing the texture. */
	TArray<IManagedTextureRequester*> References;

	/** Adds a reference to the texture. */
	void AddReference(IManagedTextureRequester* Object);

	/** Removes a reference to the object. */
	bool RemoveReference(IManagedTextureRequester* Object);

	int32 GetReferenceNum() const { return References.Num(); }
	
	/** Clears the memory used by the texture. */
	void ClearTexture();
};

typedef TSharedPtr<FManagedTexture> FManagedTexturePtr;
