// Authors: Egor A. Pristavka, Nikolay Bulatov. 
// Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/UIFormInputWidget.h"
#include "UI/UIPlaceablePropertiesWidget.h"
#include "UIEditorMediaUrlPropertiesWidget.generated.h"

/**
 * 
 */
UCLASS(HideDropdown)
class VEGAME_API UUIEditorMediaUrlPropertiesWidget final : public UUIPlaceablePropertiesWidget {
	GENERATED_BODY()

	virtual void NativeOnInitialized() override;
public:
	void OnUrlTextCommittedCallback(const FText& InText, ETextCommit::Type InCommitMethod);

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UUIFormInputWidget* UrlInputWidget;

	DECLARE_EVENT_OneParam(UUIEditorMediaUrlPropertiesWidget, FOnUrlChanged, const FString& Url);

	FOnUrlChanged OnUrlChanged;

	DECLARE_EVENT(UUIEditorMediaUrlPropertiesWidget, FOnClosed);

	FOnClosed OnClosedDelegate;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString Url;

	void SetUrl(const FString& InUrl);
	FString GetUrl();
};
