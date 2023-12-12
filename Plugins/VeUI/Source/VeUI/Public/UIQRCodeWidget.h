// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.
#pragma once

#include "UIWidgetBase.h"

#include "UIQRCodeWidget.generated.h"

class UImage;
/**
 * 
 */
UCLASS()
class VEUI_API UUIQRCodeWidget final : public UUIWidgetBase
 {
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

public:
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UImage* QrCodeWidget;

	UFUNCTION(BlueprintCallable)
	void Refresh();
};
