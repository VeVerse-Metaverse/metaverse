// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.
#pragma once

#include "Api2Request.h"
#include "Api2SharedAccessMetadata.h"
#include "UIPageContentWidget.h"
#include "VeWorldMetadata.h"
#include "ApiRequest.h"
#include "UISpaceFormPageWidget.generated.h"

class UUIFormInputWidget;
class UUIInputWidget;
class UUIButtonWidget;
class UUILoadingWidget;


/**
 * 
 */
UCLASS(HideDropdown)
class VEUI_API UUISpaceFormPageWidget final : public UUIPageContentWidget {
	GENERATED_BODY()

public:
	UUISpaceFormPageWidget(const FObjectInitializer& ObjectInitializer);


#pragma region Widgets

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UUIImageWidget* PreviewImageWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UUIFormInputWidget* UrlInputWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UCheckBox* PublicCheckBoxWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UUIInputWidget* NameInputWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UUIMultilineInputWidget* DescriptionInputWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UComboBoxString* MapComboBoxWidget;

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
	UUIButtonWidget* FileBrowserButtonWidget;

#pragma endregion Buttons

	UPROPERTY(EditAnywhere, Category="Page|SubPages", meta=(DisplayName="Detail Page"))
	TSubclassOf<UUIPageContentWidget> DetailPageClass;

	void SetMetadata(const FVeWorldMetadata& InMetadata);
	void UpdatePublicMetadata();

protected:
	virtual void NativeOnInitialized() override;
	void OnUrlTextCommittedCallback(const FText& InText, ETextCommit::Type InCommitMethod);
	virtual void NativeConstruct() override;
	virtual void NativeOnRefresh() override;

	FVeWorldMetadata Metadata;
	FApi2SharedAccessMetadata SharedAccessMetadata;

	TArray<FString> DefaultMaps;

	FVeWorldMetadata GetNewMetadata() const;
	bool IsChanged() const;

	void SubmitButton_OnClicked();
	void FileBrowserButton_OnClicked();
	void UploadFiles(TSharedRef<FOnGenericRequestCompleted2> InCallback);
	void UploadFiles_Preview(TSharedRef<FOnGenericRequestCompleted2> InCallback);
	void OnUpdateRequestCompleted(const FApiSpaceMetadata& InMetadata, const bool bSuccessful, const FString& Error);

	void CancelButton_OnClicked();
	void OnCancelDialogButtonClicked(class UUIDialogWidget*, const uint8& InButtonIndex);

	void SetIsProcessing(const bool InProcessing);

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UUISharedAccessWidget* SharedAccessWidget;

	UPROPERTY(EditAnywhere, Category="Page|SubPages", meta=(DisplayName="Shared Access List"))
	TSubclassOf<class UUISharedAccessListPageWidget> SharedAccessListPageClass;
	
private:
	bool bIsProcessing;
	FString PreviewFilePath;

};
