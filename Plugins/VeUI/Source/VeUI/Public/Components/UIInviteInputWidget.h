// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.

#pragma once

#include "Components/UIButtonWidget.h"
#include "Components/UIInputWidget.h"
#include "UIInviteInputWidget.generated.h"

/**
* 
*/
UCLASS()
class VEUI_API UUIInviteInputWidget : public UUIInputWidget {
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized() override;

private:
	UPROPERTY()
	FString PreviousText;

	UFUNCTION()
	FText CheckInputText(const FText& InText);

	UFUNCTION()
	void OnInputTextChangedCallback(const FText& InText);

	UFUNCTION()
	void OnInputTextCommittedCallback(const FText& InText, ETextCommit::Type InCommitMethod);

public:
	FOnInputTextChanged OnInviteTextChanged;
	FOnInputTextChanged OnInviteTextCommitted;
};
