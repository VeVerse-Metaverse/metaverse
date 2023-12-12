// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.
#pragma once

#include "ApiObjectMetadata.h"
#include "Components/UIButtonWidget.h"
#include "UIDialogWidget.h"
#include "Components/UIInputWidget.h"
#include "Components/UIMultilineInputWidget.h"
#include "UIFileBrowserWidget.h"
#include "Components/UILoadingWidget.h"
#include "UIWidgetBase.h"
#include "Components/CheckBox.h"
#include "Components/ComboBoxString.h"
#include "UIObjectFormWidget.generated.h"

/**
 * 
 */
UCLASS()
class VEUI_API UUIObjectFormWidget final : public UUIWidgetBase
 {
	GENERATED_BODY()

	virtual void RegisterCallbacks() override;
	virtual void UnregisterCallbacks() override;

	FDelegateHandle OnObjectCreatedDelegateHandle;
	FDelegateHandle OnObjectUpdatedDelegateHandle;
	FDelegateHandle OnFileUploadedDelegateHandle;
	FDelegateHandle OnCloseButtonClickedDelegateHandle;
	FDelegateHandle OnCancelButtonClickedDelegateHandle;
	FDelegateHandle OnSubmitButtonClickedDelegateHandle;
	FDelegateHandle OnUploadButtonClickedDelegateHandle;
	FDelegateHandle OnFileBrowserClosedDelegateHandle;
	FDelegateHandle OnFileBrowserFileSelectedDelegateHandle;

	FUIDialogButtonClicked OnDialogButtonClickedDelegate;

	TArray<FString> AllowedExtensions = TArray<FString>{TEXT("jpg"), TEXT("jpeg")};

	void SetIsProcessing(const bool bInIsLoading) const;

protected:
	virtual void NativeConstruct() override;

public:
	void OnFileBrowserFileSelected(const FPlatformFileInfo& InFileInfo);
	void OnFileUploaded(const FApiFileMetadata& InMetadata, const bool bSuccessful, const FString& Error);
	void OnBrowseButtonClicked();

#pragma region Callbacks
	void OnSubmitButtonClicked();
	void OnObjectCreated(const FApiObjectMetadata& InMetadata, const bool bSuccessful, const FString& Error);
	void OnObjectUpdated(const FApiObjectMetadata& InMetadata, const bool bSuccessful, const FString& Error) const;
	void OnCloseButtonClicked();
	void OnCancelButtonClicked();
	void OnNameTextCommitted(const FText& InText, ETextCommit::Type InTextCommitType);
	void OnArtistTextCommitted(const FText& InText, ETextCommit::Type InTextCommitType);
	void OnDateTextCommitted(const FText& InText, ETextCommit::Type InTextCommitType);
	void OnMediumTextCommitted(const FText& InText, ETextCommit::Type InTextCommitType);
	void OnSourceTextCommitted(const FText& InText, ETextCommit::Type InTextCommitType);
	void OnSourceUrlTextCommitted(const FText& InText, ETextCommit::Type InTextCommitType);
	void OnLicenseTextCommitted(const FText& InText, ETextCommit::Type InTextCommitType);
	void OnCopyrightTextCommitted(const FText& InText, ETextCommit::Type InTextCommitType);
	void OnCreditTextCommitted(const FText& InText, ETextCommit::Type InTextCommitType);
	void OnOriginTextCommitted(const FText& InText, ETextCommit::Type InTextCommitType);
	void OnLocationTextCommitted(const FText& InText, ETextCommit::Type InTextCommitType);
	void OnCloseConfirmationDialogButtonClicked(UUIDialogWidget*, const uint8& InButtonIndex);

	UFUNCTION()
	void OnDescriptionTextCommitted(const FText& InText, ETextCommit::Type InTextCommitType);

	UFUNCTION()
	void OnCheckBoxCheckStateChanged(bool bIsChecked);
#pragma endregion

	void SetMetadata(TSharedPtr<FApiObjectMetadata> InObject);
	TSharedPtr<FApiObjectMetadata> Metadata;

	TSharedPtr<FApiUploadFileMetadata> UploadFileMetadata;

	/** File selected for upload. */
	FPlatformFileInfo SelectedFileInfo;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UUIButtonWidget* CloseButtonWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UUIButtonWidget* SubmitButtonWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UUIButtonWidget* CancelButtonWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UUIButtonWidget* BrowseButtonWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UCheckBox* PublicCheckBoxWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UUIInputWidget* CreateNameWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UUIInputWidget* CreateArtistWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIMultilineInputWidget* CreateDescriptionWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UComboBoxString* TypeWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIInputWidget* DateWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIInputWidget* MediumWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIInputWidget* SourceWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIInputWidget* SourceUrlWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIInputWidget* LicenseWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIInputWidget* CopyrightWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIInputWidget* CreditWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIInputWidget* OriginWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIInputWidget* LocationWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UTextBlock* MarkdownTextWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIFileBrowserWidget* FileBrowserWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUILoadingWidget* LoadingWidget;

	DECLARE_EVENT_OneParam(UUIObjectFormWidget, FOnClosed, TSharedPtr<FApiObjectMetadata> Metadata);
	FOnClosed OnClosedDelegate;

	/** Flag to block immediate form closing if user has changed something. */
	bool bIsDirty;

	/** Flag to indicate asynchronous blocking operation in progress and block user input until it is finished. */
	bool bIsRequestInProgress;

private:
	bool bFileBrowserWidgetVisible;

	UFUNCTION(BlueprintCallable, Category="Widget|Content")
	void HideFileBrowserWidget(bool bAnimate = true, bool bPlaySound = true);

	UFUNCTION(BlueprintCallable, Category="Widget|Content")
	void ShowFileBrowserWidget(bool bAnimate = true, bool bPlaySound = true);
};
