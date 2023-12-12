// Authors: Egor A. Pristavka, Nikolay Bulatov. 
// Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "Components/GltfMeshComponent.h"

#include "VeShared.h"
#include "VeGameModule.h"
#include "Net/UnrealNetwork.h"
#include "glTFRuntimeAsset.h"
#include "VeUI.h"
#include "Subsystems/UINotificationSubsystem.h"
#include "VeDownload.h"
#include "glTFRuntimeFunctionLibrary.h"
#include "PlaceableLib.h"
#include "Async/Async.h"
#include "Inspector/InspectComponent.h"
#include "Components/BoxComponent.h"
#include "FileStorageSubsystem.h"
#include "BaseStorageItem.h"
#include "FileStorageTypes.h"

#define LOCTEXT_NAMESPACE "VeGame"


UGltfMeshComponent::UGltfMeshComponent() {
	PrimaryComponentTick.bCanEverTick = false;
	bWantsInitializeComponent = true;
	SetIsReplicatedByDefault(true);

#if 0
	BoundMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("BoundMesh");
	BoundMeshComponent->SetupAttachment(this);
	BoundMeshComponent->SetRelativeLocation(FVector::ZeroVector);
	BoundMeshComponent->SetCanEverAffectNavigation(false);
	BoundMeshComponent->SetIsReplicated(true);

	GltfSceneComponent = CreateDefaultSubobject<USceneComponent>("GltfScene");
	GltfSceneComponent->SetupAttachment(this);
	GltfSceneComponent->SetRelativeLocation(FVector::ZeroVector);
	GltfSceneComponent->SetCanEverAffectNavigation(false);
	GltfSceneComponent->SetIsReplicated(true);
#endif
}

void UGltfMeshComponent::OnRegister() {
	Super::OnRegister();

#if 1
	BoundMeshComponent = Cast<UStaticMeshComponent>(UPlaceableLib::CreateRuntimeComponent(this, UStaticMeshComponent::StaticClass(), "BoundMesh", EComponentCreationMethod::Instance));
	if (BoundMeshComponent) {
		BoundMeshComponent->bAutoActivate = false;
		BoundMeshComponent->SetMobility(EComponentMobility::Movable);
	}

	GltfSceneComponent = Cast<UStaticMeshComponent>(UPlaceableLib::CreateRuntimeComponent(this, UStaticMeshComponent::StaticClass(), "GltfScene", EComponentCreationMethod::Instance));
	if (GltfSceneComponent) {
		GltfSceneComponent->bAutoActivate = false;
		GltfSceneComponent->SetMobility(EComponentMobility::Movable);
		GltfSceneComponent->SetIsReplicated(true);
	}
#endif
}

void UGltfMeshComponent::OnUnregister() {
	Super::OnUnregister();

#if 1
	UPlaceableLib::DestroyRuntimeComponent(this, BoundMeshComponent);
	UPlaceableLib::DestroyRuntimeComponent(this, GltfSceneComponent);
#endif
}

void UGltfMeshComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UGltfMeshComponent, GltfAssetComponents);
}

void UGltfMeshComponent::InitializeComponent() {
	Super::InitializeComponent();
	BoundMeshComponent->SetStaticMesh(BoundMesh.Get());
}

void UGltfMeshComponent::BeginPlay() {
	Super::BeginPlay();
}

void UGltfMeshComponent::EndPlay(const EEndPlayReason::Type EndPlayReason) {
	Super::EndPlay(EndPlayReason);
	if (Download) {
		Download.Reset();
	}
}

