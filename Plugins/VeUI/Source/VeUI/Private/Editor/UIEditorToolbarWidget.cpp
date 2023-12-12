#include "Editor/UIEditorToolbarWidget.h"

#include "GameFrameworkEditorSubsystem.h"
#include "VeGameFramework.h"
#include "VeUI.h"
#include "Components/SpinBox.h"
#include "Components/UIButtonWidget.h"
#include "EditorComponent.h"


void UUIEditorToolbarWidget::NativeOnInitialized() {
	Super::NativeOnInitialized();

#pragma region Transform Space

	// Subscribe for world space button clicks to notify the subsystem.
	if (IsValid(WorldSpaceButtonWidget)) {
		WorldSpaceButtonWidget->GetOnButtonClicked().AddWeakLambda(this, [=] {
			if (EditorComponent.IsValid() && EditorComponent->GetTransformSpace() == ETransformSpace::World) {
				EditorComponent->SetTransformSpace(ETransformSpace::Local);
			}
		});
	}

	// Subscribe for local space button clicks to notify the subsystem.
	if (IsValid(LocalSpaceButtonWidget)) {
		LocalSpaceButtonWidget->GetOnButtonClicked().AddWeakLambda(this, [=] {
			if (EditorComponent.IsValid() && EditorComponent->GetTransformSpace() == ETransformSpace::Local) {
				EditorComponent->SetTransformSpace(ETransformSpace::World);
			}
		});
	}
#pragma endregion

#pragma region Transform Tool

	// Subscribe for translation tool button clicks to notify the subsystem.
	if (IsValid(TranslateToolButtonWidget)) {
		TranslateToolButtonWidget->GetOnButtonClicked().AddWeakLambda(this, [=] {
			if (EditorComponent.IsValid() && EditorComponent->GetTransformTool() != ETransformType::Translation) {
				EditorComponent->SetTransformTool(ETransformType::Translation);
			}
		});
	}

	// Subscribe for rotation tool button clicks to notify the subsystem.
	if (IsValid(RotationToolButtonWidget)) {
		RotationToolButtonWidget->GetOnButtonClicked().AddWeakLambda(this, [=] {
			if (EditorComponent.IsValid() && EditorComponent->GetTransformTool() != ETransformType::Rotation) {
				EditorComponent->SetTransformTool(ETransformType::Rotation);
			}
		});
	}

	// Subscribe for scale tool button clicks to notify the subsystem.
	if (IsValid(ScaleToolButtonWidget)) {
		ScaleToolButtonWidget->GetOnButtonClicked().AddWeakLambda(this, [=] {
			if (EditorComponent.IsValid() && EditorComponent->GetTransformTool() != ETransformType::Scale) {
				EditorComponent->SetTransformTool(ETransformType::Scale);
			}
		});
	}
#pragma endregion

#pragma region Transform Snapping

	// Subscribe for toggle translation snapping button clicks to notify the subsystem.
	if (IsValid(ToggleTranslationSnappingButtonWidget)) {
		ToggleTranslationSnappingButtonWidget->GetOnButtonClicked().AddWeakLambda(this, [=] {
			if (EditorComponent.IsValid()) {
				EditorComponent->SetSnapping(ETransformType::Translation, !EditorComponent->IsTranslationSnappingEnabled());
			}
		});
	}

	// Subscribe for toggle rotation snapping button clicks to notify the subsystem.
	if (IsValid(ToggleRotationSnappingButtonWidget)) {
		ToggleRotationSnappingButtonWidget->GetOnButtonClicked().AddWeakLambda(this, [=] {
			if (EditorComponent.IsValid()) {
				EditorComponent->SetSnapping(ETransformType::Rotation, !EditorComponent->IsRotationSnappingEnabled());
			}
		});
	}

	// Subscribe for toggle scale snapping button clicks to notify the subsystem.
	if (IsValid(ToggleScaleSnappingButtonWidget)) {
		ToggleScaleSnappingButtonWidget->GetOnButtonClicked().AddWeakLambda(this, [=] {
			if (EditorComponent.IsValid()) {
				EditorComponent->SetSnapping(ETransformType::Scale, !EditorComponent->IsScaleSnappingEnabled());
			}
		});
	}

	if (IsValid(CloneButtonWidget)) {
		CloneButtonWidget->GetOnButtonClicked().AddWeakLambda(this, [=] {
			if (EditorComponent.IsValid()) {
				EditorComponent->SetCloning();
			}
		});
	}

	// Subscribe for translation snapping value changes to notify the subsystem.
	if (IsValid(TranslationSnappingValueWidget)) {
		TranslationSnappingValueWidget->OnValueChanged.AddDynamic(this, &UUIEditorToolbarWidget::OnTranslationSnappingValueChanged);
	}

	// Subscribe for rotation snapping value changes to notify the subsystem.
	if (IsValid(RotationSnappingValueWidget)) {
		RotationSnappingValueWidget->OnValueChanged.AddDynamic(this, &UUIEditorToolbarWidget::OnRotationSnappingValueChanged);
	}

	// Subscribe for scale snapping value changes to notify the subsystem.
	if (IsValid(ScaleSnappingValueWidget)) {
		ScaleSnappingValueWidget->OnValueChanged.AddDynamic(this, &UUIEditorToolbarWidget::OnScaleSnappingValueChanged);
	}

#pragma endregion
}

