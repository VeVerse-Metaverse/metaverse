// Authors: Egor A. Pristavka, Nikolay Bulatov. 
// Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "Placeable/PlaceableMesh.h"

#include "ApiPropertySubsystem.h"
#include "PlaceableComponent.h"
#include "VeApi.h"
#include "VeShared.h"
#include "Blueprint/UserWidget.h"
#include "VeGameModule.h"
#include "UI/UIPlaceableMeshPropertiesWidget.h"
// #include "PlaceableComponent.h"
// #include "UI/UIPlaceableMeshPropertiesWidget.h"


// Sets default values
APlaceableMesh::APlaceableMesh() {
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void APlaceableMesh::BeginPlay() {
	Super::BeginPlay();

}

// Called every frame
void APlaceableMesh::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

}

UUIPlaceablePropertiesWidget* APlaceableMesh::GetPlaceablePropertiesWidget_Implementation(APlayerController* PlayerController) {
	if (!IsValid(PlaceablePropertiesClass)) {
		return nullptr;
	}

	auto* PropertiesWidget = CreateWidget<UUIPlaceableMeshPropertiesWidget>(PlayerController, PlaceablePropertiesClass);
	if (!PropertiesWidget) {
		VeLogErrorFunc("Failed to create a properties widget instance");
		return nullptr;
	}

	PropertiesWidget->GetOnSave().BindWeakLambda(this, [=]() {
		GET_MODULE_SUBSYSTEM(PropertySubsystem, Api, Property);
		if (PropertySubsystem) {
			const FGuid& EntityId = GetPlaceableMetadata().EntityId;
			if (EntityId.IsValid()) {
				FApiUpdatePropertyMetadata UpdateMetadata;
				UpdateMetadata.Name = TEXT("mesh");
				UpdateMetadata.Type = FVePropertyMetadata::TypeToString(EVePropertyType::String);
				UpdateMetadata.Value = PropertiesWidget->GetMeshType();

				const auto Callback = MakeShared<FOnGenericRequestCompleted>();
				Callback->BindWeakLambda(this, [=](const bool bInSuccessful, const FString& Error) {
					if (!bInSuccessful) {
						VeLogErrorFunc("failed to update mesh type: %s", *Error);
						return;
					}

					UpdatePlaceableOnServer();
				});

				PropertySubsystem->Update(EntityId, UpdateMetadata, Callback);
			}
		}
	});

	// Widget->SetMetadata(GetPlaceableComponent()->Metadata);

	return PropertiesWidget;
}
