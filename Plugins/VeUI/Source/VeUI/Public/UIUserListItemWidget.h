// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.
#pragma once


#include "UIWidgetBase.h"

#include "ManagedTextureRequester.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "ApiUserMetadata.h"
#include "UIUserListItemWidget.generated.h"


/**
 * 
 */
UCLASS(HideDropdown)
class VEUI_API UUIUserListItemWidget final : public UUIWidgetBase, public IUserObjectListEntry{
	GENERATED_BODY()

public:
	/** Displays the user avatar image. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	class UUIImageWidget* PreviewImageWidget;

	/** Displays user name. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UTextBlock* NameTextWidget;

	/** Displays user level. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UTextBlock* LevelTextWidget;

	/** Displays user rank. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UTextBlock* RankTextWidget;

	/** Displays user rank. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UTextBlock* RatingTextWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	class UUILoadingWidget* LoadingWidget;


	UPROPERTY(EditAnywhere)
	UTexture2D* DefaultTexture;

protected:
	virtual void NativeConstruct() override;
	
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;

	/** Called when the tile view releases this widget. */
	virtual void NativeOnEntryReleased() override;

	TSharedPtr<FApiUserMetadata> Metadata;

};
