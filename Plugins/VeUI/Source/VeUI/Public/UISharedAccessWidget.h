// Author: Khusan T.Yadgarov
// Copyright (c) 2022 LE7EL AS. All right reserved.
#pragma once

#include "Api2SharedAccessMetadata.h"
#include "UIPageContentWidget.h"
#include "VeWorldMetadata.h"
#include "VeUserMetadata.h"
#include "Components/UIButtonWidget.h"
#include "UISharedAccessWidget.generated.h"


/**
 * 
 */
UCLASS(HideDropdown)
class VEUI_API UUISharedAccessWidget final : public UUIWidgetBase {
	GENERATED_BODY()

	DECLARE_EVENT(UUISharedAccessWidget, UISharedAccessButtonClickedEvent);
	
public:
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UUIComboBoxUser* UserComboBoxWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UUIInputWidget* UserInputWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UCheckBox* PublicCheckBoxWidget;
	
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UCheckBox* CanViewCheckBoxWidget;
	
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UCheckBox* CanEditCheckBoxWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UCheckBox* CanDeleteCheckBoxWidget;
	
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIButtonWidget* SaveButtonWidget;
	
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIButtonWidget* UserListButtonWidget;

	void SetMetadata(const FVeWorldMetadata& InWorldMetadata);

	// UISharedAccessButtonClickedEvent OnSaveButtonClicked;
	UISharedAccessButtonClickedEvent OnUserListButtonClicked;
	
protected:
	virtual void NativeOnInitialized() override;


	void UserComboBox_OnSelectionChanged(FVeUserMetadata SelectedItem, ESelectInfo::Type SelectionType);
	void ShowUsers(const FGuid& UserId, const FString& Query, const FGuid SelectedUserId = FGuid());


	/** Request metadata for UserList */
	FVeWorldMetadata WorldMetadata;
	FVeUserMetadata UserMetadata;
	FApi2SharedAccessMetadata SharedAccessMetadata;

	
	void UpdateSharedAccessMetadataRequest();
	void OnAccessMetadataResponse(const FApi2SharedAccessMetadata& InMetadata, bool bSuccessful, const FString& Error);

	//void SetIsProcessing(bool bInIsLoading) const;

	FString UserFilter;
	FGuid DefaultUserId;


private:
	FDelegateHandle OnContainerItemClickedDelegateHandle;
	FDelegateHandle OnPaginationPageChangeDelegateHandle;
	FDelegateHandle OnFilterChangeDelegateHandle;
	FDelegateHandle OnRefreshButtonClickedDelegateHandle;

};
