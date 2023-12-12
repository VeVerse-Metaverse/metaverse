// Authors: Egor A. Pristavka, Nikolay Bulatov. 
// Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once

#include "IPlaceable.h"
#include "Blueprint/UserWidget.h"
#include "UIPlaceableStateWidget.generated.h"

/**
 * 
 */
UCLASS(HideDropdown)
class VEGAME_API UUIPlaceableStateWidget : public UUserWidget {
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UTextBlock* TextWidget;

	void SetPlaceableInterface(IPlaceable* PlaceableInterface);

protected:
	void NativeOnInitialized() override;
	
private:
};
