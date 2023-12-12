// Author: Egor A. Pristavka. Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.


#include "UIBlockchainNftWidget.h"

#include "BlockchainNft.h"
#include "VeQRCode.h"
#include "Components/Image.h"

void UUIBlockchainNftWidget::RegisterCallbacks() {
	if (IsValid(MintButtonWidget)) {
		MintButtonWidget->GetOnButtonClicked().AddWeakLambda(this, [=] {
			SwitchToMintQrCodeWidget();
		});

		CloseQRCodeButtonWidget->GetOnButtonClicked().AddWeakLambda(this, [=] {
			SwitchToMintMode();
		});
	}

	Super::RegisterCallbacks();
}

void UUIBlockchainNftWidget::UnregisterCallbacks() {
	Super::UnregisterCallbacks();
}

void UUIBlockchainNftWidget::NativeOnTextureLoaded(const FTextureLoadResult& Result) {
	if (ImageWidget) {
		ImageWidget->SetBrushResourceObject(Result.Texture.Get());
	}
}

void UUIBlockchainNftWidget::SwitchToMintMode() const {
	if (QrCodeWidget) {
		QrCodeWidget->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (MintWidget) {
		MintWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}

	if (InfoWidget) {
		InfoWidget->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (GeneralInfoWidget) {
		GeneralInfoWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}

	if (BlockchainNftActor.IsValid()) {
		BlockchainNftActor->SetReadyForMint(false);
	}
}

void UUIBlockchainNftWidget::SwitchToMintQrCodeWidget() {
	if (QrCodeWidget) {
		QrCodeWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}

	if (GeneralInfoWidget) {
		GeneralInfoWidget->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (MintWidget) {
		MintWidget->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (InfoWidget) {
		InfoWidget->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (BlockchainNftActor.IsValid()) {
		BlockchainNftActor->SetReadyForMint(true);
	}
}

void UUIBlockchainNftWidget::SwitchToInfoMode() const {
	if (QrCodeWidget) {
		QrCodeWidget->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (MintWidget) {
		MintWidget->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (GeneralInfoWidget) {
		GeneralInfoWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}

	if (InfoWidget) {
		InfoWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}

	if (BlockchainNftActor.IsValid()) {
		BlockchainNftActor->SetReadyForMint(false);
	}
}

void UUIBlockchainNftWidget::SetName(const FString& InName) {
	if (IsValid(NameTextWidget)) {
		NameTextWidget->SetText(FText::FromString(InName));
	}
}

void UUIBlockchainNftWidget::SetDescription(const FString& InDescription) {
	if (IsValid(DescriptionTextWidget)) {
		DescriptionTextWidget->SetText(FText::FromString(InDescription));
	}
}

void UUIBlockchainNftWidget::SetOwnerName(const FString& InOwnerName) {
	OwnerName = InOwnerName;
	if (IsValid(OwnerTextWidget)) {
		if (!OwnerName.IsEmpty()) {
			OwnerTextWidget->SetText(FText::FromString(FString::Printf(TEXT("%s\n%s...%s"), *InOwnerName, *OwnerAddress.Left(6), *OwnerAddress.Right(4))));
		} else {
			OwnerTextWidget->SetText(FText::FromString(FString::Printf(TEXT("%s...%s"), *OwnerAddress.Left(6), *OwnerAddress.Right(4))));
		}
	}
}

void UUIBlockchainNftWidget::SetOwnerAddress(const FString& InOwnerAddress) {
	OwnerAddress = InOwnerAddress;
	if (IsValid(OwnerTextWidget)) {
		if (!OwnerName.IsEmpty()) {
			OwnerTextWidget->SetText(FText::FromString(FString::Printf(TEXT("%s\n%s...%s"), *OwnerName, *OwnerAddress.Left(6), *OwnerAddress.Right(4))));
		} else {
			OwnerTextWidget->SetText(FText::FromString(FString::Printf(TEXT("%s...%s"), *OwnerAddress.Left(6), *OwnerAddress.Right(4))));
		}
	}
}

void UUIBlockchainNftWidget::SetImageUrl(const FString& InImageUrl) {
	RequestTexture(this, InImageUrl);
}

void UUIBlockchainNftWidget::UpdateQrCode(const FString& InUrl) {
	UTexture2D* QrCodeTexture = FVeQRCodeModule::GenerateQrCode(InUrl);

	if (IsValid(QrCodeTexture) && IsValid(QrCodeImageWidget)) {
		QrCodeImageWidget->SetBrushFromTexture(QrCodeTexture);
	}
}

void UUIBlockchainNftWidget::OnMintSuccess() {
	SwitchToInfoMode();

	OnMintSuccessBP();
}
