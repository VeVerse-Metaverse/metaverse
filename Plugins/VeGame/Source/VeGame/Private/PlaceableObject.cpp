// Authors: Egor A. Pristavka, Nikolay Bulatov. 
// Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "PlaceableObject.h"

#include "ApiPropertySubsystem.h"
#include "UI/DEPRECATED_UIPlaceableDialogWidget.h"
#include "VeGameModule.h"
#include "VeInteraction/Public/Components/InteractionObjectComponent.h"
#include "VeInteraction/Public/Components/InteractionSubjectComponent.h"
#include "VeShared.h"
#include "VeApi.h"
#include "Net/UnrealNetwork.h"


// Sets default values
APlaceableObject::APlaceableObject() {
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void APlaceableObject::BeginPlay() {
	Super::BeginPlay();

}

// Called every frame
void APlaceableObject::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

}

void APlaceableObject::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APlaceableObject, SharedPlaceableMetadata);
}

//==============================================================================

void APlaceableObject::ClientInputBegin_Implementation(UInteractionSubjectComponent* Subject, UInteractionObjectComponent* Object, const FName& Name, const FVector& Origin, const FVector& Location, const FVector& Normal) {
	// Super::ClientInputBegin_Implementation(Subject, Object, Name, Origin, Location, Normal);

	if (Subject->GetPawn()->IsLocallyControlled()) {
		ShowDialog(Subject, Object);
	}
}

void APlaceableObject::SetPlaceableMetadata(const FVePlaceableMetadata& InPlaceableMetadata) {
	SharedPlaceableMetadata = InPlaceableMetadata;
	OnPlaceableMetadataChanged(InPlaceableMetadata);
}

void APlaceableObject::OnRep_PlaceableMetadata(const FVePlaceableMetadata& OldValue) {
	OnPlaceableMetadataChanged(SharedPlaceableMetadata);
}

void APlaceableObject::OnPlaceableMetadataChanged(const FVePlaceableMetadata& InMetadata) { }

void APlaceableObject::ShowDialog(UInteractionSubjectComponent* Subject, UInteractionObjectComponent* Object) {

	auto* PlayerController = Subject->GetPlayerController();
	auto* World = GetWorld();

	if (IsValid(PlayerController)) {
		PlaceableDialogWidget_DEPRECATED = CreateWidget<UDEPRECATED_UIPlaceableDialogWidget>(PlayerController, PlaceableDialogClass_DEPRECATED);
	} else if (World) {
		PlaceableDialogWidget_DEPRECATED = CreateWidget<UDEPRECATED_UIPlaceableDialogWidget>(World, PlaceableDialogClass_DEPRECATED);
	} else {
		LogErrorF("failed to show a placeable form: the player controller and world are both nullptr");
		return;
	}

	if (!PlaceableDialogWidget_DEPRECATED) {
		LogErrorF("failed to create a placeable form widget");
		return;
	}

	PlaceableDialogWidget_DEPRECATED->OnPropertiesChanged.AddWeakLambda(this, [=](const TArray<FVePropertyMetadata>& InProperties) {
		FVePropertyBatchMetadata BatchMetadata;
		BatchMetadata.List = InProperties;
		TArray<uint8> Bytes = BatchMetadata.Serialize();
		Subject->RemoteServer_Event_Binary_BP(Object, FName(TEXT("PlaceableObject_Properties")), Bytes);
	});

	PlaceableDialogWidget_DEPRECATED->SetProperties(PropertiesWidgetClass_DEPRECATED, GetProperties());
	PlaceableDialogWidget_DEPRECATED->AddToViewport();
}

void APlaceableObject::ServerEvent_Implementation(UInteractionSubjectComponent* Subject, UInteractionObjectComponent* Object, const FName& Name, const TArray<uint8>& Args) {
	// Super::ServerEvent_Implementation(Subject, Object, Name, Args);

	if (Name != FName(TEXT("PlaceableObject_Properties"))) {
		return;
	}

	FVePropertyBatchMetadata BatchMetadata;
	BatchMetadata.Deserialize(Args);

	SetProperties(BatchMetadata.List);

	GET_MODULE_SUBSYSTEM(PropertySubsystem, Api, Property);
	if (PropertySubsystem) {
		for (auto& Property : BatchMetadata.List) {
			const auto Callback = MakeShared<FOnGenericRequestCompleted>();
			Callback->BindWeakLambda(this, [=](const bool bInSuccessful, const FString& InError) { });

			if (!PropertySubsystem->Update(SharedPlaceableMetadata.Id, FApiUpdatePropertyMetadata(Property.ToApiMetadata()), Callback)) {
				VeLog("failed update property metadata");
			}
		}
	}
}

TArray<FVePropertyMetadata> APlaceableObject::GetProperties_Implementation() const {
	return {};
}

void APlaceableObject::SetProperties_Implementation(const TArray<FVePropertyMetadata>& Properties) { }
