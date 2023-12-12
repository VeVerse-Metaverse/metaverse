// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.
#pragma once

#include "UIPageContentWidget.h"
#include "ApiPersonaMetadata.h"
#include "ApiUserMetadata.h"
#include "ManagedTextureRequester.h"
#include "Components/UIInputWidget.h"
#include "Components/UIFormInputWidget.h"
#include "UIWidgetBase.h"

#include "UIAvatarFormPageWidget.generated.h"

/**
 * 
 */
UCLASS(HideDropdown)
class VEUI_API UUIAvatarFormPageWidget final : public UUIPageContentWidget {
	GENERATED_BODY()

public:
	UUIAvatarFormPageWidget(const FObjectInitializer& ObjectInitializer);

#pragma region Widgets

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIInputWidget* NameInputWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIFormInputWidget* UrlInputWidget;

#pragma endregion Widgets

#pragma region Buttons

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UUIButtonWidget* CancelButtonWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UUIButtonWidget* SubmitButtonWidget;

#pragma endregion Buttons

	void SetMetadata(const FApiPersonaMetadata& InMetadata);

protected:
	virtual void NativeConstruct() override;
	virtual void RegisterCallbacks() override;
	virtual void UnregisterCallbacks() override;

	FApiPersonaMetadata Metadata;

	void CreatePersonaRequest();
	void OnCreatePersonaResponse(const FApiPersonaMetadata& InMetadata,bool bInSuccessful, const FString& InError);

	void SetFilePersonaRequest(const FApiPersonaMetadata& InPersonaMetadata);
	void OnSetFilePersonaResponse(const FApiPersonaMetadata& InPersonaMetadata, const FApiFileMetadata& InFileMetadata, const bool bInSuccessful, const FString& InError) const;

	void OnUrlTextCommittedCallback(const FText& InText, ETextCommit::Type InCommitMethod);

private:
	FDelegateHandle OnCancelButtonClickedDelegateHandle;
	FDelegateHandle OnSubmitButtonClickedDelegateHandle;
	FDelegateHandle OnUrlPersonInputDelegateHandle;
};

