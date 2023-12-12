// Author: Egor A. Pristavka. Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#pragma once


class UAnimatedTexture2D;
struct FManagedTexture;

struct VEDOWNLOAD_API FTextureLoadResult {
	explicit FTextureLoadResult(const FString& InUrl = TEXT(""), TWeakObjectPtr<UTexture> InTexture = nullptr, const bool bInSuccessful = true, const FString& InError = TEXT(""));

	/** Url used to load the texture. */
	FString Url;

	/** Resolved texture. */
	TWeakObjectPtr<UTexture> Texture;

	/** Error string. */
	FString Error;

	/** Is the request resolved and the texture is ready to use. */
	bool bResolved = false;

	/** Was successful or not. */
	bool bSuccessful = false;

	/** Was downloaded from the internet. */
	bool bDownloaded = false;

	/** Was loaded from the memory cache. */
	bool bMemoryCached = false;

	/** Was loaded from the disk cache. */
	bool bDiskCached = false;

	/** Get texture as texture 2d */
	UTexture2D* AsTexture2D() const;

	/** Get texture as animated texture 2d */
	UAnimatedTexture2D* AsAnimatedTexture2D() const;
};

DECLARE_DELEGATE_OneParam(FTextureLoadComplete, const FTextureLoadResult& /* Result */)

class FTextureLoadRequest {
public:
	/**
	 * @param InUrl the URL to load the texture from.
	 * @param InTextureGroup the texture group to assign to the loaded texture.
	 * @param InMipCount count of mipmaps to generate, for UI textures which are not intended to be scaled at a distance can be set to zero.
	 * @param bInOverwrite should the texture already cached to the disk be re-downloaded and overwritten. Not used.
	 */
	explicit FTextureLoadRequest(const FString& InUrl,
		const TEnumAsByte<TextureGroup> InTextureGroup = TEXTUREGROUP_World,
		const uint32 InMipCount = 8,
		const bool bInOverwrite = false);

	/** Url to download from. */
	FString Url;

	/** Texture group to use. */
	TEnumAsByte<TextureGroup> TextureGroup = TEXTUREGROUP_World;

	/** How many mips to generate. Depends on the size of the original texture, but maximum of 8 is recommended as it already produces small 8 to 32 pixel wide images. */
	uint32 MipCount = 8;

	/** Overwrite disk cache or not. */
	bool bOverwrite = false;

	/** Texture load request complete single-cast delegate. */
	FTextureLoadComplete& GetOnComplete();

	/** Callback for asynchronous task completed delegate. Called when the texture is resolved. */
	void OnTaskCompleted(const FTextureLoadResult& Result) const;

	FORCEINLINE bool friend operator==(const FTextureLoadRequest& Lhs, const FTextureLoadRequest& Rhs) {
		return Lhs.Url == Rhs.Url;
	}

	FORCEINLINE bool friend operator!=(const FTextureLoadRequest& Lhs, const FTextureLoadRequest& Rhs) {
		return Lhs.Url != Rhs.Url;
	}

private:
	/** On complete delegate. */
	FTextureLoadComplete OnComplete;
};
