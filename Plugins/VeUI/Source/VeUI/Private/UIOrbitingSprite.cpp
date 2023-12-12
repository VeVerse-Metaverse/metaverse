// 

#include "UIOrbitingSprite.h"

void UUIOrbitingSprite::NativePreConstruct() {
	Super::NativePreConstruct();

	if (ForegroundWidget) {
		ForegroundWidget->SetBrush(ForegroundBrush);
	}

	if (BackgroundWidget) {
		BackgroundWidget->SetBrush(BackgroundBrush);
	}

	Theta = FMath::RandRange(0.0f, TWO_PI);

	if (bRandomOrbitSpeed) {
		OrbitSpeed = FMath::RandRange(MinOrbitSpeed, MaxOrbitSpeed);
	}
}

void UUIOrbitingSprite::NativeConstruct() {
	Super::NativeConstruct();

	if (bRandomOrbitSpeed) {
		OrbitSpeed = FMath::RandRange(MinOrbitSpeed, MaxOrbitSpeed);
	}

	if (bCalculateOrbitPropertiesUsingBackgroundWidget && BackgroundWidget) {
		const auto OrbitWidgetSize = BackgroundWidget->Brush.GetImageSize();
		const auto OrbitWidgetOrigin = BackgroundWidget->GetRenderTransformPivot();
		const auto OrbitWidgetScale = BackgroundWidget->GetRenderTransform().Scale;
		const auto OrbitDiameter = FMath::Min(OrbitWidgetSize.X * OrbitWidgetScale.X, OrbitWidgetSize.Y * OrbitWidgetScale.Y);

		Origin = {OrbitWidgetSize.X * OrbitWidgetOrigin.X * OrbitWidgetScale.X, OrbitWidgetSize.Y * OrbitWidgetOrigin.Y * OrbitWidgetScale.Y};
		Radius = OrbitDiameter / 2;
	}
}

void UUIOrbitingSprite::NativeOnInitialized() {
	Super::NativeOnInitialized();
}

void UUIOrbitingSprite::NativeTick(const FGeometry& MyGeometry, const float InDeltaTime) {
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (ForegroundWidget) {
		// Add theta.
		Theta = FMath::UnwindRadians(Theta + OrbitSpeed * InDeltaTime);

		// Convert polar coordinates to cartesian and store it to the offset.
		FMath::SinCos(&Offset.X, &Offset.Y, Theta);
		Offset.X *= Radius;
		Offset.Y *= Radius;

		Offset.X -= ForegroundWidget->Brush.GetImageSize().X / 2;
		Offset.Y -= ForegroundWidget->Brush.GetImageSize().Y / 2;

		// Update the render transform of the sprite.
		ForegroundWidget->SetRenderTranslation(Origin + Offset);
	}
}
