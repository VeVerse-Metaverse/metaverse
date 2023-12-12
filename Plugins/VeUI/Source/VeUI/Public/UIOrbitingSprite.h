// 

#pragma once

#include "CoreMinimal.h"
#include "UIWidgetBase.h"
#include "Components/Image.h"
#include "UIOrbitingSprite.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class VEUI_API UUIOrbitingSprite final : public UUIWidgetBase {
	GENERATED_BODY()

public:
	/** Image to draw as the sprite. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Orbit")
	FSlateBrush ForegroundBrush;

	/** Image to draw as the orbit. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Orbit")
	FSlateBrush BackgroundBrush;

	/** Calculate origin and size from the background widget. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Orbit")
	bool bCalculateOrbitPropertiesUsingBackgroundWidget = true;

	/** Background orbit widget. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Orbit", meta=(BindWidget))
	UImage* BackgroundWidget = nullptr;

	/** Widget sprite that will orbit around the origin point. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Orbit", meta=(BindWidget))
	UImage* ForegroundWidget = nullptr;

	/** Current orbit angle. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Orbit")
	float Theta = 0.0f;

	/** Orbit speed. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Orbit")
	float OrbitSpeed = 1.0f;

	/** Take random orbit speed in provided interval. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Orbit")
	bool bRandomOrbitSpeed = true;

	/** Orbit speed. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Orbit", meta=(EditCondition="bRandomOrbitSpeed"))
	float MinOrbitSpeed = 0.0f;

	/** Orbit speed. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Orbit", meta=(EditCondition="bRandomOrbitSpeed"))
	float MaxOrbitSpeed = 1.0f;

	/** Orbit radius. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Orbit")
	float Radius = 100.0f;

	/** Center point of the orbit. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Orbit")
	FVector2D Origin = {0.0f, 0.0f};

	/** Current offset calculated in polar coordinates and converted to cartesian, added to the origin to convert to screen coordinates. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Orbit")
	FVector2D Offset = {0.0f, 0.0f};

	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
	virtual void NativeOnInitialized() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
};
