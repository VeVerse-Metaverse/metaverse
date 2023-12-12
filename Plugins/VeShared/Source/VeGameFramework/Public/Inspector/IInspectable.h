// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once

#include "IInspectable.generated.h"

class UUIWidgetBase;

UINTERFACE(MinimalAPI, BlueprintType, Blueprintable)
class UInspectable : public UInterface {
public:
	GENERATED_BODY()
};

class VEGAMEFRAMEWORK_API IInspectable {
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, Category="Inspectable")
	FVector GetInspectableForwardVector();
	
	virtual FVector GetInspectableForwardVector_Implementation() { return FVector::ZeroVector; }
	
	/** Override to implement different inspector preview widgets. */
	UFUNCTION(BlueprintNativeEvent, Category="Inspectable")
	UUserWidget* GetInspectPreviewWidget(APlayerController* PlayerController);

	virtual UUserWidget* GetInspectPreviewWidget_Implementation(APlayerController* PlayerController) { return nullptr; }

	/** Override to implement different inspector property widgets. */
	UFUNCTION(BlueprintNativeEvent, Category="Inspectable")
	UUserWidget* GetInspectPropertiesWidget(APlayerController* PlayerController);

	/** Override to implement different property customization widgets. */
	virtual UUserWidget* GetInspectPropertiesWidget_Implementation(APlayerController* PlayerController) { return nullptr; }

};
