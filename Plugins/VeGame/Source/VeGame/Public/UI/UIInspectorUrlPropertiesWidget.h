// Authors: Egor A. Pristavka, Nikolay Bulatov. 
// Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ManagedTextureRequester.h"
#include "Components/UIFormInputWidget.h"
#include "UIInspectorUrlPropertiesWidget.generated.h"

class UImage;
class UUIButtonWidget;

/**
 * 
 */
UCLASS(HideDropdown)
class VEGAME_API UUIInspectorUrlPropertiesWidget : public UUserWidget, public IManagedTextureRequester {
	GENERATED_BODY()

	virtual void NativeOnInitialized() override;

public:
	
	FString Url;
	
	void SetUrl(const FString& InUrl);

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UTextBlock* NameTextBlock = nullptr;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UTextBlock* UrlTextBlock = nullptr;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIButtonWidget* OpenButtonWidget = nullptr;
};
