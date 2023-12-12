#include "Editor/UIEditorTransformWidget.h"

#include "GameFrameworkEditorSubsystem.h"
#include "VeShared.h"
#include "VeGameFramework.h"
#include "EditorComponent.h"
#include "Components/SpinBox.h"


void UUIEditorTransformWidget::NativeOnInitialized() {
	Super::NativeOnInitialized();

	if (IsValid(PositionXSpinBoxWidget)) {
		PositionXSpinBoxWidget->OnValueChanged.AddDynamic(this, &UUIEditorTransformWidget::OnSpinBoxChanged);
		PositionXSpinBoxWidget->OnValueCommitted.AddDynamic(this, &UUIEditorTransformWidget::OnSpinBoxCommitted);
	}
	if (IsValid(PositionYSpinBoxWidget)) {
		PositionYSpinBoxWidget->OnValueChanged.AddDynamic(this, &UUIEditorTransformWidget::OnSpinBoxChanged);
		PositionYSpinBoxWidget->OnValueCommitted.AddDynamic(this, &UUIEditorTransformWidget::OnSpinBoxCommitted);
	}
	if (IsValid(PositionZSpinBoxWidget)) {
		PositionZSpinBoxWidget->OnValueChanged.AddDynamic(this, &UUIEditorTransformWidget::OnSpinBoxChanged);
		PositionZSpinBoxWidget->OnValueCommitted.AddDynamic(this, &UUIEditorTransformWidget::OnSpinBoxCommitted);
	}

	if (IsValid(RotationXSpinBoxWidget)) {
		RotationXSpinBoxWidget->OnValueChanged.AddDynamic(this, &UUIEditorTransformWidget::OnSpinBoxChanged);
		RotationXSpinBoxWidget->OnValueCommitted.AddDynamic(this, &UUIEditorTransformWidget::OnSpinBoxCommitted);
	}
	if (IsValid(RotationYSpinBoxWidget)) {
		RotationYSpinBoxWidget->OnValueChanged.AddDynamic(this, &UUIEditorTransformWidget::OnSpinBoxChanged);
		RotationYSpinBoxWidget->OnValueCommitted.AddDynamic(this, &UUIEditorTransformWidget::OnSpinBoxCommitted);
	}
	if (IsValid(RotationZSpinBoxWidget)) {
		RotationZSpinBoxWidget->OnValueChanged.AddDynamic(this, &UUIEditorTransformWidget::OnSpinBoxChanged);
		RotationZSpinBoxWidget->OnValueCommitted.AddDynamic(this, &UUIEditorTransformWidget::OnSpinBoxCommitted);
	}

	if (IsValid(ScaleXSpinBoxWidget)) {
		ScaleXSpinBoxWidget->OnValueChanged.AddDynamic(this, &UUIEditorTransformWidget::OnSpinBoxChanged);
		ScaleXSpinBoxWidget->OnValueCommitted.AddDynamic(this, &UUIEditorTransformWidget::OnSpinBoxCommitted);
	}
	if (IsValid(ScaleYSpinBoxWidget)) {
		ScaleYSpinBoxWidget->OnValueChanged.AddDynamic(this, &UUIEditorTransformWidget::OnSpinBoxChanged);
		ScaleYSpinBoxWidget->OnValueCommitted.AddDynamic(this, &UUIEditorTransformWidget::OnSpinBoxCommitted);
	}
	if (IsValid(ScaleZSpinBoxWidget)) {
		ScaleZSpinBoxWidget->OnValueChanged.AddDynamic(this, &UUIEditorTransformWidget::OnSpinBoxChanged);
		ScaleZSpinBoxWidget->OnValueCommitted.AddDynamic(this, &UUIEditorTransformWidget::OnSpinBoxCommitted);
	}
}

void UUIEditorTransformWidget::NativeConstruct() {
	Super::NativeConstruct();

	GET_MODULE_SUBSYSTEM(EditorSubsystem, GameFramework, Editor);
	if (EditorSubsystem) {
		EditorSubsystem->GetOnEditorComponentChanged().AddUObject(this, &UUIEditorTransformWidget::BindToEditorComponent);
		BindToEditorComponent(EditorSubsystem->GetEditorComponent(), nullptr);
	}
}

void UUIEditorTransformWidget::NativeDestruct() {
	Super::NativeDestruct();

	GET_MODULE_SUBSYSTEM(EditorSubsystem, GameFramework, Editor);
	if (EditorSubsystem) {
		EditorSubsystem->GetOnEditorComponentChanged().RemoveAll(this);
		BindToEditorComponent(nullptr, EditorSubsystem->GetEditorComponent());
	}
}

void UUIEditorTransformWidget::BindToEditorComponent(UEditorComponent* NewEditorComponent, UEditorComponent* OldEditorComponent) {
	if (OldEditorComponent) {
		OldEditorComponent->GetOnSelectionChanged().RemoveAll(this);
		OldEditorComponent->GetOnSelectionTransformChanged().RemoveAll(this);
	}

	if (NewEditorComponent) {
		NewEditorComponent->GetOnSelectionChanged().AddWeakLambda(this, [=](const TArray<AActor*>& Actors) {
			if (Actors.Num() > 0) {
				++LockChanges;
				SetTransform(Actors[0]->GetActorTransform());
				--LockChanges;
			}
		});

		NewEditorComponent->GetOnSelectionTransformChanged().AddWeakLambda(this, [=](const FTransform& InTransform) {
			++LockChanges;
			SetTransform(InTransform);
			--LockChanges;
		});
	}
}

