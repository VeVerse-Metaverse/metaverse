// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.
#pragma once

#include "Components/UIButtonWidget.h"
#include "UIWidgetBase.h"
#include "UIOpenSeaWidget.generated.h"


UCLASS()
class VEUI_API UUIOpenSeaWidget final : public UUIWidgetBase
 {
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	virtual void RegisterCallbacks() override;
	virtual void UnregisterCallbacks() override;

	
	FDelegateHandle OnCloseButtonClickedDelegateHandle;
	
	
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIButtonWidget* CloseButtonWidget;
	

	DECLARE_EVENT(UUIFileBrowserWidget, FOnClosed);
	FOnClosed OnClosedDelegate;

	//void GetMods(const IApiBatchQueryRequestMetadata& RequestMetadata = IApiBatchQueryRequestMetadata()) const;

};
