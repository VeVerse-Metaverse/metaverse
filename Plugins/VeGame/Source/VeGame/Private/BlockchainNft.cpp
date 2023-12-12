// Author: Egor A. Pristavka. Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#include "BlockchainNft.h"

#include "VeBlockchain.h"
#include "BlockchainNftSubsystem.h"
#include "VeShared.h"
#include "VeApi.h"
#include "VeApi2.h"
#include "Api2AuthSubsystem.h"

ABlockchainNft::ABlockchainNft()
	: AInteractiveActor() {
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	WidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComponent"));
	if (IsValid(WidgetComponent)) {
		WidgetComponent->SetupAttachment(RootComponent);
		WidgetComponent->SetRelativeLocation(FVector{0.0f, 0.0f, 532.0f});
	}
}

void ABlockchainNft::BeginPlay() {
	Super::BeginPlay();

	if (IsValid(WidgetComponent)) {
		if (!IsValid(Widget)) {
			Widget = Cast<UUIBlockchainNftWidget>(WidgetComponent->GetWidget());
			if (!IsValid(Widget)) {
				Widget = Cast<UUIBlockchainNftWidget>(CreateWidget<UUIWidgetBase>(GetWorld(), WidgetClass));
				WidgetComponent->SetWidget(Widget);
			}
		}

		WidgetComponent->SetHiddenInGame(true);
	}

	if (IsValid(Widget)) {
		Widget->BlockchainNftActor = MakeWeakObjectPtr(this);

		GET_MODULE_SUBSYSTEM(AuthSubsystem, Api2, Auth);
		if (AuthSubsystem) {
			Widget->UpdateQrCode(FString::Printf(TEXT("%s?tokenId=%d&key=%s"), *Api::GetW3MintLandUrl(), TokenId, *AuthSubsystem->GetUserApiKey()));
		}
	} else {
		LogErrorF("widget is nullptr");
	}
}

void ABlockchainNft::ShowWidgetComponent() const {
	if (IsValid(WidgetComponent)) {
		WidgetComponent->SetHiddenInGame(false);
	}
}

void ABlockchainNft::HideWidgetComponent() const {
	if (IsValid(WidgetComponent)) {
		WidgetComponent->SetHiddenInGame(true);
	}
}

void ABlockchainNft::RequestMetadata() {
	RequestOwnerAddress();
}

bool ABlockchainNft::RequestOwnerAddress() {
	if (FVeBlockchainModule* BlockchainModule = FVeBlockchainModule::Get()) {
		if (const TSharedPtr<FBlockchainNftSubsystem> NftSubsystem = BlockchainModule->GetNftSubsystem()) {
			TSharedPtr<FOnOwnerOfRequestCompleted> CallbackPtr = MakeShareable(new FOnOwnerOfRequestCompleted{});

			CallbackPtr->BindWeakLambda(this, [=](const FString& InOwnerAddress, const bool bInSuccessful, const FString InRequestError) mutable {
				if (!bInSuccessful) {
					LogErrorF("failed to send get owner of request: %s", *InRequestError);
				} else {
					SetOwnerAddress(InOwnerAddress);

					if (!InOwnerAddress.IsEmpty()) {
						GET_MODULE_SUBSYSTEM(UserSubsystem, Api, User);
						if (UserSubsystem) {
							TSharedPtr<FOnUserRequestCompleted> UserCallbackPtr = MakeShareable(new FOnUserRequestCompleted{});
							UserCallbackPtr->BindWeakLambda(
								this, [=](const FApiUserMetadata& InUserMetadata, const bool bInUserRequestSuccessful, const FString InUserRequestError) mutable {
									if (bInUserRequestSuccessful) {
										SetOwnerName(InUserMetadata.Name);
									} else {
										LogErrorF("unsuccessful request: %s", *InUserRequestError);
									}
									UserCallbackPtr.Reset();
								});

							UserSubsystem->GetUserByEthAddress(InOwnerAddress, UserCallbackPtr);
						}
					}
				}

				RequestTokenMetadata();

				CallbackPtr.Reset();
			});

			return NftSubsystem->GetOwnerOf(TokenId, CallbackPtr);
		} else { LogErrorF("failed to get subsystem F%s%sSubsystem", STRINGIFY(Blockchain), STRINGIFY(Nft)); }
	} else { LogErrorF("failed to get module FVe%sModule", STRINGIFY(Blockchain)); }

	return false;
}