void UUIEditorTransformWidget::OnTransformChanged() {
	if (LockChanges) {
		return;
	}

	GET_MODULE_SUBSYSTEM(EditorSubsystem, GameFramework, Editor);
	if (EditorSubsystem) {
		FTransform Transform = GetTransform();

		if (auto* EditorComponent = EditorSubsystem->GetEditorComponent(); IsValid(EditorComponent)) {
			EditorComponent->ApplyTransform(Transform);

			if (FinalTransform) {
				FinalTransform = false;
				EditorComponent->Owner_OnTransformComplete();
			}
		}
	}
}

void UUIEditorTransformWidget::OnSpinBoxChanged(float InValue) {
	OnTransformChanged();
}

void UUIEditorTransformWidget::OnSpinBoxCommitted(float InValue, ETextCommit::Type CommitMethod) {
	FinalTransform = true;
}

FVector UUIEditorTransformWidget::GetLocation() const {
	FVector Location = FVector::ZeroVector;

	if (IsValid(PositionXSpinBoxWidget)) {
		Location.X = PositionXSpinBoxWidget->GetValue();
	}

	if (IsValid(PositionYSpinBoxWidget)) {
		Location.Y = PositionYSpinBoxWidget->GetValue();
	}

	if (IsValid(PositionZSpinBoxWidget)) {
		Location.Z = PositionZSpinBoxWidget->GetValue();
	}

	return Location;
}

void UUIEditorTransformWidget::SetLocation(const FVector& Value) {
	if (GetLocation() != Value) {
		++LockChanges;

		if (IsValid(PositionXSpinBoxWidget)) {
			PositionXSpinBoxWidget->SetValue(Value.X);
		}

		if (IsValid(PositionYSpinBoxWidget)) {
			PositionYSpinBoxWidget->SetValue(Value.Y);
		}

		if (IsValid(PositionZSpinBoxWidget)) {
			PositionZSpinBoxWidget->SetValue(Value.Z);
		}

		--LockChanges;
		OnTransformChanged();
	}
}

FQuat UUIEditorTransformWidget::GetRotation() const {
	FRotator Rotator = FRotator::ZeroRotator;

	if (IsValid(RotationXSpinBoxWidget)) {
		Rotator.Roll = RotationXSpinBoxWidget->GetValue();
	}

	if (IsValid(RotationYSpinBoxWidget)) {
		Rotator.Pitch = RotationYSpinBoxWidget->GetValue();
	}

	if (IsValid(RotationZSpinBoxWidget)) {
		Rotator.Yaw = RotationZSpinBoxWidget->GetValue();
	}

	return Rotator.Quaternion();
}

void UUIEditorTransformWidget::SetRotation(const FQuat& Value) {
	if (GetRotation() != Value) {
		++LockChanges;

		FRotator Rotation(Value);

		if (IsValid(RotationXSpinBoxWidget)) {
			RotationXSpinBoxWidget->SetValue(Rotation.Roll);
		}

		if (IsValid(RotationYSpinBoxWidget)) {
			RotationYSpinBoxWidget->SetValue(Rotation.Pitch);
		}

		if (IsValid(RotationZSpinBoxWidget)) {
			RotationZSpinBoxWidget->SetValue(Rotation.Yaw);
		}

		--LockChanges;
		OnTransformChanged();
	}
}

FVector UUIEditorTransformWidget::GetScale() const {
	FVector Result;

	if (IsValid(ScaleXSpinBoxWidget)) {
		Result.X = ScaleXSpinBoxWidget->GetValue();
	}

	if (IsValid(ScaleYSpinBoxWidget)) {
		Result.Y = ScaleYSpinBoxWidget->GetValue();
	}

	if (IsValid(ScaleZSpinBoxWidget)) {
		Result.Z = ScaleZSpinBoxWidget->GetValue();
	}

	return Result;
}

void UUIEditorTransformWidget::SetScale(const FVector& Value) {
	if (GetScale() != Value) {
		++LockChanges;

		if (IsValid(ScaleXSpinBoxWidget)) {
			ScaleXSpinBoxWidget->SetValue(Value.X);
		}

		if (IsValid(ScaleYSpinBoxWidget)) {
			ScaleYSpinBoxWidget->SetValue(Value.Y);
		}

		if (IsValid(ScaleZSpinBoxWidget)) {
			ScaleZSpinBoxWidget->SetValue(Value.Z);
		}

		--LockChanges;
		OnTransformChanged();
	}
}

FTransform UUIEditorTransformWidget::GetTransform() const {
	FTransform Result;
	Result.SetLocation(GetLocation());
	Result.SetRotation(GetRotation());
	Result.SetScale3D(GetScale());
	return Result;
}

void UUIEditorTransformWidget::SetTransform(const FTransform& InTransform) {
	if (!GetTransform().Equals(InTransform)) {
		++LockChanges;

		SetLocation(InTransform.GetLocation());
		SetRotation(InTransform.GetRotation());
		SetScale(InTransform.GetScale3D());

		--LockChanges;
		OnTransformChanged();
	}
}
