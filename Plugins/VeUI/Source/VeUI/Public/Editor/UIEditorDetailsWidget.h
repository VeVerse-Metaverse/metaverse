// Copyright (c) 2022 LE7EL AS. All right reserved.

#pragma once

#include "UIWidgetBase.h"
#include "IPlaceable.h"
#include "UIEditorDetailsWidget.generated.h"

/**
 * 
 */
UCLASS(HideDropdown)
class VEUI_API UUIEditorDetailsWidget : public UUIWidgetBase {
	GENERATED_BODY()

public:
#pragma region Widgets

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UUIEditorTransformWidget* TransformWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UTextBlock* NameTextWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UEditableText* IdTextWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UBorder* ContainerWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UUIButtonWidget* SaveButtonWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UUIButtonWidget* ResetButtonWidget;

#pragma endregion Widgets

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	void BindToEditorComponent(class UEditorComponent* NewEditorComponent, class UEditorComponent* OldEditorComponent);

	TWeakObjectPtr<AActor> SelectedActor;
	TWeakObjectPtr<class UPlaceableComponent> PlaceableComponent;
	TWeakObjectPtr<class UUIPlaceablePropertiesWidget> PropertiesWidget;

	void Clear();

	void UpdateButtons();

	void PlaceableInterface_OnPlaceableStateChanged(EPlaceableState State, float Ratio);

private:
	bool bSaveEnableInterface = false;
	bool bSaveEnableWidget = false;
	bool bResetEnableInterface = false;
	bool bResetEnableWidget = false;

};