void UGltfMeshComponent::SetAssetUrl(const FString& InUrl) {
	if (Download.IsValid()) {
		return;
	}

	ClearAsset();

	glTFRuntimeAssetUrl.Empty();

	GET_MODULE_SUBSYSTEM(FileStorageSubsystem, Download, FileStorage);
	if (!FileStorageSubsystem) {
		return;
	}

	FStorageItemId StorageItemId(InUrl);
	FBaseStorageItemOptions StorageItemOptions;
	StorageItemOptions.DeleteCache = false;
	StorageItemOptions.DelayForCache = FMath::RandRange(0.5f, 1.0f);

	auto StorageItem = FileStorageSubsystem->GetFileStorageItem(StorageItemId);

	StorageItem->GetOnProgress().AddWeakLambda(this, [=](FBaseStorageItem* InStorageItem, const FFileStorageProgressMetadata& InMetadata) {
		VeLogFunc("Downloading mesh: %.2f", InMetadata.Progress * 100.f);
		OnDownloadingProgress.Broadcast(InMetadata.Progress);
	});

	StorageItem->GetOnComplete().AddWeakLambda(this, [=](FBaseStorageItem* InStorageItem, const FFileStorageResultMetadata& InMetadata) {
		OnDownloadingEnd.Broadcast();

		if (InMetadata.bSuccessful) {
			FglTFRuntimeConfig Config;
			Config.bAllowExternalFiles = true;
			GltfRuntimeAsset = UglTFRuntimeFunctionLibrary::glTFLoadAssetFromFilename(InMetadata.Filename, false, Config);
		}

		if (IsValid(GltfRuntimeAsset)) {
			VeLogFunc("Loading glTF mesh: Success. %s", *InStorageItem->GetStorageItemId().ToString());
		} else {
			VeLogErrorFunc("Loading glTF mesh: Failure. %s", *InMetadata.Error);
		}

		if (!IsRunningDedicatedServer()) {
			GET_MODULE_SUBSYSTEM(NotificationSubsystem, UI, Notification);
			if (NotificationSubsystem) {
				FUINotificationData NotificationData;
				if (IsValid(GltfRuntimeAsset)) {
					// NotificationData.Type = EUINotificationType::Success;
					// NotificationData.Header = LOCTEXT("DownloadglTFHeader", "Loaded glTF mesh");
					// NotificationData.Message = FText::Format(LOCTEXT("DownloadglTFMessage", "Loaded glTF mesh: {0}"), FText::FromString(InStorageItem->GetStorageItemId().GetUrl()));
				} else {
					NotificationData.Type = EUINotificationType::Failure;
					NotificationData.Header = LOCTEXT("DownloadglTFHeaderError", "Error");
					NotificationData.Message = FText::Format(LOCTEXT("DownloadglTFMessageError", "Failed to download the glb: {0}"), FText::FromString(InMetadata.Error));
					NotificationSubsystem->AddNotification(NotificationData);
				}
			}
		}

		glTFRuntimeAssetUrl = InStorageItem->GetStorageItemId().GetUrl();
		DynamicMeshCreate();

		Download.Reset();
	});

	StorageItem->Process(StorageItemOptions);
}

void UGltfMeshComponent::ClearAsset() {
	if (GetOwnerRole() == ROLE_Authority) {
		ClearAsset_Server();
	} else {
		ClearAsset_Client();
	}
}

void UGltfMeshComponent::ClearAsset_Server() {
	GltfAssetComponents.Empty();

	TArray<USceneComponent*> Children;
	GltfSceneComponent->GetChildrenComponents(true, Children);
	for (auto* Component : Children) {
		Component->DestroyComponent();
	}
}

void UGltfMeshComponent::ClearAsset_Client() {
	TArray<USceneComponent*> Children;
	GltfSceneComponent->GetChildrenComponents(true, Children);

	if (GltfAssetComponents_IsReceived) {
		for (auto* Component : Children) {
			if (!GltfAssetComponents_IsExists(Component)) {
				Component->DestroyComponent();
			}
		}
	} else {
		for (auto* Component : Children) {
			Component->DestroyComponent();
		}
	}
}

void UGltfMeshComponent::SetIsVisible(bool IsVisible) {
	bIsVisible = IsVisible;

	const ECollisionEnabled::Type CollisionEnabled = (IsVisible) ? ECollisionEnabled::QueryAndPhysics : ECollisionEnabled::NoCollision;
	const bool HiddenInGame = !IsVisible;

	if (BoundMeshComponent) {
		BoundMeshComponent->SetHiddenInGame(HiddenInGame);
		BoundMeshComponent->SetCollisionEnabled(CollisionEnabled);
	}

	if (GltfSceneComponent) {
		TArray<USceneComponent*> Children;
		GltfSceneComponent->GetChildrenComponents(true, Children);
		for (auto* SceneComponent : Children) {
			SceneComponent->SetHiddenInGame(HiddenInGame);
			if (auto* StaticMeshComponent = Cast<UStaticMeshComponent>(SceneComponent)) {
				StaticMeshComponent->SetCollisionEnabled(CollisionEnabled);
			}
		}
	}
}

void UGltfMeshComponent::ScaleMultiplier(float Value) {
	if (!Value) {
		Value = 1.0f;
	}
	GltfSceneComponent->SetRelativeScale3D(FVector(Value, Value, Value));
}