void UUIEditorToolbarWidget::NativeConstruct() {
	Super::NativeConstruct();

	GET_MODULE_SUBSYSTEM(EditorSubsystem, GameFramework, Editor);
	if (EditorSubsystem) {
		EditorSubsystem->GetOnEditorComponentChanged().AddUObject(this, &UUIEditorToolbarWidget::BindToEditorComponent);
		BindToEditorComponent(EditorSubsystem->GetEditorComponent(), nullptr);
	}
}

void UUIEditorToolbarWidget::NativeDestruct() {
	Super::NativeDestruct();

	GET_MODULE_SUBSYSTEM(EditorSubsystem, GameFramework, Editor);
	if (EditorSubsystem) {
		EditorSubsystem->GetOnEditorComponentChanged().RemoveAll(this);
		BindToEditorComponent(nullptr, EditorSubsystem->GetEditorComponent());
	}
}

void UUIEditorToolbarWidget::BindToEditorComponent(UEditorComponent* NewEditorComponent, UEditorComponent* OldEditorComponent) {
	if (OldEditorComponent) {
		OldEditorComponent->GetOnTransformSpaceChanged().RemoveAll(this);
		OldEditorComponent->GetOnTransformToolChanged().RemoveAll(this);
		OldEditorComponent->GetOnTransformSnappingChanged().RemoveAll(this);
	}

	EditorComponent = NewEditorComponent;

	if (EditorComponent.IsValid()) {
		// Set initial transform space.
		SetTransformSpace(EditorComponent->GetTransformSpace());

		// Subscribe for transform space changes.
		EditorComponent->GetOnTransformSpaceChanged().AddWeakLambda(this, [=](const ETransformSpace TransformSpace) {
			SetTransformSpace(TransformSpace);
		});

		// Set initial transform tool.
		SetTransformTool(EditorComponent->GetTransformTool());

		// Subscribe for transform tool changes.
		EditorComponent->GetOnTransformToolChanged().AddWeakLambda(this, [=](const ETransformType Tool) {
			SetTransformTool(Tool);
		});

		// Set initial translation snapping.
		SetTransformSnapping(ETransformType::Translation, EditorComponent->IsTranslationSnappingEnabled(), EditorComponent->GetTranslationSnappingValue());
		// Set initial rotation snapping.
		SetTransformSnapping(ETransformType::Rotation, EditorComponent->IsRotationSnappingEnabled(), EditorComponent->GetRotationSnappingValue());
		// Set initial scale snapping.
		SetTransformSnapping(ETransformType::Scale, EditorComponent->IsScaleSnappingEnabled(), EditorComponent->GetScaleSnappingValue());

		// Subscribe for transform snapping changes.
		EditorComponent->GetOnTransformSnappingChanged().AddWeakLambda(this, [=](const ETransformType Tool, const bool bEnabled, const float Value) {
			SetTransformSnapping(Tool, bEnabled, Value);
		});
	}
}

void UUIEditorToolbarWidget::SetTransformSpace(const ETransformSpace TransformSpace) const {
	switch (TransformSpace) {
	case ETransformSpace::World:
		if (IsValid(WorldSpaceButtonWidget)) {
			WorldSpaceButtonWidget->Show();
			TransformSpaceLabelWidget->SetText(FText::FromString("World"));
		}

		if (IsValid(LocalSpaceButtonWidget)) {
			LocalSpaceButtonWidget->Hide();
		}
		break;
	case ETransformSpace::Local:
		if (IsValid(LocalSpaceButtonWidget)) {
			LocalSpaceButtonWidget->Show();
			TransformSpaceLabelWidget->SetText(FText::FromString("Local"));
		}

		if (IsValid(WorldSpaceButtonWidget)) {
			WorldSpaceButtonWidget->Hide();
		}
		break;
	}
}

