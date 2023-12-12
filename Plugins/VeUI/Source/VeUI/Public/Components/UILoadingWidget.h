// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.
#pragma once

#include "ManagedTextureRequester.h"
#include "UIWidgetBase.h"

#include "UILoadingWidget.generated.h"

/**
 * 
 */
UCLASS()
class VEUI_API UUILoadingWidget final : public UUIWidgetBase, public IManagedTextureRequester {
	GENERATED_BODY()

	virtual void NativeOnTextureLoaded(const FTextureLoadResult& InResult) override;
	void SetTexture(UTexture2D* Texture);

public:
	UPROPERTY(BlueprintReadOnly, Transient, Category="Widget|Animation", meta=(BindWidgetAnimOptional))
	UWidgetAnimation* LoadingAnimation;

	/** Displays the user avatar image. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UImage* PreviewImageWidget;

	FString ImageUrl;

protected:
	virtual void NativeConstruct() override;

};
