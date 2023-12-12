// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.

#pragma once

#include "ApiUserMetadata.h"
#include "Components/UIButtonWidget.h"
#include "ManagedTextureRequester.h"
#include "UIWidgetBase.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "Components/Border.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "UISocialFriendInviteWidget.generated.h"

UCLASS()
class VEUI_API UUISocialFriendInviteWidget : public UUIWidgetBase
,  public IUserObjectListEntry, public IManagedTextureRequester {
	GENERATED_BODY()

	FDelegateHandle OnAcceptButtonClickedDelegateHandle;
	// FDelegateHandle OnIgnoreButtonClickedDelegateHandle;
	// FDelegateHandle OnBlockButtonClickedDelegateHandle;
	
public:
	
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UImage* AvatarImageWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UBorder* AvatarBorderWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UTextBlock* NameTextWidget;
	
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UTextBlock* StatusTextWidget;
	
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UTextBlock* LevelTextWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UTextBlock* RankTextWidget;
	
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UTextBlock* MutualFriendNumberTextWidget;
	
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIButtonWidget* AcceptButtonWidget;
	
	// UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	// UUIButtonWidget* IgnoreButtonWidget;
	// 	
	// UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	// UUIButtonWidget* BlockButtonWidget;
	
	// UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	// UBorder* BorderAvatar;
	
	UPROPERTY(EditAnywhere)
	UTexture2D* DefaultAvatar;

protected:

	virtual void RegisterCallbacks() override;
	virtual void UnregisterCallbacks() override;
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
	virtual void NativeOnTextureLoaded(const FTextureLoadResult& InResult) override;

	FGuid UserId;
	
	void SetStatus(EApiPresenceStatus Status);
	void SetIsLoading(bool bInIsLoading);
	void SetTexture(UTexture2D* Texture);
	void OnAcceptButtonClicked();
	void OnFollowRequestCompleted(bool bInSuccessful, const FString& InError);
	// void OnIgnoreButtonClicked();
	// void OnBlockButtonClicked();
	
	
};
