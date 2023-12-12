// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.
#pragma once

#include "ApiUserMetadata.h"
#include "UIWidgetBase.h"

#include "UISocialPanelWidget.generated.h"

class UUIButtonWidget;
class UUISocialUserCardWidget;
class UUISocialFriendListWidget;
class UUISocialInvitationListWidget;

/**
 * 
 */
UCLASS()
class VEUI_API UUISocialPanelWidget final : public UUIWidgetBase
 {
	GENERATED_BODY()

	FDelegateHandle OnFriendsButtonClickedDelegateHandle;
	FDelegateHandle OnInvitationButtonClickedDelegateHandle;
	FDelegateHandle OnGetAuthUserDelegateHandle;
	FDelegateHandle OnGetAuthUserCompletedDelegateHandle;
	FDelegateHandle OnGetFriendListDelegateHandle;
	FDelegateHandle OnGetFriendListCompletedDelegateHandle;
	FDelegateHandle OnGetInvitationListDelegateHandle;
	FDelegateHandle OnGetInvitationListCompletedDelegateHandle;

public:

	void Refresh();
	
	/** Expands the panel. */
	UUMGSequencePlayer* Expand(bool bAnimate = true, bool bPlaySound = true);

	/** Collapses the panel. */
	UUMGSequencePlayer* Collapse(bool bAnimate = true, bool bPlaySound = true);

	bool bIsCollapsed = false;
	void SetIsCollapsed(bool bInIsCollapsed);
	bool GetIsCollapsed() const;

	/** Root widget expand animation. */
	UPROPERTY(BlueprintReadOnly, Transient, Category="Widget|Animation", meta=(BindWidgetAnimOptional))
	UWidgetAnimation* ExpandAnimation;

	/** Root widget collapse animation (optional). Reversed fade in animation will be used if not defined. */
	UPROPERTY(BlueprintReadOnly, Transient, Category="Widget|Animation", meta=(BindWidgetAnimOptional))
	UWidgetAnimation* CollapseAnimation;

	/** Sound to play on expand. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Widget|Sound")
	USoundBase* ExpandSound;

	/** Sound to play on collapse. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Widget|Sound")
	USoundBase* CollapseSound;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUISocialUserCardWidget* UserCardWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIButtonWidget* FriendsButtonWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIButtonWidget* InvitationsButtonWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUISocialFriendListWidget* SocialFriendListWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUISocialInvitationListWidget* SocialInvitationListWidget;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeOnAddedToFocusPath(const FFocusEvent& InFocusEvent) override;
	virtual void NativeOnRemovedFromFocusPath(const FFocusEvent& InFocusEvent) override;
	virtual void RegisterCallbacks() override;
	virtual void UnregisterCallbacks() override;
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;

	void OnFriendsButtonClicked();
	void OnInvitationsButtonClicked();

#pragma region Navigation
private:
	
	bool bInvitationWidgetVisible;
	bool bFriendListWidgetVisible;
	
	UFUNCTION(BlueprintCallable, Category="Widget|Content")
	void ShowInvitationWidget(bool bAnimate = true, bool bPlaySound = true);
		
	UFUNCTION(BlueprintCallable, Category="Widget|Content")
	void ShowFriendListWidget(bool bAnimate = true, bool bPlaySound = true);
	
	UFUNCTION(BlueprintCallable, Category="Widget|Content")
	void HideInvitationWidget(bool bAnimate = true, bool bPlaySound = true);

	UFUNCTION(BlueprintCallable, Category="Widget|Content")
	void HideFriendListWidget(bool bAnimate = true, bool bPlaySound = true);

	UFUNCTION(BlueprintCallable, Category="Widget|Content")
	void NavigateToInvitationList(bool bAnimate = true, bool bPlaySound = true);

	UFUNCTION(BlueprintCallable, Category="Widget|Content")
	void NavigateToFriendList(bool bAnimate = true, bool bPlaySound = true);

#pragma endregion Navigation

};