void UGltfMeshComponent::DynamicMeshCreate() {
	if (!IsValid(GltfRuntimeAsset)) {
		return;
	}

	if (GetNetMode() < ENetMode::NM_Client) {
		DynamicMeshCreate_Server(GltfRuntimeAsset.Get());
		GltfRuntimeAsset = nullptr;
	} else {
		if (GltfAssetComponents_IsReceived) {
			GltfAssetComponents_IsReceived = false;
			DynamicMeshCreate_Client(GltfRuntimeAsset.Get(), GltfAssetComponents);
			GltfRuntimeAsset = nullptr;
		}
	}
}

void UGltfMeshComponent::DynamicMeshCreate_Server(UglTFRuntimeAsset* Asset) {
	FglTFRuntimeStaticMeshConfig MeshConfig;
	MeshConfig.bBuildSimpleCollision = true;
	MeshConfig.MaterialsConfig = FglTFRuntimeMaterialsConfig{};
	MeshConfig.Outer = this;

	const FAttachmentTransformRules AttachmentTransformRules(
		EAttachmentRule::KeepRelative,
		EAttachmentRule::KeepRelative,
		EAttachmentRule::KeepRelative,
		true);

	const auto Nodes = Asset->GetNodes();
	if (Nodes.Num() <= 0) {
		const FString Error = FString::Printf(TEXT("No glTF nodes found in the glb file %s"), *glTFRuntimeAssetUrl);
		VeLogWarningFunc("Loading glTF mesh: Failure. %s", *Error);

		if (!IsRunningDedicatedServer()) {
			GET_MODULE_SUBSYSTEM(NotificationSubsystem, UI, Notification);
			if (NotificationSubsystem) {
				FUINotificationData NotificationData;
				NotificationData.Type = EUINotificationType::Failure;
				NotificationData.Header = LOCTEXT("DownloadglTFHeaderError", "Error");
				NotificationData.Message = FText::Format(LOCTEXT("DownloadglTFMessageNoNodes", "No glTF nodes found in the glb file {0}"), FText::FromString(glTFRuntimeAssetUrl));
				NotificationSubsystem->AddNotification(NotificationData);
			}
		}

		return;
	}

	ensure(GltfAssetComponents.Num() == 0);
	GltfAssetComponents.Empty();

	for (const auto& Node : Nodes) {
		USceneComponent* NodeComponent = nullptr;
		if (Node.MeshIndex < 0) {
			NodeComponent = NewObject<USceneComponent>(GltfSceneComponent.Get(), USceneComponent::StaticClass());
		} else {
			auto* MeshComponent = NewObject<UStaticMeshComponent>(GltfSceneComponent.Get(), UStaticMeshComponent::StaticClass());
			if (IsValid(MeshComponent)) {
				if (auto* Mesh = Asset->LoadStaticMesh(Node.MeshIndex, MeshConfig)) {
					MeshComponent->SetStaticMesh(Mesh);
					if (!IsRunningDedicatedServer()) {
						DynamicMeshCreate_AddCollision(MeshComponent);
					}
				} else {
					VeLogErrorFunc("Failed to load a static mesh");
				}
				NodeComponent = MeshComponent;
			}
		}

		if (IsValid(NodeComponent)) {
			NodeComponent->RegisterComponent();
			NodeComponent->SetCanEverAffectNavigation(false);
			NodeComponent->SetIsReplicated(true);

			NodeComponents.Add(Node.Index, NodeComponent);
			GltfAssetComponents.Emplace(Node.Index, NodeComponent);
		} else {
			VeLogErrorFunc("Failed to create a component");
		}
	}

	for (const auto& Node : Nodes) {
		if (NodeComponents.Contains(Node.Index)) {
			USceneComponent* NodeComponent = NodeComponents[Node.Index];
			if (Node.ParentIndex != -1) {
				if (NodeComponents.Contains(Node.ParentIndex)) {
					USceneComponent* ParentNodeComponent = NodeComponents[Node.ParentIndex];
					if (IsValid(ParentNodeComponent)) {
						NodeComponent->AttachToComponent(ParentNodeComponent, AttachmentTransformRules);
						NodeComponent->SetRelativeTransform(Node.Transform);
					} else {
						VeLogErrorFunc("Invalid parent component");
					}
				} else {
					VeLogErrorFunc("Failed to find a parent component");
				}
			} else {
				NodeComponent->AttachToComponent(GltfSceneComponent.Get(), AttachmentTransformRules);
				NodeComponent->SetRelativeTransform(Node.Transform);
			}
		} else {
			VeLogErrorFunc("Failed to find a component");
		}
	}

	BoundMeshComponent->SetHiddenInGame(true);
	BoundMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void UGltfMeshComponent::DynamicMeshCreate_Client(UglTFRuntimeAsset* Asset, const TArray<FVeGltfMetadata>& InGltfMetadataList) {
	FglTFRuntimeStaticMeshConfig MeshConfig;
	MeshConfig.bBuildSimpleCollision = true;
	MeshConfig.MaterialsConfig = FglTFRuntimeMaterialsConfig{};

	const auto Nodes = Asset->GetNodes();
	if (Nodes.Num() <= 0) {
		const FString Error = FString::Printf(TEXT("No glTF nodes found in the glb file %s"), *glTFRuntimeAssetUrl);
		VeLogWarningFunc("Loading glTF mesh: Failure. %s", *Error);

		if (IsRunningDedicatedServer()) {
			return;
		}

		GET_MODULE_SUBSYSTEM(NotificationSubsystem, UI, Notification);
		if (NotificationSubsystem) {
			FUINotificationData NotificationData;
			NotificationData.Type = EUINotificationType::Failure;
			NotificationData.Header = LOCTEXT("DownloadglTFHeaderError", "Error");
			NotificationData.Message = FText::Format(LOCTEXT("DownloadglTFMessageNoNodes", "No glTF nodes found in the glb file {0}"), FText::FromString(glTFRuntimeAssetUrl));
			NotificationSubsystem->AddNotification(NotificationData);
		}
		return;
	}

	for (const auto& Node : Nodes) {
		auto* VeGltfMetadataItem = FindGltfMetadataItem(InGltfMetadataList, Node.Index);
		if (!VeGltfMetadataItem) {
			continue;
		}

		if (Node.MeshIndex < 0) {
			// NodeComponent = NewObject<USceneComponent>(this, USceneComponent::StaticClass(), FName{Node.Name});
		} else {
			// auto* MeshComponent = NewObject<UStaticMeshComponent>(this, UStaticMeshComponent::StaticClass(), FName{Node.Name});
			auto* MeshComponent = Cast<UStaticMeshComponent>(VeGltfMetadataItem->SceneComponent);
			if (IsValid(MeshComponent)) {
				if (auto* Mesh = Asset->LoadStaticMesh(Node.MeshIndex, MeshConfig)) {
					MeshComponent->SetStaticMesh(Mesh);
					DynamicMeshCreate_AddCollision(MeshComponent);
				} else {
					VeLogErrorFunc("Failed to load a static mesh");
				}
			}
		}
	}

	BoundMeshComponent->SetHiddenInGame(true);
	BoundMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

UBoxComponent* UGltfMeshComponent::DynamicMeshCreate_AddCollision(UStaticMeshComponent* InMeshComponent) {
	auto* CollisionComponent = NewObject<UBoxComponent>(this, UBoxComponent::StaticClass());

	if (CollisionComponent) {
		const FBoxSphereBounds& BoxSphereBounds = InMeshComponent->GetStaticMesh()->GetExtendedBounds();
		CollisionComponent->SetBoxExtent(BoxSphereBounds.GetBox().GetExtent(), true);
		CollisionComponent->SetRelativeLocation(BoxSphereBounds.Origin);

		CollisionComponent->SetCollisionProfileName(UCollisionProfile::CustomCollisionProfileName);
		CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		CollisionComponent->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);
		CollisionComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
		CollisionComponent->SetCollisionResponseToChannel(ECC_InteractionTraceChannel, ECR_Block);

		CollisionComponent->SetupAttachment(InMeshComponent);
		CollisionComponent->RegisterComponent();

		return CollisionComponent;
	}

	return nullptr;
}

void UGltfMeshComponent::OnRep_GltfAssetComponents() {
	if (GltfAssetComponents.IsValidIndex(0) && GltfAssetComponents[0].SceneComponent != nullptr) {
		GltfAssetComponents_IsReceived = true;
		DynamicMeshCreate();
	}
}

const FVeGltfMetadata* UGltfMeshComponent::FindGltfMetadataItem(const TArray<FVeGltfMetadata>& InGltfMetadataList, int32 InNodeIndex) {
	for (auto& Item : InGltfMetadataList) {
		if (Item.NodeIndex == InNodeIndex) {
			return &Item;
		}
	}
	return nullptr;
}

bool UGltfMeshComponent::GltfAssetComponents_IsExists(USceneComponent* Component) {
	for (auto& Item : GltfAssetComponents) {
		if (Item.SceneComponent == Component) {
			return true;
		}
	}
	return false;
}

#undef LOCTEXT_NAMESPACE
