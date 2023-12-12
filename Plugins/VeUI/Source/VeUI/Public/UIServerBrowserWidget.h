// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.
#pragma once

#include "UIServerDetailWidget.h"
#include "UIServerFilterWidget.h"
#include "UIServerListWidget.h"
#include "UIWidgetBase.h"

#include "UIServerBrowserWidget.generated.h"

/**
 * 
 */
UCLASS()
class VEUI_API UUIServerBrowserWidget final : public UUIWidgetBase
 {
	GENERATED_BODY()
	
	virtual void RegisterCallbacks() override;
	virtual void UnregisterCallbacks() override;

	FDelegateHandle OnCloseButtonClickedDelegateHandle;

public:

	/** List of servers. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UUIServerListWidget* ServerListWidget;

	/** Server browser filters. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UUIServerFilterWidget* ServerFilterWidget;

	/** Selected server details. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UUIServerDetailWidget* ServerDetailWidget;
	
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UUIButtonWidget* CloseButtonWidget;
	
	DECLARE_EVENT(UUIFileBrowserWidget, FOnClosed);
	FOnClosed OnClosedDelegate;
};
