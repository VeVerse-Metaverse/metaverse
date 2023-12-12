// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.

#pragma once

#include "IPlaceable.h"
#include "Components/UIButtonWidget.h"
#include "Components/UIFormInputWidget.h"
#include "UIPageContentWidget.h"
#include "Components/ComboBox.h"
#include "Components/ComboBoxString.h"
#include "UIPlaceableClassWidget.generated.h"

/**
 * 
 */
UCLASS()
class VEUI_API UUIPlaceableClassWidget final : public UUIPageContentWidget {
	GENERATED_BODY()

	virtual void RegisterCallbacks() override;
	virtual void UnregisterCallbacks() override;

protected:
	FDelegateHandle OnCloseButtonClickedDelegateHandle;
	// FDelegateHandle OnUrlPersonInputDelegateHandle;
	FDelegateHandle OnSubmitButtonClickedDelegateHandle;

public:
	//virtual void OnNavigate() override;
	UFUNCTION()
	void OnClassSelectionChanged(FString String, ESelectInfo::Type Arg);
	virtual void NativeConstruct() override;
	
	// void OnUrlTextCommittedCallback(const FText& InText, ETextCommit::Type InCommitMethod);
	void OnSubmitButtonClicked();
	void SetMimeType(const FString& InMimeType);
	
	FString MimeType;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UUIButtonWidget* CloseButtonWidget = nullptr;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UComboBoxString* PlaceableClassComboBoxWidget = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Widget|Content")
	TMap<FString, TSubclassOf<AActor>> OptionClassMap;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Widget|Content")
	TArray<FString> Options;

	// UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	// UUIFormInputWidget* UrlInputWidget = nullptr;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UUIButtonWidget* SubmitButtonWidget;

	DECLARE_EVENT(UUIPlaceableClassWidget, FOnClosed);
	
	DECLARE_EVENT_OneParam(UUIPlaceableClassWidget, FOnClassSelected, UClass* Class);
	FOnClassSelected OnClassSelected;

	FOnClosed OnClosedDelegate;

};

