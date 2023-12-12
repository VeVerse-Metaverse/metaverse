// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.
#pragma once

#include "UIListWidget.h"
#include "UISocialFilterWidget.h"
#include "Components/UICommonPaginationWidget.h"
#include "UISocialFriendInviteWidget.h"
#include "Components/TileView.h"
#include "UISocialInvitationListWidget.generated.h"

/**
 * 
 */
UCLASS()
class VEUI_API UUISocialInvitationListWidget : public UUIWidgetBase
 {
	GENERATED_BODY()

public:
#pragma region Widgets
	
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUISocialFilterWidget* FilterWidget;
	
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UTileView* ContainerWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUICommonPaginationWidget* PaginationWidget;

#pragma endregion Widgets
	
	UPROPERTY(EditAnywhere, Category="Page|SubPages", meta=(DisplayName="Detail Page"))
	TSubclassOf<class UUIPageContentWidget> DetailPageClass;

	UPROPERTY(EditAnywhere)
	int32 ItemsPerPage = 10;
	
	void SetUserId(const FGuid& InUserGuid);
	void Refresh();

protected:

	virtual void NativeConstruct() override;
	virtual void RegisterCallbacks() override;
	virtual void UnregisterCallbacks() override;

	FGuid UserId;
	IApiFollowersRequestMetadata RequestMetadata;

	void RequestlInvitations(const IApiFollowersRequestMetadata& InRequestMetadata = IApiFollowersRequestMetadata());
	void SetIsProcessing(bool bInIsLoading);
	void OnFriendsLoaded(const FApiUserBatchMetadata& Metadata, bool bSuccessful, const FString& Error);

private:
	FDelegateHandle OnContainerItemClickedDelegateHandle;
	FDelegateHandle OnFilterChangeDelegateHandle;
	FDelegateHandle OnPaginationPageChangeDelegateHandle;

};
