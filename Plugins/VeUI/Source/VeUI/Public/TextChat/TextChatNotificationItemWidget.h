// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.
#pragma once


#include "UINotifyItemWidget.h"
#include "TextChatNotificationItemWidget.generated.h"

/**
 * 
 */
UCLASS(HideDropdown)
class VEUI_API UTextChatNotificationItemWidget : public UUINotifyItemWidget {
	GENERATED_BODY()

public:
#pragma region Widgets
	
	UPROPERTY(BlueprintReadOnly, Category="Widget", meta=(BindWidget))
	class UTextBlock* MessageTextWidget;

#pragma endregion Widgets
	
	UPROPERTY(EditAnywhere, Category="Notification")
	int32 MessageTextLimit = 150;

	void SetMessage(const TSharedPtr<class FRpcWebSocketsTextChatMessage>& Message);
	
};
