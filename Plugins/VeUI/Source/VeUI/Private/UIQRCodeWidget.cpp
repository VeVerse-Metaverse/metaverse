// Authors: Khusan T.Yadgarov, Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022-2023 VeVerse AS. All rights reserved.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "UIQRCodeWidget.h"

#include "VeShared.h"
#include "VeApi2.h"
#include "Api2AuthSubsystem.h"
#include "VeQRCode.h"
#include "Components/Image.h"


void UUIQRCodeWidget::NativeOnInitialized() {
	Super::NativeOnInitialized();
}

void UUIQRCodeWidget::NativeConstruct() {
	Super::NativeConstruct();
}

void UUIQRCodeWidget::NativeDestruct() {
	Super::NativeDestruct();
}

void UUIQRCodeWidget::Refresh() {
#if WITH_QRCODE
	GET_MODULE_SUBSYSTEM(AuthSubsystem, Api2, Auth);
	if (AuthSubsystem) {
		const FString ApiKey = AuthSubsystem->GetUserApiKey();
		const FString Url = FString::Printf(TEXT("%s?key=%s"), *Api::GetW3LoginUrl(), *ApiKey);

		UTexture2D* QrCodeTexture = FVeQRCodeModule::GenerateQrCode(Url);

		if (IsValid(QrCodeTexture) && IsValid(QrCodeWidget)) {
			QrCodeWidget->SetBrushFromTexture(QrCodeTexture);
		}
	}
#endif
}
