// Authors: Egor A. Pristavka, Nikolay Bulatov. 
// Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "PdfViewer.h"

#include "Components/UIImageWidget.h"
#include "Net/UnrealNetwork.h"
#include "VeInteraction/Public/Components/InteractionObjectComponent.h"
#include "VeInteraction/Public/Components/InteractionSubjectComponent.h"


// Sets default values
APdfViewer::APdfViewer() {
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	WidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("Widget"));
	WidgetComponent->SetupAttachment(RootComponent);

	bReplicates = true;
}

void APdfViewer::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APdfViewer, CurrentPageUrl);
}

// Called when the game starts or when spawned
void APdfViewer::BeginPlay() {
	Super::BeginPlay();
}

// Called every frame
void APdfViewer::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}

void APdfViewer::OnRep_CurrentPageUrl() {
	Update();
}

void APdfViewer::Update() {
	if (!IsRunningDedicatedServer()) {
		auto* Widget = Cast<UUIImageWidget>(WidgetComponent->GetWidget());
		if (IsValid(Widget)) {
			Widget->ShowImage(CurrentPageUrl);
		}
	}
}

void APdfViewer::OnPlaceableMetadataChanged(const FVePlaceableMetadata& InMetadata) {
	Super::OnPlaceableMetadataChanged(InMetadata);

	PageUrls.Empty();
	for (auto File : InMetadata.Files) {
		if (File.Type == EApiFileType::PdfImage) {
			PageUrls.Emplace(File.Url);
		}
	}

	Authority_FirstPage();
}

void APdfViewer::Authority_FirstPage() {
	if (HasAuthority()) {
		CurrentPage = 0;
		if (PageUrls.Num() > 0) {
			CurrentPageUrl = PageUrls[CurrentPage];
		} else {
			CurrentPageUrl = TEXT("");
		}
		if (!IsRunningDedicatedServer()) {
			OnRep_CurrentPageUrl();
		}
	}
}

void APdfViewer::Authority_NextPage() {
	if (HasAuthority()) {
		if (PageUrls.Num() > 1) {
			CurrentPage = (CurrentPage + 1) % PageUrls.Num();
			CurrentPageUrl = PageUrls[CurrentPage];
			if (!IsRunningDedicatedServer()) {
				OnRep_CurrentPageUrl();
			}
		}
	}
}

void APdfViewer::Authority_PreviousPage() {
	if (HasAuthority()) {
		if (PageUrls.Num() > 1) {
			CurrentPage = (CurrentPage - 1 + PageUrls.Num()) % PageUrls.Num();
			CurrentPageUrl = PageUrls[CurrentPage];
			if (!IsRunningDedicatedServer()) {
				OnRep_CurrentPageUrl();
			}
		}
	}
}

void APdfViewer::ClientInputBegin_Implementation(UInteractionSubjectComponent* Subject, UInteractionObjectComponent* Object, const FName& Name, const FVector& Origin, const FVector& Location, const FVector& Normal) {
	if (Name == FName(EKeys::LeftMouseButton.GetDisplayName().ToString())) {
		Subject->RemoteServer_Event_Binary_BP(Object, FName(TEXT("APdfViewer_Prev")), TArray<uint8>());
		return;
	}
	
	if (Name == FName(EKeys::RightMouseButton.GetDisplayName().ToString())) {
		Subject->RemoteServer_Event_Binary_BP(Object, FName(TEXT("APdfViewer_Next")), TArray<uint8>());
		return;
	}
	
	if (Name == FName(EKeys::BackSpace.GetDisplayName().ToString())) {
		Subject->RemoteServer_Event_Binary_BP(Object, FName(TEXT("APdfViewer_Home")), TArray<uint8>());
		return;
	}

	Super::ClientInputBegin_Implementation(Subject, Object, Name, Origin, Location, Normal);
}

void APdfViewer::ServerEvent_Implementation(UInteractionSubjectComponent* Subject, UInteractionObjectComponent* Object, const FName& Name, const TArray<uint8>& Args) {
	if (Name == FName(TEXT("APdfViewer_Prev"))) {
		Authority_PreviousPage();
		return;
	}

	if (Name == FName(TEXT("APdfViewer_Next"))) {
		Authority_NextPage();
		return;
	}

	if (Name == FName(TEXT("APdfViewer_Home"))) {
		Authority_FirstPage();
		return;
	}

	Super::ServerEvent_Implementation(Subject, Object, Name, Args);
}
