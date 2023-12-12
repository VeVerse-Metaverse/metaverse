// 

#pragma once

#include "CoreMinimal.h"
#include "UIWidgetBase.h"
#include "Components/Image.h"
#include "UIRotatingSprite.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class VEUI_API UUIRotatingSprite final : public UUIWidgetBase {
	GENERATED_BODY()

public:
	/** Image to draw as the sprite. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Orbit")
	FSlateBrush ForegroundBrush;

	/** Widget sprite that will rotate. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Orbit", meta=(BindWidget))
	UImage* ForegroundWidget = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Orbit")
	bool bRotate = true;

	/** Current rotation angle. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Orbit")
	float Theta = 0.0f;

	/** Rotation speed. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Orbit")
	float RotationSpeed = 1.0f;

	/** Take random rotation speed in provided interval. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Orbit")
	bool bRandomRotationSpeed = true;

	/** Rotation speed. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Orbit", meta=(EditCondition="bRandomRotationSpeed"))
	float MinRotationSpeed = 0.0f;

	/** Rotation speed. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Orbit", meta=(EditCondition="bRandomRotationSpeed"))
	float MaxRotationSpeed = 1.0f;

	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
	virtual void NativeOnInitialized() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
};
