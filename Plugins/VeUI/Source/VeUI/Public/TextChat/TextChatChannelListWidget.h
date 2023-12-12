// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.
#pragma once

#include "Blueprint/UserWidget.h"
#include "TextChatChannelListWidget.generated.h"

/**
 * 
 */
UCLASS()
class VEUI_API UTextChatChannelListWidget : public UUserWidget {
	GENERATED_BODY()

	DECLARE_EVENT_OneParam(UTextChatBookmarkItemWidget, FOnChannelItemWidgetEvent, class UTextChatChannelItemWidget* ItemWidget);
	
public:
#pragma region Widgets

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UScrollBox* ScrollBoxWidget;

#pragma endregion Widgets

#pragma region Buttons
#pragma endregion Buttons

	FOnChannelItemWidgetEvent& GetOnChannelSelectClicked() { return OnChannelSelectClicked; }
	FOnChannelItemWidgetEvent& GetOnChannelDeselectClicked() { return OnChannelDeselectClicked; }
	
protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UTextChatChannelItemWidget> ChannelItemClass;

	UPROPERTY(EditAnywhere)
	bool FilterSelected = false;

	UPROPERTY(EditAnywhere)
	bool AddChannelAsFirst = false;
	
	void UpdateChannelList();
	UTextChatChannelItemWidget* CreateChannelWidget(const TSharedRef<FRpcWebSocketsTextChatChannel>& Channel);
	void RemoveChannelWidget(const TSharedPtr<FRpcWebSocketsTextChatChannel>& Channel);
	void UpdateChannelVisible(const TSharedPtr<FRpcWebSocketsTextChatChannel>& Channel, bool Selected, UTextChatChannelItemWidget* ChannelItemWidget = nullptr);
	UTextChatChannelItemWidget* FindChannelItemWidget(const TSharedPtr<FRpcWebSocketsTextChatChannel>& Channel);

private:
	FOnChannelItemWidgetEvent OnChannelSelectClicked;
	FOnChannelItemWidgetEvent OnChannelDeselectClicked;

	void TextChatSubsystem_OnChannelListAddedCallback(const TSharedPtr<FRpcWebSocketsTextChatChannel>& Channel);
	void TextChatSubsystem_OnChannelSelectChangedCallback(const TSharedPtr<FRpcWebSocketsTextChatChannel>& Channel, bool Selected, bool CanDeselect);
	void ChannelItemWidget_OnSelectClickedCallback(UTextChatChannelItemWidget* ChannelItemWidget);
	void ChannelItemWidget_OnDeselectClickedCallback(UTextChatChannelItemWidget* ChannelItemWidget);

};

