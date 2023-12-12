// Authors: Khusan T.Yadgarov, Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022-2023 VeVerse AS. All rights reserved.
// Copyright (c) 2023 LE7EL AS. All rights reserved.


#include "UIRotatingSprite.h"

void UUIRotatingSprite::NativePreConstruct() {
	Super::NativePreConstruct();

	if (ForegroundWidget) {
		ForegroundWidget->SetBrush(ForegroundBrush);
	}

	Theta = FMath::RandRange(0.0f, TWO_PI);

	if (bRandomRotationSpeed) {
		RotationSpeed = FMath::RandRange(MinRotationSpeed, MaxRotationSpeed);
	}
}

void UUIRotatingSprite::NativeConstruct() {
	Super::NativeConstruct();

	if (bRandomRotationSpeed) {
		RotationSpeed = FMath::RandRange(MinRotationSpeed, MaxRotationSpeed);
	}
}

void UUIRotatingSprite::NativeOnInitialized() {
	Super::NativeOnInitialized();
}

void UUIRotatingSprite::NativeTick(const FGeometry& MyGeometry, const float InDeltaTime) {
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (ForegroundWidget && bRotate) {
		// Add theta.
		Theta = FMath::UnwindRadians(Theta + RotationSpeed * InDeltaTime);

		// Update the render transform of the sprite.
		ForegroundWidget->SetRenderTransformAngle(Theta);
	}
}
