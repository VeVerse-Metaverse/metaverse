// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.
#pragma once

#include "ApiSpaceMetadata.h"
#include "ApiSpaceMetadataObject.h"
#include "Components/UIButtonWidget.h"
#include "Components/UIInputWidget.h"
#include "Components/UIMultilineInputWidget.h"
#include "UIWidget.h"
#include "Components/CheckBox.h"
#include "Components/ComboBoxString.h"
#include "Components/Image.h"
#include "Components/MultiLineEditableTextBox.h"
#include "UISpaceFormWidget.generated.h"

class UUIDialogWidget;
/**
 * 
 */
UCLASS()
class VEUI_API UUISpaceFormWidget final : public UUIWidget {
	GENERATED_BODY()

	virtual void RegisterCallbacks() override;
	virtual void UnregisterCallbacks() override;

	FDelegateHandle OnSpaceCreatedDelegateHandle;
	FDelegateHandle OnSpaceUpdatedDelegateHandle;
	FDelegateHandle OnCloseButtonClickedDelegateHandle;
	FDelegateHandle OnCancelButtonClickedDelegateHandle;
	FDelegateHandle OnSubmitButtonClickedDelegateHandle;

protected:
	virtual void NativeConstruct() override;

public:
	void SetMetadata(TSharedPtr<FApiSpaceMetadata> InSpace);
	//void CreateMetadata(TSharedPtr<FApiSpaceMetadata> OutSpace);

	void SaveMetadata();
	void OnSpaceCreated(const FApiSpaceMetadata& Metadata, const bool bSuccessful, const FString& Error);
	void OnSpaceUpdated(const FApiSpaceMetadata& Metadata, const bool bSuccessful, const FString& Error);
	void OnCloseButtonClicked();
	void OnCancelButtonClicked();
	void OnNameTextCommitted(const FText& InText, ETextCommit::Type InTextCommitType);
	void OnDialogWidget(UUIDialogWidget*, const uint8& InButtonIndex);

	UFUNCTION()
	void OnDescriptionTextCommitted(const FText& InText, ETextCommit::Type InTextCommitType);

	UFUNCTION()
	void OnCheckBoxCheckStateChanged(bool bIsChecked);

	TSharedPtr<FApiSpaceMetadata> Metadata;
	
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UUIButtonWidget* CloseButtonWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UUIButtonWidget* SubmitButtonWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UUIButtonWidget* CancelButtonWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UCheckBox* PublicCheckBoxWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIInputWidget* CreateNameWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIMultilineInputWidget* CreateDescriptionWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UComboBoxString* MapWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UTextBlock* MarkdownTextWidget;

	DECLARE_EVENT_OneParam(UUISpaceFormWidget, FOnClosed,TSharedPtr<FApiSpaceMetadata> Metadata);
	FOnClosed OnClosed;

	bool bIsDirty;
	bool bIsRequestInProgress;
};
