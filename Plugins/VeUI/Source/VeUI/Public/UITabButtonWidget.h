// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.

#pragma once

#include "Components/UIButtonWidget.h"
#include "Components/HorizontalBox.h"
#include "Components/Image.h"

#include "UITabButtonWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTabSelectedEvent, int, Index, FString, TabName);

UCLASS()
class VEUI_API UUITabButtonWidget final : public UUIWidgetBase
 {
		GENERATED_BODY()
	
	virtual void NativePreConstruct() override;
	
	virtual void RegisterCallbacks() override;
	
public:
	void SetTabName(const FText& Text);

	FText GetTabName() const { return TabName; }

	void SetTabActive (const bool Active);
	
	bool GetTabActive() const { return bActive; }

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Widget|Content", meta=(BindWidget))
	UUIButtonWidget* Button = nullptr;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Widget|Content", meta=(BindWidget))
	UImage* UnderlineImage = nullptr;
	
	FDelegateHandle OnTabSelectedDelegateHandle;

	UPROPERTY(BlueprintReadWrite, Category="Widget|Content", meta=(InstanceEditable))
	int TabIndex;

	/** Called when the button is clicked */
	UPROPERTY(BlueprintAssignable, Category="TabButton|Event")
	FOnTabSelectedEvent OnTabSelectedEvent;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Widget|Content")
	FLinearColor ActiveTabColor = FLinearColor(0.0f, 0.2f, 1.0f, 1.0f);
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Widget|Content")
	FLinearColor DefaultTabColor = FLinearColor(0.3f, 0.3f, 0.3f, 1.0f);

protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Widget|Content")
	FText TabName;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Widget|Content")
	bool bActive = false;

	UFUNCTION()
	void OnButtonClickedCallback();

	UFUNCTION()
	FLinearColor GetColor();
};