void UUIEditorToolbarWidget::SetTransformTool(const ETransformType Tool) const {
	if (IsValid(TranslateToolButtonWidget)) {
		TranslateToolButtonWidget->SetButtonPreset(EUIButtonPreset::Default);
	}

	if (IsValid(RotationToolButtonWidget)) {
		RotationToolButtonWidget->SetButtonPreset(EUIButtonPreset::Default);
	}

	if (IsValid(ScaleToolButtonWidget)) {
		ScaleToolButtonWidget->SetButtonPreset(EUIButtonPreset::Default);
	}

	switch (Tool) {
	case ETransformType::Translation:
		if (IsValid(TranslateToolButtonWidget)) {
			TranslateToolButtonWidget->SetButtonPreset(EUIButtonPreset::Action);
		}
		break;
	case ETransformType::Rotation:
		if (IsValid(RotationToolButtonWidget)) {
			RotationToolButtonWidget->SetButtonPreset(EUIButtonPreset::Action);
		}
		break;
	case ETransformType::Scale:
		if (IsValid(ScaleToolButtonWidget)) {
			ScaleToolButtonWidget->SetButtonPreset(EUIButtonPreset::Action);
		}
		break;
	default:
		break;
	}
}

void UUIEditorToolbarWidget::SetTransformSnapping(const ETransformType Tool, const bool bEnabled, const float Value) const {
	switch (Tool) {
	case ETransformType::Translation:
		if (IsValid(ToggleTranslationSnappingButtonWidget)) {
			if (bEnabled) {
				ToggleTranslationSnappingButtonWidget->SetButtonPreset(EUIButtonPreset::Action);
			} else {
				ToggleTranslationSnappingButtonWidget->SetButtonPreset(EUIButtonPreset::Default);
			}
		}

		if (IsValid(TranslationSnappingValueWidget)) {
			VeLog("Received snapping translation value: %.3f", Value);
			if (!FMath::IsNearlyEqual(TranslationSnappingValueWidget->GetValue(), Value)) {
				TranslationSnappingValueWidget->SetValue(Value);
			}
		}
		break;
	case ETransformType::Rotation:
		if (IsValid(ToggleRotationSnappingButtonWidget)) {
			if (bEnabled) {
				ToggleRotationSnappingButtonWidget->SetButtonPreset(EUIButtonPreset::Action);
			} else {
				ToggleRotationSnappingButtonWidget->SetButtonPreset(EUIButtonPreset::Default);
			}
		}

		if (IsValid(RotationSnappingValueWidget)) {
			VeLog("Received snapping rotation value: %.3f", Value);
			if (!FMath::IsNearlyEqual(RotationSnappingValueWidget->GetValue(), Value)) {
				RotationSnappingValueWidget->SetValue(Value);
			}
		}
		break;
	case ETransformType::Scale:
		if (IsValid(ToggleScaleSnappingButtonWidget)) {
			if (bEnabled) {
				ToggleScaleSnappingButtonWidget->SetButtonPreset(EUIButtonPreset::Action);
			} else {
				ToggleScaleSnappingButtonWidget->SetButtonPreset(EUIButtonPreset::Default);
			}
		}

		if (IsValid(ScaleSnappingValueWidget)) {
			VeLog("Received snapping scale value: %.3f", Value);
			if (!FMath::IsNearlyEqual(ScaleSnappingValueWidget->GetValue(), Value)) {
				ScaleSnappingValueWidget->SetValue(Value);
			}
		}
		break;
	default:
		break;
	}
}

void UUIEditorToolbarWidget::OnTranslationSnappingValueChanged(const float InValue) {
	if (!EditorComponent.IsValid()) {
		return;
	}

	VeLog("Sent snapping translation value: %.3f", InValue);
	EditorComponent->SetSnappingValue(ETransformType::Translation, InValue);

}

void UUIEditorToolbarWidget::OnRotationSnappingValueChanged(const float InValue) {
	if (!EditorComponent.IsValid()) {
		return;
	}

	VeLog("Sent snapping rotation value: %.3f", InValue);
	EditorComponent->SetSnappingValue(ETransformType::Rotation, InValue);
}

void UUIEditorToolbarWidget::OnScaleSnappingValueChanged(const float InValue) {
	if (!EditorComponent.IsValid()) {
		return;
	}

	VeLog("Sent snapping scale value: %.3f", InValue);
	EditorComponent->SetSnappingValue(ETransformType::Scale, InValue);
}
