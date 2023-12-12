// Authors: Egor A. Pristavka, Nikolay Bulatov. 
// Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once

#include "UIWidgetBase.h"
#include "VePropertyMetadata.h"
#include "DEPRECATED_UIPlaceablePropertiesWidgetOLD.generated.h"

/**
 * 
 */
UCLASS(HideDropdown, Deprecated)
class VEGAME_API UDEPRECATED_UIPlaceablePropertiesWidgetOLD : public UUIWidgetBase {
	GENERATED_BODY()

public:
#pragma region Widgets

	// void SetPlaceableObject(class APlaceableObject* InPlaceableObject); 

#pragma endregion Widgets

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	TArray<FVePropertyMetadata> GetProperties() const;
	virtual TArray<FVePropertyMetadata> GetProperties_Implementation() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void SetProperties(const TArray<FVePropertyMetadata>& Properties);
	virtual void SetProperties_Implementation(const TArray<FVePropertyMetadata>& Properties);
	
protected:
	// TWeakObjectPtr<APlaceableObject> PlaceableObject;
	
};
