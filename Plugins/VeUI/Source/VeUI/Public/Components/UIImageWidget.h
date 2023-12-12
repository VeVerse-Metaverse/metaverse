// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.

#pragma once

#include "CoreFwd.h"
#include "UIWidgetBase.h"
#include "ManagedTextureRequester.h"
#include "ApiFileMetadata.h"
#include "VeFileMetadata.h"
#include "Components/Image.h"
#include "UIImageWidget.generated.h"


UCLASS()
class VEUI_API UUIImageWidget : public UUIWidgetBase, public IManagedTextureRequester {
	GENERATED_BODY()

	DECLARE_EVENT(UUIImageWidget, FOnUIImageWidgetEvent);

public:
#pragma region Widgets

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UImage* ImageWidget;

	/** Blocks the preview image during loading. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UUILoadingWidget* LoadingWidget;

#pragma endregion Widgets

	UPROPERTY(EditAnywhere, Category="UIImage", meta=(DeprecatedProperty, DeprecationMessage="Use of texture asset for default texture is deprecated. Use FallbackTexture URL instead."))
	UTexture2D* DefaultTexture;

	UPROPERTY(EditAnywhere, Category="UIImage")
	bool LoadingVisible;

	const FString& GetURL() const {
		return CurrentURL;
	}

	UFUNCTION(BlueprintCallable)
	bool ShowImage(const FString& InURL);
	bool ShowImage(const TArray<FApiFileMetadata>& Files, EApiFileType Type);
	bool ShowImage(const TArray<FVeFileMetadata>& Files, EApiFileType Type);
	/**
	 * @brief Display fallback/default texture.
	 */
	void ShowDefaultImage();

	/**
	 * @brief Load fallback/default texture.
	 */
	void LoadFallbackImage();

	void SetTexture(UTexture* Texture);

protected:
	virtual void NativeOnInitializedShared() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void SynchronizeProperties() override;
	virtual void NativeOnTextureLoaded(const FTextureLoadResult& InResult) override;

	FVector2D GetSize() const {
		return IsValid(ImageWidget) ? ImageWidget->GetDesiredSize() : FVector2D::ZeroVector;
	}

	FString CurrentURL;
	void SetIsLoading(bool bInIsLoading) const;

	FVector2D DesiredSize;

	/**
	 * @brief Proxy for the fallback/default texture requester.
	 */
	TSharedPtr<FManagedTextureRequester> FallbackTextureRequester;

	/**
	 * @brief Request fallback/default texture.
	 */
	void RequestFallbackTexture();

};
