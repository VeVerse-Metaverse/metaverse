// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.
#pragma once

#include "UIWidgetBase.h"

#include "Blueprint/IUserObjectListEntry.h"
#include "Components/TextBlock.h"
#include "ApiObjectMetadata.h"
#include "ApiPersonaMetadata.h"
#include "Components/UILoadingWidget.h"
#include "Components/Border.h"
#include "Components/SizeBox.h"
#include "UIAvatarListItemWidget.generated.h"

/**
 * 
 */
UCLASS()
class VEUI_API UUIAvatarListItemWidget final : public UUIWidgetBase, public IUserObjectListEntry {
	GENERATED_BODY()

	void SetIsLoading(bool bInIsLoading) const;

	//virtual void NativeOnTextureLoaded(const FTextureLoadResult& InResult) override;

	FDelegateHandle OnGetCurrenPersonaMetadataDelegateHandle;

public:
	virtual void NativeConstruct() override;

	/** Called when the tile view releases this widget. */
	virtual void NativeOnEntryReleased() override;

	virtual void RegisterCallbacks() override;
	virtual void UnregisterCallbacks() override;

	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;

	/** Called when the tile view sending the metadata object to be used in this widget. */
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;

	void SetCurrentPersonaId(const FGuid PersonaId);

	TSharedPtr<FApiPersonaMetadata> Metadata;
	
	FString ImageUrl;

	FGuid CurrentPersonaId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsActive = false;

	UPROPERTY(EditAnywhere)
	UTexture2D* DefaultTexture;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UBorder* PreviewImageBorderWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	USizeBox* PreviewImageBoxWidget;

	/** Displays the preview image. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	class UUIImageWidget* PreviewImageWidget;

	/** Displays the preview image. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	class UUILogoImageWidget* LogoImageWidget;

	/** Displays object name. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UTextBlock* NameTextWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UUILoadingWidget* LoadingWidget;

	UPROPERTY(BlueprintReadOnly, Transient, Category="Widget|Animation", meta=(BindWidgetAnimOptional))
	UWidgetAnimation* HoverInAnimation;

	UPROPERTY(BlueprintReadOnly, Transient, Category="Widget|Animation", meta=(BindWidgetAnimOptional))
	UWidgetAnimation* HoverOutAnimation;

	UPROPERTY(BlueprintReadOnly, Transient, Category="Widget|Animation", meta=(BindWidgetAnimOptional))
	UWidgetAnimation* ActiveHoverInAnimation;

	UPROPERTY(BlueprintReadOnly, Transient, Category="Widget|Animation", meta=(BindWidgetAnimOptional))
	UWidgetAnimation* ActiveHoverOutAnimation;

protected:
	void CheckSelected(const FGuid PersonaId);
};
