// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once

#include "VePlaceableMetadata.h"
#include "IPlaceable.generated.h"


UENUM(BlueprintType)
enum class EPlaceableState : uint8 {
	None,
	Downloading,
	Uploading
};

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnPlaceableStateChanged, EPlaceableState State, float Ratio);


class UUIWidgetBase;
UINTERFACE(MinimalAPI, BlueprintType, Blueprintable)
class UPlaceable : public UInterface {
public:
	GENERATED_BODY()
};

class VEGAMEFRAMEWORK_API IPlaceable {
	GENERATED_BODY()

public:
	/** Override to implement different property customization widgets. */
	UFUNCTION(BlueprintNativeEvent, Category="Placeable")
	class UUIPlaceablePropertiesWidget* GetPlaceablePropertiesWidget(APlayerController* PlayerController);
	virtual UUIPlaceablePropertiesWidget* GetPlaceablePropertiesWidget_Implementation(APlayerController* PlayerController) { return nullptr; }

	EPlaceableState GetPlaceableState() const { return PlaceableState; }
	float GetPlaceableRatio() const { return PlaceableRatio; }
	FOnPlaceableStateChanged& GetOnPlaceableStateChanged() { return OnPlaceableStateChanged; };

protected:
	void SetPlaceableStateHelper(EPlaceableState State, float Ratio = 0.0f);

private:
	FOnPlaceableStateChanged OnPlaceableStateChanged;
	EPlaceableState PlaceableState = EPlaceableState::None;
	float PlaceableRatio = 0.0f;

};
