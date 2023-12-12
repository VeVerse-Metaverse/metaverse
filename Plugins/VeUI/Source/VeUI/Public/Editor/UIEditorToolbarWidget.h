#pragma once

#include "UIWidgetBase.h"
#include "TransformationEnums.h"
#include "UIEditorToolbarWidget.generated.h"

UCLASS()
class VEUI_API UUIEditorToolbarWidget final : public UUIWidgetBase {
	GENERATED_BODY()

public:
	/** Sets transform space and updates widgets. */
	void SetTransformSpace(ETransformSpace TransformSpace) const;

	/** Sets transform tool and updates widgets. */
	void SetTransformTool(ETransformType Tool) const;

	/** Sets transform snapping and updates widgets. */
	void SetTransformSnapping(ETransformType Tool, bool bEnabled, float Value) const;

	/** Callback for the translation snapping value widget change events. */
	UFUNCTION()
	void OnTranslationSnappingValueChanged(float InValue);

	/** Callback for the rotation snapping value widget change events. */
	UFUNCTION()
	void OnRotationSnappingValueChanged(float InValue);

	/** Callback for the scale snapping value widget change events. */
	UFUNCTION()
	void OnScaleSnappingValueChanged(float InValue);

	/** Switches transformations to the world space. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UUIButtonWidget* WorldSpaceButtonWidget;

	/** Switches transformations to the local space. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UUIButtonWidget* LocalSpaceButtonWidget;

	/** Label under the buttons, changes text to correspond the active button. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UTextBlock* TransformSpaceLabelWidget;

	/** Switches transformations to the translation mode. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UUIButtonWidget* TranslateToolButtonWidget;

	/** Switches transformations to the rotation mode. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UUIButtonWidget* RotationToolButtonWidget;

	/** Switches transformations to the scale mode. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UUIButtonWidget* ScaleToolButtonWidget;

	/** Toggles the translation snapping. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UUIButtonWidget* ToggleTranslationSnappingButtonWidget;

	/** Toggles the rotation snapping. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UUIButtonWidget* ToggleRotationSnappingButtonWidget;

	/** Toggles the scale snapping. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UUIButtonWidget* ToggleScaleSnappingButtonWidget;

	/** Toggles the scale snapping. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UUIButtonWidget* CloneButtonWidget;

	/** Reflects and applies the value for the translation snapping. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class USpinBox* TranslationSnappingValueWidget;

	/** Reflects and applies the value for the rotation snapping. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class USpinBox* RotationSnappingValueWidget;

	/** Reflects and applies the value for the translation snapping. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class USpinBox* ScaleSnappingValueWidget;

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	void BindToEditorComponent(class UEditorComponent* NewEditorComponent, class UEditorComponent* OldEditorComponent);

private:
	TWeakObjectPtr<class UEditorComponent> EditorComponent;
};
