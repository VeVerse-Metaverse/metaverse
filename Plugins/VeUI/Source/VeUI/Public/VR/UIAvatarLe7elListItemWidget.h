// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.
#pragma once

#include <ManagedTextureRequester.h>

#include "UIWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "ApiObjectMetadata.h"
#include "ApiPersonaMetadata.h"
#include "Components/UILoadingWidget.h"
#include "Components/Border.h"
#include "Components/SizeBox.h"
#include "UIAvatarLe7elListItemWidget.generated.h"

/**
 * 
 */
UCLASS()
class VEUI_API UUIAvatarLe7elListItemWidget final : public UUIWidget, public IUserObjectListEntry, public IManagedTextureRequester {
	GENERATED_BODY()

	void SetIsLoading(bool bInIsLoading) const;

	virtual void NativeOnTextureLoaded(const FTextureLoadResult& InResult) override;
	
	FDelegateHandle OnGetCurrenPersonaMetadataDelegateHandle;

	
public:
	virtual void NativeConstruct() override;
	
	virtual void RegisterCallbacks() override;
	virtual void UnregisterCallbacks() override;
	
	void SetCurrentPersonaId(FGuid PersonaId);
	/** Called when the tile view releases this widget. */
	virtual void NativeOnEntryReleased() override;

	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;

		
	FGuid CurrentPersonaId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsActive = false;
	
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UBorder* PreviewImageBorderWidget;
	
	UPROPERTY(BlueprintReadOnly, Transient, Category="Widget|Animation", meta=(BindWidgetAnimOptional))
	UWidgetAnimation* HoverInAnimation;

	UPROPERTY(BlueprintReadOnly, Transient, Category="Widget|Animation", meta=(BindWidgetAnimOptional))
	UWidgetAnimation* HoverOutAnimation;

	UPROPERTY(BlueprintReadOnly, Transient, Category="Widget|Animation", meta=(BindWidgetAnimOptional))
	UWidgetAnimation* ActiveHoverInAnimation;

	UPROPERTY(BlueprintReadOnly, Transient, Category="Widget|Animation", meta=(BindWidgetAnimOptional))
	UWidgetAnimation* ActiveHoverOutAnimation;

	/** Called when the tile view sending the metadata object to be used in this widget. */
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;

	TSharedPtr<FApiPersonaMetadata> Persona;

	UPROPERTY(EditAnywhere)
	UTexture2D* DefaultTexture;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	USizeBox* PreviewImageBoxWidget;

	/** Displays the preview image. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UImage* PreviewImageWidget;

	/** Displays object name. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UTextBlock* NameTextWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUILoadingWidget* LoadingWidget;
};
