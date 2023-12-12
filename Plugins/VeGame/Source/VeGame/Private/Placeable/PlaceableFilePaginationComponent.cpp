// Authors: Egor A. Pristavka, Nikolay Bulatov. 
// Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "Placeable/PlaceableFilePaginationComponent.h"

#include "PlaceableComponent.h"
#include "VeShared.h"
#include "Net/UnrealNetwork.h"
#include "VeGameModule.h"
#include "Components/InteractionObjectComponent.h"
#include "Components/InteractionSubjectComponent.h"


UPlaceableFilePaginationComponent::UPlaceableFilePaginationComponent() {
	PrimaryComponentTick.bCanEverTick = false;

	SetIsReplicatedByDefault(true);
}

void UPlaceableFilePaginationComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UPlaceableFilePaginationComponent, CurrentPageUrl);
}

void UPlaceableFilePaginationComponent::BeginPlay() {
	Super::BeginPlay();

	// const auto* Owner = GetOwner();
	// if (IsValid(Owner)) {
	// 	if (auto* Object = Owner->FindComponentByClass<UInteractionObjectComponent>(); Object) {
	// 		Object->OnClientInputBegin.AddDynamic(this, &UPlaceableFilePaginationComponent::ClientInputBegin);
	// 		Object->OnServerBinaryEvent.AddDynamic(this, &UPlaceableFilePaginationComponent::ServerEvent);
	// 	} else {
	// 		VeLogErrorFunc("no object component");
	// 	}
	// } else {
	// 	VeLogErrorFunc("no owner");
	// }
}

void UPlaceableFilePaginationComponent::OnRep_CurrentPageUrl() const {
	OnPageChanged.Broadcast();
}

void UPlaceableFilePaginationComponent::Authority_FirstPage() {
	if (!HasAuthority()) return;

	CurrentPage = 0;
	if (PageUrls.Num() > 0) {
		CurrentPageUrl = PageUrls[CurrentPage];
	} else {
		CurrentPageUrl.Empty();
	}

	if (!IsRunningDedicatedServer()) {
		OnPageChanged.Broadcast();
	}
}

void UPlaceableFilePaginationComponent::Authority_NextPage() {
	if (!HasAuthority()) return;

	if (const auto PageUrlsNum = PageUrls.Num(); PageUrlsNum > 1) {
		CurrentPage = (CurrentPage + 1) % PageUrlsNum;
		CurrentPageUrl = PageUrls[CurrentPage];

		if (!IsRunningDedicatedServer()) {
			OnPageChanged.Broadcast();
		}
	}
}

void UPlaceableFilePaginationComponent::Authority_PreviousPage() {
	if (!HasAuthority()) return;

	if (const auto PageUrlsNum = PageUrls.Num(); PageUrlsNum > 1) {
		CurrentPage = (CurrentPage - 1 + PageUrlsNum) % PageUrlsNum;
		CurrentPageUrl = PageUrls[CurrentPage];

		if (!IsRunningDedicatedServer()) {
			OnPageChanged.Broadcast();
		}
	}
}

void UPlaceableFilePaginationComponent::ClientInputBegin(UInteractionSubjectComponent* Subject, UInteractionObjectComponent* Object, const FName& Name, UInputAction* Action, const FVector& Origin, const FVector& Location, const FVector& Normal) {
	if (Name == FName(EKeys::LeftMouseButton.GetDisplayName().ToString())) {
		Subject->RemoteServer_Event_Binary_BP(Object, FName(TEXT("PdfPage-")), {});
		return;
	}

	if (Name == FName(EKeys::RightMouseButton.GetDisplayName().ToString())) {
		Subject->RemoteServer_Event_Binary_BP(Object, FName(TEXT("PdfPage+")), {});
		return;
	}

	if (Name == FName(EKeys::R.GetDisplayName().ToString())) {
		Subject->RemoteServer_Event_Binary_BP(Object, FName(TEXT("PdfPage0")), {});
	}
}

void UPlaceableFilePaginationComponent::ServerEvent(UInteractionSubjectComponent* Subject, UInteractionObjectComponent* Object, const FName& Name, const TArray<uint8>& Args) {
	if (Name == FName(TEXT("PdfPage-"))) {
		Authority_PreviousPage();
		return;
	}

	if (Name == FName(TEXT("PdfPage+"))) {
		Authority_NextPage();
		return;
	}

	if (Name == FName(TEXT("PdfPage0"))) {
		Authority_FirstPage();
	}
}
