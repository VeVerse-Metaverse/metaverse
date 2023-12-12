// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.
#pragma once

#include "UIWidget.h"
#include "Components/UICommonPaginationWidget.h"
#include "UIUserBrowserWidget.generated.h"

class UUIUserListWidget;
//class UUIUserFilterWidget;
class UUIUserDetailWidget;


/**
 * 
 */
UCLASS()
class VEUI_API UUIUserBrowserWidget final : public UUIWidget {
	GENERATED_BODY()

	void NativeConstruct();
	virtual void RegisterCallbacks() override;
	virtual void UnregisterCallbacks() override;
	
	FDelegateHandle OnUserDetailWidgetClosedDelegateHandle;
	FDelegateHandle OnUserSelectedDelegateHandle;
	FDelegateHandle OnCloseButtonClickedDelegateHandle;


public:
	//virtual void NativeConstruct() override;

	virtual void OnNavigate() override;

	/** List of user. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIUserListWidget* UserListWidget;

	// /** User list filters. */
	// UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	// UUIUserFilterWidget* UserFilterWidget;
	// 
	// UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	// UUIPaginationWidget* PaginationWidget;
	
	/** User details. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIUserDetailWidget* UserDetailWidget;
	
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIButtonWidget* CloseButtonWidget;
	
	//void GetUsers(const IApiBatchQueryRequestMetadata& RequestMetadata = IApiBatchQueryRequestMetadata()) const;

	DECLARE_EVENT(UUIFileBrowserWidget, FOnClosed);
	FOnClosed OnClosedDelegate;

#pragma region Navigation
private:
	bool bUserListWidgetVisible;
	bool bUserDetailWidgetVisible;
	
	/** Fades in the reset password widget. */
	UFUNCTION(BlueprintCallable, Category="Widget|Content")
	void ShowUserListWidget(bool bAnimate = true, bool bPlaySound = true);
	
	/** Fades in the reset password widget. */
	UFUNCTION(BlueprintCallable, Category="Widget|Content")
	void ShowUserDetailWidget(bool bAnimate = true, bool bPlaySound = true);
	
	/** Fades out the login widget. */
	UFUNCTION(BlueprintCallable, Category="Widget|Content")
	void HideUserListWidget(bool bAnimate = true, bool bPlaySound = true);
	
	/** Fades out the login widget. */
	UFUNCTION(BlueprintCallable, Category="Widget|Content")
	void HideUserDetailWidget(bool bAnimate = true, bool bPlaySound = true);
	
	UFUNCTION(BlueprintCallable, Category="Widget|Content")
	void NavigateToUserList(bool bAnimate = true, bool bPlaySound = true);
	
	UFUNCTION(BlueprintCallable, Category="Widget|Content")
	void NavigateToUserDetail(bool bAnimate = true, bool bPlaySound = true);
#pragma endregion
};
