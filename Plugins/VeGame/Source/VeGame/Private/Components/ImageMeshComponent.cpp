// Authors: Egor A. Pristavka, Nikolay Bulatov. 
// Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "Components/ImageMeshComponent.h"

#include "PlaceableLib.h"
#include "VeShared.h"
#include "VeGameModule.h"
#include "Materials/MaterialInstanceDynamic.h"


UImageMeshComponent::UImageMeshComponent() {
	PrimaryComponentTick.bCanEverTick = false;
	bWantsInitializeComponent = true;
	SetIsReplicatedByDefault(true);

#if 0
	FrameMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("FrameMesh");
	FrameMeshComponent->SetupAttachment(this);
	FrameMeshComponent->SetRelativeLocation(FVector::ZeroVector);
	FrameMeshComponent->SetCanEverAffectNavigation(false);
	FrameMeshComponent->SetIsReplicated(true);

	CanvasMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("CanvasMesh");
	CanvasMeshComponent->SetupAttachment(this);
	CanvasMeshComponent->SetRelativeLocation(FVector::ZeroVector);
	CanvasMeshComponent->SetCanEverAffectNavigation(false);
	CanvasMeshComponent->SetIsReplicated(true);
#endif
}

void UImageMeshComponent::OnRegister() {
	Super::OnRegister();

	FrameMeshComponent = Cast<UStaticMeshComponent>(UPlaceableLib::CreateRuntimeComponent(this, UStaticMeshComponent::StaticClass(), "FrameMesh", EComponentCreationMethod::Instance));
	if (FrameMeshComponent) {
		FrameMeshComponent->SetStaticMesh(FrameStaticMesh);
	}
	CanvasMeshComponent = Cast<UStaticMeshComponent>(UPlaceableLib::CreateRuntimeComponent(this, UStaticMeshComponent::StaticClass(), "CanvasMesh", EComponentCreationMethod::Instance));
	if (CanvasMeshComponent) {
		CanvasMeshComponent->SetStaticMesh(CanvasStaticMesh);
	}

	SetSize(DefaultHeight, DefaultWidth);

	if (IsRunningDedicatedServer()) {
		return;
	}

	if (FrameMaterial) {
		FrameMaterialInstance = UMaterialInstanceDynamic::Create(FrameMaterial, this);
		FrameMeshComponent->SetMaterial(0, FrameMaterialInstance);
		FrameMaterialInstance->SetVectorParameterValue(FrameMaterialColorParameterName, FrameColor);
	}

	if (CanvasMaterial) {
		CanvasMaterialInstance = UMaterialInstanceDynamic::Create(CanvasMaterial, this);
		CanvasMeshComponent->SetMaterial(0, CanvasMaterialInstance);
		// CanvasMaterialInstance->SetTextureParameterValue(TEXT("T"), StreamMediaTexture);
	}
}

void UImageMeshComponent::OnUnregister() {
	Super::OnUnregister();

	UPlaceableLib::DestroyRuntimeComponent(this, FrameMeshComponent);
	UPlaceableLib::DestroyRuntimeComponent(this, CanvasMeshComponent);
}

void UImageMeshComponent::InitializeComponent() {
	Super::InitializeComponent();
}

void UImageMeshComponent::BeginPlay() {
	Super::BeginPlay();
}

bool UImageMeshComponent::ShowImage(const FString& InURL, TEnumAsByte<enum TextureGroup> TextureGroup) {
	if (DefaultTexture) {
		SetTexture(DefaultTexture);
	}

	CurrentURL = InURL;

	if (!CurrentURL.IsEmpty()) {
		SetIsLoading(true);
		RequestTexture(this, CurrentURL, TextureGroup);
		return true;
	}

	return false;
}

void UImageMeshComponent::SetSize(float Height, float Width) {
	if (Height <= 0.0f) {
		Height = DefaultHeight;
	}

	if (Width <= 0.0f) {
		Width = DefaultWidth;
	}

	if (Height > 0.0f && Width > 0.0f) {
		if (FrameMeshComponent) {
			FrameMeshComponent->SetRelativeScale3D(FVector(1.0f, Width, Height));
		}
		if (CanvasMeshComponent) {
			CanvasMeshComponent->SetRelativeScale3D(FVector(1.0f, Width - 1.0f, Height - 1.0f));
			CanvasMeshComponent->SetRelativeLocation(FVector(1.001f, 0.0f, 0.0f));
		}
	} else {
		VeLogErrorFunc("Invalid canvas dimensions");
	}
}

void UImageMeshComponent::SetIsVisible(bool IsVisible) {
	bIsVisible = IsVisible;

	const ECollisionEnabled::Type CollisionEnabled = (IsVisible) ? ECollisionEnabled::QueryAndPhysics : ECollisionEnabled::NoCollision;
	const bool HiddenInGame = !IsVisible;

	if (FrameMeshComponent) {
		FrameMeshComponent->SetHiddenInGame(HiddenInGame);
		FrameMeshComponent->SetCollisionEnabled(CollisionEnabled);
	}

	if (CanvasMeshComponent) {
		CanvasMeshComponent->SetHiddenInGame(HiddenInGame);
	}
}

void UImageMeshComponent::SetIsLoading(bool Value) {
	if (Value) {
		OnDownloadingBegin.Broadcast();
	} else {
		OnDownloadingEnd.Broadcast();
	}
}

void UImageMeshComponent::SetTexture(UTexture* Texture) const {
	if (CanvasMaterialInstance) {
		CanvasMaterialInstance->SetTextureParameterValue(CanvasMaterialTextureParameterName, Texture);
		// CanvasMaterialInstance->SetScalarParameterValue(CanvasMaterialEmissivePowerParameterName, EmissivePower);
	}
}

void UImageMeshComponent::NativeOnTextureLoaded(const FTextureLoadResult& InResult) {
	SetIsLoading(false);
	SetTexture(InResult.Texture.Get());
}