bool ABlockchainNft::RequestTokenMetadata() {

	if (FVeBlockchainModule* BlockchainModule = FVeBlockchainModule::Get()) {
		if (const TSharedPtr<FBlockchainNftSubsystem> NftSubsystem = BlockchainModule->GetNftSubsystem()) {
			const TSharedPtr<FOnTokenUriRequestCompleted> CallbackPtr = MakeShareable(new FOnTokenUriRequestCompleted{});

			CallbackPtr->BindWeakLambda(this, [=](const FBlockchainTokenMetadata& InMetadata, const bool bInSuccessful, const FString InRequestError) mutable {
				if (!bInSuccessful) {
					LogErrorF("failed to send get owner of request: %s", *InRequestError);
				} else {
					OnMetadataReceived(InMetadata);
				}
			});

			return NftSubsystem->GetTokenUri(TokenId, CallbackPtr);
		} else { LogErrorF("failed to get subsystem F%s%sSubsystem", STRINGIFY(Blockchain), STRINGIFY(Nft)); }
	} else { LogErrorF("failed to get module FVe%sModule", STRINGIFY(Blockchain)); }

	return false;
}

void ABlockchainNft::SetOwnerAddress(const FString& InOwnerAddress) {
	if (IsValid(Widget)) {
		Widget->SetOwnerAddress(InOwnerAddress);
	}

	if (bReadyForMint) {
		if (!InOwnerAddress.IsEmpty()) {
			// Detect that the owner address has changed
			if (OwnerAddress != InOwnerAddress) {
				Widget->OnMintSuccess();
			}
		}
	}

	if (OwnerAddress != InOwnerAddress) {
		OnOwnerAddressChanged();
	}

	OwnerAddress = InOwnerAddress;
}

void ABlockchainNft::SetOwnerName(const FString& InOwnerName) {
	OwnerName = InOwnerName;
	if (IsValid(Widget)) {
		Widget->SetOwnerName(InOwnerName);
	}
}

void ABlockchainNft::SetName(const FString& InName) {
	Name = InName;
	if (IsValid(Widget)) {
		Widget->SetName(InName);
	}
}

void ABlockchainNft::SetDescription(const FString& InDescription) {
	Description = InDescription;
	if (IsValid(Widget)) {
		Widget->SetDescription(InDescription);
	}
}

void ABlockchainNft::SetImageUrl(const FString& InImageUrl) const {
	if (IsValid(Widget)) {
		Widget->SetImageUrl(InImageUrl);
	}
}

EBlockchainNftOwnership ABlockchainNft::GetOwnership() const {
	if (!OwnerAddress.IsEmpty()) {
		GET_MODULE_SUBSYSTEM(AuthSubsystem, Api2, Auth);
		if (AuthSubsystem) {
			if (AuthSubsystem->GetUserMetadata().EthAddress.ToLower() == OwnerAddress.ToLower()) {
				return EBlockchainNftOwnership::Self;
			} else {
				return EBlockchainNftOwnership::Other;
			}
		}
	}

	return EBlockchainNftOwnership::None;
}

void ABlockchainNft::OnMetadataReceived(const FBlockchainTokenMetadata& InMetadata) {
	SetName(InMetadata.Name);
	SetDescription(InMetadata.Description);
	SetImageUrl(InMetadata.Image);

	if (Name != InMetadata.Name || Description != InMetadata.Description || ImageUrl != InMetadata.Image) {
		OnMetadataChanged();
	}
}

void ABlockchainNft::SetReadyForMint(const bool bInReadyForMint) {
	bReadyForMint = bInReadyForMint;
}
