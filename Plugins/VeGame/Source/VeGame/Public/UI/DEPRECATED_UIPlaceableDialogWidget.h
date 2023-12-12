// Authors: Egor A. Pristavka, Nikolay Bulatov. 
// Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once

#include "UIWidgetBase.h"
#include "VePropertyMetadata.h"
#include "DEPRECATED_UIPlaceableDialogWidget.generated.h"

UCLASS(HideDropdown, Deprecated)
class VEGAME_API UDEPRECATED_UIPlaceableDialogWidget : public UUIWidgetBase {
	GENERATED_BODY()

	DECLARE_EVENT_OneParam(UTextChatBookmarkItemWidget, FOnPlaceableDialogPropertiesEvent, const TArray<FVePropertyMetadata>& InProperties);

public:
#pragma region Widgets

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UBorder* ContainerWidget;
	
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UUIButtonWidget* ConfirmButtonWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UUIButtonWidget* CancelButtonWidget;

#pragma endregion Widgets

	FOnPlaceableDialogPropertiesEvent OnPropertiesChanged;

	void SetProperties(TSubclassOf<class UDEPRECATED_UIPlaceablePropertiesWidgetOLD> PropertiesWidgetClass, const TArray<FVePropertyMetadata>& InProperties);

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeOnClose();

	TWeakObjectPtr<class UDEPRECATED_UIPlaceablePropertiesWidgetOLD> PlaceablePropertiesWidget; 
};