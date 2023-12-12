// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.
#pragma once

#include "Blueprint/UserWidget.h"
#include "TextChatInputWidget.generated.h"


/**
 * 
 */
UCLASS()
class VEUI_API UTextChatInputWidget : public UUserWidget {
	GENERATED_BODY()

	DECLARE_EVENT_OneParam(UTextChatInputWidget, FOnTextChatMessageSended, const FText& Message);
	DECLARE_EVENT(UTextChatInputWidget, FOnTextChatInputEvent);

#pragma region Widgets

public:
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UEditableTextBox* TextBoxWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UButton* SendButtonWidget;
	
#pragma endregion Widgets

public:
	UTextChatInputWidget(const FObjectInitializer& ObjectInitializer);
	
	void Clear();
	UWidget* GetWidgetToFocus() const;
	
	FOnTextChatMessageSended& GetOnMessageSent() { return OnMessageSent; }
	
protected:
	virtual void NativeOnInitialized() override;

	virtual void NativeOnSendMessage();
	
private:
	FOnTextChatMessageSended OnMessageSent;

	UFUNCTION()
	void TextBoxWidget_OnTextCommittedCallback(const FText& Text, ETextCommit::Type CommitMethod);

	UFUNCTION()
	void TextBoxWidget_OnTextChangedCallback(const FText& Text);
	
	UFUNCTION()
	void SendButtonWidget_OnClickedCallback();
	
};
