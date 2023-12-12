// Authors: Egor A. Pristavka, Nikolay Bulatov. 
// Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ManagedTextureRequester.h"
#include "Components/UIFormInputWidget.h"
#include "UI/UIPlaceablePropertiesWidget.h"
#include "UIEditorGltfPropertiesWidget.generated.h"

class UImage;
class UUIButtonWidget;

/**
 * 
 */
UCLASS(HideDropdown)
class VEGAME_API UUIEditorGltfPropertiesWidget : public UUIPlaceablePropertiesWidget, public IManagedTextureRequester {
	GENERATED_BODY()

	virtual void NativeOnTextureLoaded(const FTextureLoadResult& InResult) override;

	virtual void NativeOnInitialized() override;

public:
	void OnUrlTextCommittedCallback(const FText& InText, ETextCommit::Type InCommitMethod);
	void OnFileBrowserButtonClicked();

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIFormInputWidget* UrlInputWidget = nullptr;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UUIButtonWidget* FileBrowserButtonWidget = nullptr;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UImage* PreviewImageWidget = nullptr;

	typedef TDelegate<void(const FString& Url)> FOnUrlChanged;
	FOnUrlChanged OnUrlChanged;

	typedef TDelegate<void()> FOnClosed;
	FOnClosed OnClosed;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString Url;

	void SetUrl(const FString& InUrl);
	FString GetUrl();
};
