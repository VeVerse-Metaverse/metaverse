// Author: Egor A. Pristavka. 
// Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#pragma once

#include "Components/UIFormInputWidget.h"
#include "UI/UIPlaceablePropertiesWidget.h"
#include "UIUrlPropertiesWidget.generated.h"

/**
 * 
 */
UCLASS(HideDropdown)
class VEGAME_API UUIUrlPropertiesWidget : public UUIPlaceablePropertiesWidget {
	GENERATED_BODY()

	
public:
	UPROPERTY(EditAnywhere, Category="Widget|Content", meta=(BindWidget))
	class UUIFormInputWidget* UrlInputWidget;
	
	void SetUrl(const FString& Url);
	
	FString GetUrl() const;
};
