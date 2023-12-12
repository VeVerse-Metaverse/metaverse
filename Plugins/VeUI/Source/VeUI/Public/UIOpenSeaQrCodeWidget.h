// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.
#pragma once

#include "Components/UIButtonWidget.h"
#include "UIWidgetBase.h"

#include "UIOpenSeaQrCodeWidget.generated.h"


UCLASS()
class VEUI_API UUIOpenSeaQrCodeWidget final : public UUIWidgetBase
 {
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	virtual void RegisterCallbacks() override;
	virtual void UnregisterCallbacks() override;

	
	FDelegateHandle OnCloseButtonClickedDelegateHandle;
	FDelegateHandle OnAuthenticateMetamaskButtonClickedDelegateHandle;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIButtonWidget* CloseButtonWidget;
	
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIButtonWidget* AuthenticationMetamaskButtonWidget;

	DECLARE_EVENT(UUIFileBrowserWidget, FOnClosed);
	FOnClosed OnClosedDelegate;

	//void GetMods(const IApiBatchQueryRequestMetadata& RequestMetadata = IApiBatchQueryRequestMetadata()) const;

};
