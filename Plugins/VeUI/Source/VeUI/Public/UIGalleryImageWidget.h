// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.
#pragma once

#include "UIWidgetBase.h"

#include "Components/Image.h"
#include "UIGalleryImageWidget.generated.h"

UCLASS()
class VEUI_API UUIGalleryImageWidget: public UUIWidgetBase
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UImage* PreviewImageWidget;

	UPROPERTY(EditAnywhere)
	UTexture2D* DefaultTexture;
};
