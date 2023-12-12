// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.
#pragma once


#include "UIPageContentWidget.h"
#include "ApiObjectMetadata.h"
#include "ApiRequest.h"
#include "UIObjectFormPageWidget.generated.h"

class UUIInputWidget;
class UUIButtonWidget;
class UUILoadingWidget;


/**
 * 
 */
UCLASS(HideDropdown)
class VEUI_API UUIObjectFormPageWidget final : public UUIPageContentWidget {
	GENERATED_BODY()

public:
	UUIObjectFormPageWidget(const FObjectInitializer& ObjectInitializer);
	
#pragma region Widgets

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UUIImageWidget* PreviewImageWidget;
	
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UCheckBox* PublicCheckBoxWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UUIMultilineInputWidget* ModelInputWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIInputWidget* NameInputWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UUIMultilineInputWidget* DescriptionInputWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIInputWidget* ArtistInputWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIInputWidget* HeightInputWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIInputWidget* WidthInputWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIInputWidget* ScaleMultiplierInputWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIInputWidget* TypeInputWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIInputWidget* DateInputWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIInputWidget* DimensionsInputWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIInputWidget* MediumInputWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIInputWidget* SourceInputWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIInputWidget* SourceUrlInputWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIInputWidget* LicenseInputWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIInputWidget* CopyrightInputWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIInputWidget* CreditInputWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIInputWidget* OriginInputWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIInputWidget* LocationInputWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUILoadingWidget* LoadingWidget;

#pragma endregion Widgets
	
#pragma region Buttons
	
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIButtonWidget* SubmitButtonWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIButtonWidget* CancelButtonWidget;

	/** Open dialog for select file */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIButtonWidget* PreviewBrowserButtonWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIButtonWidget* ModelBrowserButtonWidget;

#pragma endregion Buttons

	UPROPERTY(EditAnywhere, Category="Page|SubPages", meta=(DisplayName="Detail Page"))
	TSubclassOf<UUIPageContentWidget> DetailPageClass;

	void SetMetadata(const FApiObjectMetadata& InMetadata);
	
protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;

	FApiObjectMetadata Metadata;

	/** PreviewFile for upload. */
	FString PreviewFilePath;

	/** ModelFile for upload. */
	FString ModelFilePath;

	FApiObjectMetadata GetNewMetadata() const;
	bool IsChanged() const;

	void SubmitButton_OnClicked();
	void OnUpdateRequestCompleted(const FApiObjectMetadata& InMetadata, const bool bSuccessful, const FString& Error);
	void UploadFiles(TSharedRef<FOnGenericRequestCompleted> InCallback);
	void UploadFiles_Preview(TSharedRef<FOnGenericRequestCompleted> InCallback);
	void UploadFiles_Model(TSharedRef<FOnGenericRequestCompleted> InCallback);
	// void OnFileUploadRequestCompleted(const FApiFileMetadata& InFileMetadata, const bool bSuccessful, const FString& Error, bool IsNewRecord);

	void CancelButton_OnClicked();
	void OnCancelDialogButtonClicked(class UUIDialogWidget*, const uint8& InButtonIndex);

	void SetIsProcessing(const bool InProcessing);

	/** Open file dialog */	
	void PreviewBrowserButton_OnClicked();

	/** Open file dialog */	
	void ModelBrowserButton_OnClicked();

private:
	/** Flag to indicate asynchronous blocking operation in progress and block user input until it is finished. */
	bool bIsProcessing;

};
