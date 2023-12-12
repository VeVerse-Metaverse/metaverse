#pragma once

#include "UIWidgetBase.h"
#include "UIEditorTransformWidget.generated.h"

UCLASS()
class VEUI_API UUIEditorTransformWidget final : public UUIWidgetBase {
	GENERATED_BODY()

public:
#pragma region Widgets

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class USpinBox* PositionXSpinBoxWidget;
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class USpinBox* PositionYSpinBoxWidget;
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class USpinBox* PositionZSpinBoxWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class USpinBox* RotationXSpinBoxWidget;
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class USpinBox* RotationYSpinBoxWidget;
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class USpinBox* RotationZSpinBoxWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class USpinBox* ScaleXSpinBoxWidget;
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class USpinBox* ScaleYSpinBoxWidget;
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class USpinBox* ScaleZSpinBoxWidget;

#pragma endregion Widgets

	FVector GetLocation() const;
	void SetLocation(const FVector& Value);

	FQuat GetRotation() const;
	void SetRotation(const FQuat& Value);

	FVector GetScale() const;
	void SetScale(const FVector& Value);

	FTransform GetTransform() const;
	void SetTransform(const FTransform& InTransform);

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	void BindToEditorComponent(class UEditorComponent* NewEditorComponent, class UEditorComponent* OldEditorComponent);

	UFUNCTION()
	void OnTransformChanged();

	UFUNCTION()
	void OnSpinBoxChanged(float InValue);

	UFUNCTION()
	void OnSpinBoxCommitted(float InValue, ETextCommit::Type CommitMethod);

private:
	uint8 LockChanges = 0;
	bool FinalTransform = false;
};

