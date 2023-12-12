// Authors: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022-2023 VeVerse AS. All rights reserved.
// Copyright (c) 2023 LE7EL AS. All rights reserved.


#pragma once

#include "Components/UIFormInputWidget.h"
#include "UI/UIPlaceablePropertiesWidget.h"
#include "Components/ComboBoxString.h"
#include "UIEditorCogAiPropertiesWidget.generated.h"

class UUIButtonWidget;

/**
 * 
 */
UCLASS(HideDropdown)
class VEAI_API UUIEditorCogAiPropertiesWidget : public UUIPlaceablePropertiesWidget {
	GENERATED_BODY()

	UFUNCTION()
	void NativeOnChangedVoice(FString SelectedItem, ESelectInfo::Type SelectionType);

	virtual void NativeOnInitialized() override;

public:
	void SetProperties(const FString& InUrlAvatar, const FString& InOpenAIKey, const FString& InAiName, const FString& InAiDescription, const FString& InPersonalityTraits, const FString& InAiVoice);

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIFormInputWidget* UrlAvatarWidget = nullptr;
	
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIFormInputWidget* OpenAIKeyWidget = nullptr;
	
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIFormInputWidget* AiNameWidget = nullptr;
		
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIFormInputWidget* AiDescriptionWidget = nullptr;
			
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIFormInputWidget* PersonalityTraitsWidget = nullptr;
				
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UComboBoxString* AiVoiceWidget = nullptr;
	
	typedef TDelegate<void(const FString& Url)> FOnUrlChanged;
	FOnUrlChanged OnUrlChanged;

	typedef TDelegate<void()> FOnClosed;
	FOnClosed OnClosed;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString Url;

	FString GetUrl();
	FString GetOpenAiKey();
	FString GetAiName();
	FString GetAiDescription();
	FString GetPersonalityTraits();
	FString GetAiVoice();
};
