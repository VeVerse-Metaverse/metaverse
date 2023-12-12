// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.
#pragma once
#include "UIWidgetBase.h"

#include "Blueprint/IUserObjectListEntry.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "PlatformFileInfo.h"
#include "UIFileBrowserListItemWidget.generated.h"

/**
 * 
 */
UCLASS()
class VEUI_API UUIFileBrowserListItemWidget final : public UUIWidgetBase
, public IUserObjectListEntry {
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;

	FPlatformFileInfo File;

	/** Displays the preview image. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UImage* PreviewImageWidget;

	/** Displays file name. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UTextBlock* NameTextWidget;
};
