// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.

#pragma once

#include "Components/UIButtonWidget.h"
#include "Components/UIFormInputWidget.h"
#include "UIPageContentWidget.h"
#include "UIPlaceableUrlWidget.generated.h"

/**
 * 
 */
UCLASS()
class VEUI_API UUIPlaceableUrlWidget final : public UUIPageContentWidget {
	GENERATED_BODY()

	virtual void RegisterCallbacks() override;
	virtual void UnregisterCallbacks() override;


protected:
	
	FDelegateHandle OnCloseButtonClickedDelegateHandle;
	FDelegateHandle OnUrlPersonInputDelegateHandle;
	FDelegateHandle OnSubmitButtonClickedDelegateHandle;



public:
	//virtual void OnNavigate() override;

	virtual void NativeConstruct() override;
	
	void OnUrlTextCommittedCallback(const FText& InText, ETextCommit::Type InCommitMethod);
	void OnSubmitButtonClicked();
	void SetMimeType(const FString& InMimeType);
	
	FString MimeType;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UUIButtonWidget* CloseButtonWidget = nullptr;
	
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UUIFormInputWidget* UrlInputWidget = nullptr;
			
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UUIButtonWidget* SubmitButtonWidget;

	DECLARE_EVENT(UUIPlaceableUrlWidget, FOnClosed);
	
	DECLARE_EVENT_OneParam(UUIPlaceableUrlWidget, FOnUrlChanged, const FString& Url);
	FOnUrlChanged OnUrlChanged;

	FOnClosed OnClosedDelegate;

};

