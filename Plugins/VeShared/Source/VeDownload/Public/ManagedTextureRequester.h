// Author: Egor A. Pristavka. Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#pragma once
#include "TextureLoadRequest.h"

class VEDOWNLOAD_API IManagedTextureRequester {
public:
	IManagedTextureRequester();

	virtual ~IManagedTextureRequester();

	/**
	 * Use this method to request the texture.
	 * @param Context
	 * @param InUrl URL to load the texture from
	 * @param InTextureGroup texture group for the newly loaded texture. UI group will produce no mipmaps.
	 */
	void RequestTexture(UObject* Context, const FString& InUrl, TEnumAsByte<enum TextureGroup> InTextureGroup = TEXTUREGROUP_World);

	/**
	 * Use this method to request the texture. This version allows to manually specify required settings. 
	 * @param Context uobject context 
	 * @param InRequest shared pointer to the request to use.
	 */
	void RequestTexture(UObject* Context, TSharedPtr<FTextureLoadRequest> InRequest);

	/**
	 * Use this method to manually remove a single texture reference.
	 * @param InUrl URL that is used to identify the texture to be released.
	 */
	void ReleaseTexture(const FString& InUrl);

	/**
	 * Use this method to manually remove all textures referenced by this object, e.g. on the actor EndPlay event.
	 * Called automatically when the requester is destroyed.
	 */
	void ReleaseAllTextures();

protected:
	/**
	 * @brief Virtual handler called when the texture is requested. Must be implemented in derived classes.
	 */
	virtual void NativeOnTextureRequested(TSharedPtr<FTextureLoadRequest> InRequest) {};

	/** Virtual handler called when the texture is ready. Must be implemented in derived classes. */
	virtual void NativeOnTextureLoaded(const FTextureLoadResult& Result) {};

	/** Clear previous request. */
	void ClearRequest();

private:
	/** Called when the texture load is completed. */
	void OnTextureLoadCompleted(const FTextureLoadResult& Result);

	/** Current request, cached so we could unbind delegate if the new request made. */
	TSharedPtr<FTextureLoadRequest> Request;

protected:
	bool bUsingAnyTextures = false;

	FString CurrentTextureUrl;

	/** Previous request, cached so we could clear the texture reference after loading a new one. */
	FString PreviousTextureUrl;
};

DECLARE_MULTICAST_DELEGATE_OneParam(FManagedTextureLoaded, const FTextureLoadResult& /* Result */)

/**
 * Managed texture requester implementation for proxy objects.
 */
class VEDOWNLOAD_API FManagedTextureRequester final : public IManagedTextureRequester {

private:
	FManagedTextureLoaded OnTextureLoaded;

	/**
	 * @brief Cached result of the texture load.
	 */
	FTextureLoadResult Result;

	/**
	 * @brief Flag indicating that the texture load has been completed.
	 */
	bool bBusy = false;

public:
	FManagedTextureLoaded& GetOnTextureLoaded() {
		return OnTextureLoaded;
	}

	const FTextureLoadResult& GetResult() const {
		return Result;
	}

	bool IsBusy() const {
		return bBusy;
	}

private:
	/** Virtual handler called when the texture is ready. Notifies subscribers that the texture has been loaded. */
	virtual void NativeOnTextureLoaded(const FTextureLoadResult& InResult) override;

	/** Virtual handler called when the texture is requested. */
	virtual void NativeOnTextureRequested(TSharedPtr<FTextureLoadRequest> InRequest) override;
};
