// Author: Egor A. Pristavka. Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#include "VePortal.h"

#include "ImageUtils.h"
#include "VeApi.h"
#include "VeApi2.h"
#include "Api2AuthSubsystem.h"
#include "Api2FileSubsystem.h"
#include "Api2PortalSubsystem.h"
#include "Components/AudioComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InteractiveControlComponent.h"
#include "Engine/Selection.h"
#include "Engine/TextureRenderTargetCube.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Net/UnrealNetwork.h"
#include "Particles/ParticleSystemComponent.h"
#include "Serialization/BufferArchive.h"
#include "UI/UIPortalInfoWidget.h"
#include "VeDownload.h"
#include "VeShared.h"
#include "VeGameModule.h"
#include "Components/InteractionObjectComponent.h"
#include "VeGameFramework.h"
#include "UI/UIPortalPropertiesWidget.h"
#include "PlaceableComponent.h"
#include "PlaceableLib.h"
#include "EditorComponent.h"
#include "GameFrameworkEditorSubsystem.h"
#include "GameFrameworkWorldSubsystem.h"
#include "GameFrameworkWorldItem.h"
#include "VePlayerController.h"
#include "Components/SceneCaptureComponent2D.h"
#include "VeWorldSettings.h"


void AVePortal::NativeOnTextureLoaded(const FTextureLoadResult& InResult) {
	if (!InResult.bSuccessful || !InResult.Texture.IsValid()) {
		return;
	}

	if (UTexture2D* Texture2D = InResult.AsTexture2D()) {
		UMaterialInterface* PreviewMaterialInterface = PreviewFrontSideComponent->GetMaterial(0);
		if (UMaterialInstanceDynamic* PreviewMID = Cast<UMaterialInstanceDynamic>(PreviewMaterialInterface)) {
			// FStaticParameterSet StaticParameters = PreviewMID->GetStaticParameters();
			// for (auto& SwitchParameter : StaticParameters.StaticSwitchParameters) {
			// 	if (SwitchParameter.ParameterInfo.Name == "bUseCubeMap") {
			// 		SwitchParameter.Value = true;
			// 		break;
			// 	}
			// }
			//PreviewMID->UpdateStaticPermutation(StaticParameters);
			PreviewMID->SetTextureParameterValue("Texture1", Texture2D);
		}
	}

}

// Sets default values
AVePortal::AVePortal(const FObjectInitializer& ObjectInitializer)
	: APlayerStart(ObjectInitializer) {
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
	// SetReplicates(true);
	SetReplicatingMovement(true);


	bAlwaysRelevant = true;

	InteractiveTarget = CreateDefaultSubobject<UInteractiveTargetComponent>("InteractiveTarget");
	InteractionObjectComponent = CreateDefaultSubobject<UInteractionObjectComponent>("Interaction Object");

	// Set capsule mobility to movable so that it won't affect world lighting or navigation
	if (UCapsuleComponent* Capsule = GetCapsuleComponent()) {
		Capsule->SetMobility(EComponentMobility::Movable);
	}

	PortalRootComponent = CreateDefaultSubobject<USceneComponent>("PortalRoot");
	PortalRootComponent->SetupAttachment(RootComponent);
	PortalRootComponent->SetRelativeLocationAndRotation(FVector(-300.0f, 0, 135.0f), FRotator(0.0f, -90.0f, 0.0f));

	PortalScaleComponent = CreateDefaultSubobject<USceneComponent>("PortalScale");
	PortalScaleComponent->SetupAttachment(PortalRootComponent);
	PortalScaleComponent->SetRelativeScale3D(FVector(0.005f, 0.005f, 0.005f));

#pragma region Triggers
	OuterTriggerComponent = CreateDefaultSubobject<USphereComponent>("OuterTrigger");
	OuterTriggerComponent->SetupAttachment(PortalRootComponent);
	OuterTriggerComponent->SetSphereRadius(300.0f);
	OuterTriggerComponent->SetRelativeLocationAndRotation(FVector::ZeroVector, FRotator::ZeroRotator);
	OuterTriggerComponent->SetRelativeScale3D(FVector::OneVector);

	InnerTriggerComponent = CreateDefaultSubobject<UBoxComponent>("InnerTrigger");
	InnerTriggerComponent->SetupAttachment(PortalRootComponent);
	InnerTriggerComponent->SetBoxExtent(FVector(150.0f, 30.0f, 150.0f));
	InnerTriggerComponent->SetRelativeLocationAndRotation(FVector::ZeroVector, FRotator::ZeroRotator);
	InnerTriggerComponent->SetRelativeScale3D(FVector::OneVector);
#pragma endregion

#pragma region Preview
	PreviewFrontSideComponent = CreateDefaultSubobject<UStaticMeshComponent>("PreviewFrontSide");
	PreviewFrontSideComponent->SetupAttachment(PortalScaleComponent);
	PreviewFrontSideComponent->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, 0.0f), FRotator(0.0f, 0.0f, 90.0f));
	PreviewFrontSideComponent->SetRelativeScale3D(FVector(350.0f, 350.0f, 50.0f));

	PreviewBackSideComponent = CreateDefaultSubobject<UStaticMeshComponent>("PreviewBackSide");
	PreviewBackSideComponent->SetupAttachment(PortalScaleComponent);
	PreviewBackSideComponent->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, 0.0f), FRotator(0.0f, 0.0f, -90.0f));
	PreviewBackSideComponent->SetRelativeScale3D(FVector(-350.0f, -350.0f, 50.0f));
#pragma endregion

#pragma region Rings
	TopInnerRingComponent = CreateDefaultSubobject<UStaticMeshComponent>("TopInnerRing");
	TopInnerRingComponent->SetupAttachment(PortalScaleComponent);
	TopInnerRingComponent->SetRelativeLocationAndRotation(FVector(0.0f, 200.0f, 0.0f), FRotator(0.0f, 0.0f, 90.0f));
	TopInnerRingComponent->SetRelativeScale3D(FVector(950.0f, 950.0f, 950.0f));

	TopMiddleRingComponent = CreateDefaultSubobject<UStaticMeshComponent>("TopMiddleRing");
	TopMiddleRingComponent->SetupAttachment(PortalScaleComponent);
	TopMiddleRingComponent->SetRelativeLocationAndRotation(FVector(0.0f, 100.0f, 0.0f), FRotator(0.0f, 0.0f, 90.0f));
	TopMiddleRingComponent->SetRelativeScale3D(FVector(1000.0f, 1000.0f, 1000.0f));

	TopOuterRingComponent = CreateDefaultSubobject<UStaticMeshComponent>("TopOuterRing");
	TopOuterRingComponent->SetupAttachment(PortalScaleComponent);
	TopOuterRingComponent->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, 0.0f), FRotator(0.0f, 0.0f, 90.0f));
	TopOuterRingComponent->SetRelativeScale3D(FVector(1050.0f, 1050.0f, 1050.0f));

	BottomInnerRingComponent = CreateDefaultSubobject<UStaticMeshComponent>("BottomInnerRing");
	BottomInnerRingComponent->SetupAttachment(PortalScaleComponent);
	BottomInnerRingComponent->SetRelativeLocationAndRotation(FVector(0.0f, 250.0f, 0.0f), FRotator(180.0f, 0.0f, 90.0f));
	BottomInnerRingComponent->SetRelativeScale3D(FVector(950.0f, 950.0f, 950.0f));

	BottomMiddleRingComponent = CreateDefaultSubobject<UStaticMeshComponent>("BottomMiddleRing");
	BottomMiddleRingComponent->SetupAttachment(PortalScaleComponent);
	BottomMiddleRingComponent->SetRelativeLocationAndRotation(FVector(0.0f, 150.0f, 0.0f), FRotator(180.0f, 0.0f, 90.0f));
	BottomMiddleRingComponent->SetRelativeScale3D(FVector(1000.0f, 1000.0f, 1000.0f));

	BottomOuterRingComponent = CreateDefaultSubobject<UStaticMeshComponent>("BottomOuterRing");
	BottomOuterRingComponent->SetupAttachment(PortalScaleComponent);
	BottomOuterRingComponent->SetRelativeLocationAndRotation(FVector(0.0f, 50.0f, 0.0f), FRotator(180.0f, 0.0f, 90.0f));
	BottomOuterRingComponent->SetRelativeScale3D(FVector(1050.0f, 1050.0f, 1050.0f));
#pragma endregion

#pragma region Audio
	BackgroundAudioComponent = CreateDefaultSubobject<UAudioComponent>("BackgroundAudio");
	BackgroundAudioComponent->SetupAttachment(PortalRootComponent);
	BackgroundAudioComponent->SetRelativeLocationAndRotation(FVector::ZeroVector, FRotator::ZeroRotator);
	BackgroundAudioComponent->SetRelativeScale3D(FVector::OneVector);

	StatusAudioComponent = CreateDefaultSubobject<UAudioComponent>("StatusAudio");
	StatusAudioComponent->SetupAttachment(PortalRootComponent);
	StatusAudioComponent->SetRelativeLocationAndRotation(FVector::ZeroVector, FRotator::ZeroRotator);
	StatusAudioComponent->SetRelativeScale3D(FVector::OneVector);

	LoadingStatusAudioComponent = CreateDefaultSubobject<UAudioComponent>("LoadingStatusAudio");
	LoadingStatusAudioComponent->SetupAttachment(PortalRootComponent);
	LoadingStatusAudioComponent->SetRelativeLocationAndRotation(FVector::ZeroVector, FRotator::ZeroRotator);
	LoadingStatusAudioComponent->SetRelativeScale3D(FVector::OneVector);
#pragma endregion

#pragma region Status Text
	WidgetComponent = CreateDefaultSubobject<UWidgetComponent>("Widget");
	WidgetComponent->SetupAttachment(PortalRootComponent);
	WidgetComponent->SetRelativeLocationAndRotation(FVector::ZeroVector, FRotator(0.0f, 90.0f, 0.0f));
	WidgetComponent->SetRelativeScale3D(FVector::OneVector);
	WidgetComponent->SetWidgetClass(WidgetClass);
	WidgetComponent->SetWidgetSpace(EWidgetSpace::World);
#pragma endregion

#pragma region Effects
	TrackComponent = CreateDefaultSubobject<UStaticMeshComponent>("Track");
	TrackComponent->SetupAttachment(PortalRootComponent);
	TrackComponent->SetRelativeLocationAndRotation(FVector(0, 200.0f, -230.0f), FRotator(0.0f, -90.0f, 0.0f));
	TrackComponent->SetRelativeScale3D(FVector(3.0f, 1.5f, 1.0f));

	BackgroundParticleSystemComponent = CreateDefaultSubobject<UParticleSystemComponent>("BackgroundParticleSystem");
	BackgroundParticleSystemComponent->SetupAttachment(PortalRootComponent);
	BackgroundParticleSystemComponent->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -140.0f), FRotator::ZeroRotator);
	BackgroundParticleSystemComponent->SetRelativeScale3D(FVector::OneVector);
#pragma endregion

	PlaceableComponent = CreateDefaultSubobject<UPlaceableComponent>(TEXT("PlaceableComponent"));
}

// void AVePortal::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
// 	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
//
// 	// DOREPLIFETIME(AVePortal, Id);
// 	// DOREPLIFETIME(AVePortal, ReplicatedMetadata);
// 	// DOREPLIFETIME(AVePortal, ReplicatedServerMetadata);
// }

void AVePortal::OnConstruction(const FTransform& Transform) {
	Super::OnConstruction(Transform);

	if (IsRunningDedicatedServer()) {
		return;
	}

	if (WidgetComponent) {
		if (const UUIPortalInfoWidget* UIPortalInfoWidget = Cast<UUIPortalInfoWidget>(WidgetComponent->GetWidget())) {
			UIPortalInfoWidget->UpdateNameColor(NameTextColor);
		}
	}

	if (!Cast<UMaterialInstanceDynamic>(TopInnerRingMaterial)) {
		TopInnerRingMID = TopInnerRingComponent->CreateDynamicMaterialInstance(0, TopInnerRingMaterial);
	}

	if (TopInnerRingMID) {
		TopInnerRingMID->SetVectorParameterValue(TEXT("C"), TopInnerRingColor);
		TopInnerRingMID->SetScalarParameterValue(TEXT("L"), RingBrightness);
	}

	if (!Cast<UMaterialInstanceDynamic>(TopMiddleRingMaterial)) {
		TopMiddleRingMID = TopMiddleRingComponent->CreateDynamicMaterialInstance(0, TopMiddleRingMaterial);
	}

	if (TopMiddleRingMID) {
		TopMiddleRingMID->SetVectorParameterValue(TEXT("C"), TopMiddleRingColor);
		TopMiddleRingMID->SetScalarParameterValue(TEXT("L"), RingBrightness);
	}

	if (!Cast<UMaterialInstanceDynamic>(TopOuterRingMaterial)) {
		TopOuterRingMID = TopOuterRingComponent->CreateDynamicMaterialInstance(0, TopOuterRingMaterial);
	}

	if (TopOuterRingMID) {
		TopOuterRingMID->SetVectorParameterValue(TEXT("C"), TopOuterRingColor);
		TopOuterRingMID->SetScalarParameterValue(TEXT("L"), RingBrightness);
	}

	if (!Cast<UMaterialInstanceDynamic>(BottomInnerRingMaterial)) {
		BottomInnerRingMID = BottomInnerRingComponent->CreateDynamicMaterialInstance(0, BottomInnerRingMaterial);
	}

	if (BottomInnerRingMID) {
		BottomInnerRingMID->SetVectorParameterValue(TEXT("C"), BottomInnerRingColor);
		BottomInnerRingMID->SetScalarParameterValue(TEXT("L"), RingBrightness);
	}

	if (!Cast<UMaterialInstanceDynamic>(BottomMiddleRingMaterial)) {
		BottomMiddleRingMID = BottomMiddleRingComponent->CreateDynamicMaterialInstance(0, BottomMiddleRingMaterial);
	}

	if (BottomMiddleRingMID) {
		BottomMiddleRingMID->SetVectorParameterValue(TEXT("C"), BottomMiddleRingColor);
		BottomMiddleRingMID->SetScalarParameterValue(TEXT("L"), RingBrightness);
	}

	if (!Cast<UMaterialInstanceDynamic>(BottomOuterRingMaterial)) {
		BottomOuterRingMID = BottomOuterRingComponent->CreateDynamicMaterialInstance(0, BottomOuterRingMaterial);
	}

	if (BottomOuterRingMID) {
		BottomOuterRingMID->SetVectorParameterValue(TEXT("C"), BottomOuterRingColor);
		BottomOuterRingMID->SetScalarParameterValue(TEXT("L"), RingBrightness);
	}
}

void AVePortal::PostInitializeComponents() {
	Super::PostInitializeComponents();

	if (IsRunningDedicatedServer()) {
		return;
	}

	InnerTriggerComponent->OnComponentBeginOverlap.AddDynamic(this, &AVePortal::OnInnerTriggerBeginOverlap);
	InnerTriggerComponent->OnComponentEndOverlap.AddDynamic(this, &AVePortal::OnInnerTriggerEndOverlap);

	OuterTriggerComponent->OnComponentBeginOverlap.AddDynamic(this, &AVePortal::OnOuterTriggerBeginOverlap);
	OuterTriggerComponent->OnComponentEndOverlap.AddDynamic(this, &AVePortal::OnOuterTriggerEndOverlap);
}

// Called every frame
void AVePortal::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}

// Called when the game starts or when spawned
void AVePortal::BeginPlay() {
	Super::BeginPlay();

	if (IsRunningDedicatedServer()) {
		return;
	}

	if (UUIPortalInfoWidget* PortalInfoWidget = Cast<UUIPortalInfoWidget>(WidgetComponent->GetWidget())) {
		PortalInfoWidget->GetOnDownloadClicked().AddUObject(this, &AVePortal::PortalInfoWidget_OnDownloadClicked);
	}

	PlaceableComponent->GetOnPlaceableMetadataChanged().AddWeakLambda(this, [=](const FVePlaceableMetadata& InMetadata, EPlaceableChangeType InChange) {
		UpdatePortalMetadata();
	});

	GET_MODULE_SUBSYSTEM(AuthSubsystem, Api2, Auth);
	if (AuthSubsystem) {
		auto OnChangeAuthorizationStateCallback = [=](EAuthorizationState AuthorizationState) {
			if (AuthSubsystem->IsAuthorized()) {
				UpdatePortalMetadata();
			}
		};

		AuthSubsystem->GetOnChangeAuthorizationState().AddWeakLambda(this, OnChangeAuthorizationStateCallback);
		OnChangeAuthorizationStateCallback(AuthSubsystem->GetAuthorizationState());
	}
}

void AVePortal::EndPlay(const EEndPlayReason::Type EndPlayReason) {
	Super::EndPlay(EndPlayReason);

	GET_MODULE_SUBSYSTEM(AuthSubsystem, Api2, Auth);
	if (AuthSubsystem) {
		AuthSubsystem->GetOnChangeAuthorizationState().RemoveAll(this);
	}
}

// void AVePortal::OnRep_PortalId() {
// 	auto TempId = Id;
// 	RequestPortalMetadata();
// }

void AVePortal::PortalInfoWidget_OnDownloadClicked() {
	if (!WorldItem.IsValid()) {
		return;
	}
	auto WorldItemPtr = WorldItem.Pin();

	const auto WorldState = WorldItemPtr->GetState();
	if (WorldState == EVeWorldState::PendingDownloadPackage) {
		StartByStatus_DownloadPackage(WorldItemPtr);
	}
}

UUIPlaceablePropertiesWidget* AVePortal::GetPlaceablePropertiesWidget_Implementation(APlayerController* PlayerController) {
	if (!IsValid(PlaceablePropertiesClass)) {
		return nullptr;
	}

	auto* PropertiesWidget = CreateWidget<UUIPortalPropertiesWidget>(PlayerController, PlaceablePropertiesClass);
	if (!PropertiesWidget) {
		VeLogErrorFunc("Failed to create a properties widget instance");
		return nullptr;
	}

	PropertiesWidget->SetMetadata(FVePortalMetadata(ApiPortalMetadata));

	PropertiesWidget->RefreshRenderTargetButtonClicked.BindWeakLambda(this, [=]() {
		if (ApiPortalMetadata.Id.IsValid()) {
			OnRefreshRenderTargetPreviewButtonClicked(ApiPortalMetadata.Id);
		}
	});

	PropertiesWidget->GetOnSave().BindWeakLambda(this, [=]() {
		FApiUpdatePortalMetadata UpdateMetadata;
		UpdateMetadata.Name = PropertiesWidget->GetPortalName();
		UpdateMetadata.DestinationId = PropertiesWidget->GetPortalDestinationId();
		UpdateMetadata.bPublic = true;

		if (GetId().IsValid()) {
			Properties_OnSave_Update(PropertiesWidget, UpdateMetadata);
		} else {
			Properties_OnSave_Create(PropertiesWidget, UpdateMetadata);
		}
	});

	return PropertiesWidget;
}

void AVePortal::Properties_OnSave_Create(UUIPlaceablePropertiesWidget* PropertiesWidget, const FApiUpdatePortalMetadata& InUpdateMetadata) {
	GET_MODULE_SUBSYSTEM(PortalSubsystem, Api2, Portal);
	if (!PortalSubsystem) {
		return;
	}

	FApiUpdatePortalMetadata UpdateMetadata = InUpdateMetadata;

	if (const auto* VeWorldSettings = Cast<AVeWorldSettings>(GetWorldSettings())) {
		UpdateMetadata.WorldId = VeWorldSettings->GetWorldId();
	}

	const auto PortalCallback = MakeShared<FOnPortalRequestCompleted2>();
	PortalCallback->BindWeakLambda(this, [=](const FApiPortalMetadata InPortalMetadata, const EApi2ResponseCode InResponseCode, const FString& Error) {
		if (InResponseCode != EApi2ResponseCode::Ok) {
			SetPlaceableState(EPlaceableState::None);
			VeLogErrorFunc("Failed to create a portal: %s", *Error);
			return;
		}

		// Id = InPortalMetadata.Id;

		GET_MODULE_SUBSYSTEM(PlaceableSubsystem, Api, Placeable);
		if (PlaceableSubsystem) {
			const auto PlaceableCallback = MakeShared<FOnPlaceableRequestCompleted>();
			PlaceableCallback->BindLambda([=](const FApiPlaceableMetadata& InPlaceableMetadata, const EApiResponseCode InResponseCode, const FString& InError) {
				SetPlaceableState(EPlaceableState::None);

				if (InResponseCode != EApiResponseCode::Ok) {
					VeLogErrorFunc("Failed to update placeable entity: %s", *InError);
					return;
				}

				UpdatePlaceableOnServer();
			});

			// Set EntityId for Placeable
			const auto& PlaceableId = GetPlaceableMetadata().Id;
			const auto& EntityId = InPortalMetadata.Id;
			PlaceableSubsystem->UpdatePlaceableEntity(PlaceableId, EntityId, PlaceableCallback);
		}
	});

	// Create Portal
	SetPlaceableState(EPlaceableState::Uploading);
	PortalSubsystem->Create(UpdateMetadata, PortalCallback);
}

void AVePortal::Properties_OnSave_Update(UUIPlaceablePropertiesWidget* PropertiesWidget, const FApiUpdatePortalMetadata& InUpdateMetadata) {
	GET_MODULE_SUBSYSTEM(PortalSubsystem, Api, Portal);
	if (PortalSubsystem) {
		FApiUpdatePortalMetadata UpdateMetadata = InUpdateMetadata;
		UpdateMetadata.Id = GetId();

		const auto PortalCallback = MakeShared<FOnPortalRequestCompleted>();
		PortalCallback->BindWeakLambda(this, [=](const FApiPortalMetadata InPortalMetadata, const bool bInSuccessful, const FString& Error) {
			SetPlaceableState(EPlaceableState::None);

			if (!bInSuccessful) {
				VeLogErrorFunc("failed to update a portal: %s", *Error);
				return;
			}

			UpdatePlaceableOnServer();
		});

		// Update Portal
		SetPlaceableState(EPlaceableState::Uploading);
		PortalSubsystem->Update(UpdateMetadata, PortalCallback);
	}
}

void AVePortal::UpdatePlaceableOnServer() {
	GET_MODULE_SUBSYSTEM(EditorSubsystem, GameFramework, Editor);
	if (EditorSubsystem) {
		if (auto* EditorComponent = EditorSubsystem->GetEditorComponent(); IsValid(EditorComponent)) {
			EditorComponent->Owner_UpdatePlaceables({this});
		}
	}
}

// void AVePortal::RequestDownloadConfirmation() {
// 	if (!WorldItem.IsValid()) {
// 		return;
// 	}
// 	auto WorldItemPtr = WorldItem.Pin();
//
// 	const int64 DownloadSize = WorldItemPtr->GetPackageSize();
// 	if (DownloadSize > 0) {
// 		UUIPortalInfoWidget* PortalInfoWidget = Cast<UUIPortalInfoWidget>(WidgetComponent->GetWidget());
// 		if (IsValid(PortalInfoWidget)) {
// 			PortalInfoWidget->UpdateDownloadSize(DownloadSize);
// 		}
// 	}
// }

void AVePortal::RequestPreviewCubemap() {

	const TArray<FApiFileMetadata>::ElementType* FileMetadata = ApiPortalMetadata.Destination.Files.FindByPredicate([=](const FApiFileMetadata InFileMetadata) {
		return InFileMetadata.Type == EApiFileType::RenderTargetPreview;
	});

	if (IsRunningDedicatedServer()) {
		return;
	}

	if (FileMetadata) {
		RequestTexture(this, FileMetadata->Url, TEXTUREGROUP_World);
	}

	// const TArray<FApiFileMetadata>::ElementType* FileMetadata = PortalMetadata.Destination.Files.FindByPredicate([=](const FApiFileMetadata InFileMetadata) {
	// 	return InFileMetadata.Type == EApiFileType::Cubemap;
	// });
	//
	// if (IsRunningDedicatedServer()) {
	// 	return;
	// }
	//
	// if (FileMetadata) {
	// 	RequestTexture(FileMetadata->Url, TEXTUREGROUP_World);
	// }

}

void AVePortal::OnOuterTriggerBeginOverlap(UPrimitiveComponent* /*OverlappedComponent*/, AActor* OtherActor, UPrimitiveComponent* /*OtherComp*/, int32 /*OtherBodyIndex*/, bool /*bFromSweep*/, const FHitResult& /*SweepResult*/) {
	if (IsRunningDedicatedServer()) {
		return;
	}

	bPawnOuterOverlap = true;

	if (auto* Pawn = Cast<APawn>(OtherActor)) {
		PawnToTeleport = Pawn;
		StartByStatus();
	}
}

void AVePortal::OnInnerTriggerBeginOverlap(UPrimitiveComponent* /*OverlappedComponent*/, AActor* OtherActor, UPrimitiveComponent* /*OtherComp*/, int32 /*OtherBodyIndex*/, bool /*bFromSweep*/, const FHitResult& /*SweepResult*/) {
	if (IsRunningDedicatedServer()) {
		return;
	}

	bPawnInnerOverlap = true;
	StartByStatus();
}

void AVePortal::OnOuterTriggerEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) {
	bPawnOuterOverlap = false;
	PawnToTeleport = nullptr;
}

void AVePortal::OnInnerTriggerEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) {
	bPawnInnerOverlap = false;
}

void AVePortal::OnCubeMapRenderPreview(const FGuid& InPortalId) {

	UTextureRenderTargetCube* CubemapPreviewRenderTarget = GetCubemapPreviewTarget();
	if (CubemapPreviewRenderTarget) {
		TArray<uint8> OutJpegBytes;
		FBufferArchive Buffer;
		if (FImageUtils::ExportRenderTargetCubeAsHDR(CubemapPreviewRenderTarget, Buffer)) {
			OutJpegBytes = static_cast<TArray<uint8>>(MoveTemp(Buffer));
			//}

			//if (UVeImageFunctionLibrary::ExportCubeRenderTarget2DAsJPEGRaw(CubemapPreviewRenderTarget, OutJpegBytes)) {
			GET_MODULE_SUBSYSTEM(FileUploadSubsystem, Api2, File);
			if (FileUploadSubsystem) {
				FApi2FileUploadMemoryRequestMetadata UploadMetadata;
				UploadMetadata.Id = InPortalId;
				UploadMetadata.FileBytes = OutJpegBytes;
				UploadMetadata.FileName = FString::Printf(TEXT("portal_cubemap_%s.hdr"), *InPortalId.ToString(EGuidFormats::Digits));
				UploadMetadata.FileType = Api::FileTypes::Cubemap;
				UploadMetadata.MimeType = Api::ContentTypes::ImageExr;

				const auto InCallback = MakeShared<FOnFileRequestCompleted>();
				InCallback->BindWeakLambda(this, [InCallback](const FApiFileMetadata& InMetadata, const bool bInSuccessful, const FString& InError) {
					if (!bInSuccessful) {
						VeLogErrorFunc("Failed to upload persona preview: %s", *InError);
						return;
					}
				});

				FileUploadSubsystem->UploadMemory(UploadMetadata, InCallback);
			}
		}
	}
}

void AVePortal::OnRefreshRenderTargetPreviewButtonClicked(const FGuid& InPortalId) {

	UTextureRenderTarget2D* RenderTargetPreview = GetRenderTargetPreview();
	if (RenderTargetPreview) {
		TArray<uint8> OutJpegBytes;
		FBufferArchive Buffer;
		if (FImageUtils::ExportRenderTarget2DAsPNG(RenderTargetPreview, Buffer)) {
			OutJpegBytes = static_cast<TArray<uint8>>(MoveTemp(Buffer));
			//}

			//if (UVeImageFunctionLibrary::ExportRenderTarget2DAsJPEGRaw(RenderTargetPreview, OutJpegBytes)) {
			GET_MODULE_SUBSYSTEM(FileSubsystem, Api2, File);
			if (FileSubsystem) {
				FApi2FileUploadMemoryRequestMetadata UploadMetadata;
				UploadMetadata.Id = InPortalId;
				UploadMetadata.FileBytes = OutJpegBytes;
				UploadMetadata.FileName = FString::Printf(TEXT("portal_rendertarget_%s"), *InPortalId.ToString(EGuidFormats::Digits));
				UploadMetadata.FileType = Api::FileTypes::RenderTargetPreview;
				UploadMetadata.MimeType = Api::ContentTypes::ImagePng;

				TSharedRef<FOnFileRequestCompleted> InCallback = MakeShareable(new FOnFileRequestCompleted{});
				InCallback->BindWeakLambda(this, [InCallback](const FApiFileMetadata& InMetadata, const bool bInSuccessful, const FString& InError) mutable {
					if (!bInSuccessful) {
						VeLogErrorFunc("failed to upload portal preview: %s", *InError);
						return;
					}
				});

				FileSubsystem->UploadMemory(UploadMetadata, InCallback);
			}
		}
	}
}

void AVePortal::SetWorldItem(TSharedPtr<FGameFrameworkWorldItem> InWorldItem) {
	if (WorldItem == InWorldItem) {
		return;
	}

	if (WorldItem.IsValid()) {
		auto WorldItemRef = WorldItem.Pin();
		WorldItemRef->GetOnStateChanged().RemoveAll(this);
	}

	WorldItem = InWorldItem;

	if (InWorldItem.IsValid()) {
		InWorldItem->GetOnStateChanged().AddUObject(this, &AVePortal::WorldItem_OnStateChanged);
		WorldItem_OnStateChanged(*InWorldItem);
	}
}

void AVePortal::WorldItem_OnStateChanged(const FGameFrameworkWorldItem& InWorldItem) {
	if (!WorldItem.IsValid()) {
		return;
	}
	auto WorldItemPtr = WorldItem.Pin();

	auto* UIPortalInfoWidget = Cast<UUIPortalInfoWidget>(WidgetComponent->GetWidget());
	if (!UIPortalInfoWidget) {
		return;
	}

	const auto WorldState = WorldItemPtr->GetState();

	// Widgets
	switch (WorldState) {
	case EVeWorldState::PendingDownloadPackage:
		UIPortalInfoWidget->ShowDownloadButton();
		UIPortalInfoWidget->HideLoadingProgress();
		break;
	case EVeWorldState::PackageDownloading:
		UIPortalInfoWidget->HideDownloadButton();
		UIPortalInfoWidget->ShowLoadingProgress();
		break;
	default:
		UIPortalInfoWidget->HideDownloadButton();
		UIPortalInfoWidget->HideLoadingProgress();
		break;
	}

	switch (WorldState) {
	case EVeWorldState::Error: {
		SetPortalState(EVePortalState::Error);
		break;
	}

	case EVeWorldState::Error_Unavailable: {
		SetPortalState(EVePortalState::NoPak);
		break;
	}

	case EVeWorldState::Error_DownloadFail: {
		SetPortalState(EVePortalState::ConfirmDownload);
		break;
	}

	case EVeWorldState::Error_MountFail: {
		SetPortalState(EVePortalState::Error);
		break;
	}

	case EVeWorldState::PendingDownloadPackage: {
		UIPortalInfoWidget->UpdateDownloadSize(WorldItemPtr->GetPackageSize());
		SetPortalState(EVePortalState::ConfirmDownload);
		break;
	}

	case EVeWorldState::PackageDownloading: {
		SetPortalState(EVePortalState::Downloading);

		const float Ratio = WorldItemPtr->GetStateRatio();
		const UUIPortalInfoWidget* PortalInfoWidget = Cast<UUIPortalInfoWidget>(WidgetComponent->GetWidget());
		if (IsValid(PortalInfoWidget)) {
			PortalInfoWidget->UpdateLoadingProgress(Ratio);
		}

		OnDownloadStateChanged_BP(Ratio);
		break;
	}

	case EVeWorldState::PackageDownloaded: {

		// MountPackage
		StartByStatus_MountPackage(WorldItemPtr);
		break;
	}

	case EVeWorldState::PackageMounted: {
		SetPortalState(EVePortalState::Offline);

		// Matching server
		StartByStatus_MatchingServer(WorldItemPtr);
		break;
	}

	case EVeWorldState::ServerMatching: {
		SetPortalState(EVePortalState::WaitServer);
		break;
	}

	case EVeWorldState::ServerOnline: {
		SetPortalState(EVePortalState::Online);

		// Open world
		StartByStatus_TeleportOnline(WorldItemPtr);
		break;
	}

	// case EVeWorldState::Teleporting:

	default: {
		SetPortalState(EVePortalState::Loading);
		break;
	}
	}

}

void AVePortal::StartByStatus() {
	if (!WorldItem.IsValid()) {
		return;
	}
	auto WorldItemPtr = WorldItem.Pin();

	const auto WorldState = WorldItemPtr->GetState();
	switch (WorldState) {
	case EVeWorldState::PackageMounted:
		StartByStatus_MatchingServer(WorldItemPtr);
		break;

	case EVeWorldState::ServerOnline:
		StartByStatus_TeleportOnline(WorldItemPtr);
		break;

	default:
		break;
	}
}

void AVePortal::StartByStatus_DownloadPackage(const TSharedPtr<FGameFrameworkWorldItem>& WorldItemPtr) {
	if (!bPawnOuterOverlap) {
		return;
	}

	const auto Callback = MakeShared<FOnPackageState>();
	Callback->BindWeakLambda(this, [=](EVePackageState InState) {
		if (InState == EVePackageState::Mounted) {
			StartByStatus_MatchingServer(WorldItemPtr);
		}
	});

	WorldItemPtr->DownloadPackage(true, Callback);
}

void AVePortal::StartByStatus_MountPackage(const TSharedPtr<FGameFrameworkWorldItem>& WorldItemPtr) {
	const auto Callback = MakeShared<FOnPackageState>();
	Callback->BindLambda([=](EVePackageState InState) { });

	WorldItemPtr->MountPackage(Callback);
}

void AVePortal::StartByStatus_MatchingServer(const TSharedPtr<FGameFrameworkWorldItem>& WorldItemPtr) {
	if (!bPawnOuterOverlap) {
		return;
	}

	const auto Callback = MakeShared<FOnMatchingServerState>();
	Callback->BindWeakLambda(this, [=](EVeMatchingServerState InState) {
		if (InState == EVeMatchingServerState::Online) {
			StartByStatus_TeleportOnline(WorldItemPtr);
		}
	});

	WorldItemPtr->MatchingServer(Callback);
	// if (AController* Controller = PawnToTeleport->GetController()) {
	// 	if (auto* PlayerController = Cast<APlayerController>(Controller)) {
	// 		WorldItemPtr->MatchingServer(Callback, PlayerController);
	// 	}
	// }
}

void AVePortal::StartByStatus_TeleportOnline(const TSharedPtr<FGameFrameworkWorldItem>& WorldItemPtr) {
	if (!bPawnInnerOverlap) {
		return;
	}

	if (!PawnToTeleport.IsValid()) {
		return;
	}

	const auto Callback = MakeShared<FOnOpenWorldState>();
	Callback->BindWeakLambda(this, [=](bool InState) {
		if (InState) {
			OnTeleportRequested_BP(PawnToTeleport.Get());
		}
	});

	if (AController* Controller = PawnToTeleport->GetController()) {
		if (auto* PlayerController = Cast<APlayerController>(Controller)) {
			WorldItemPtr->TeleportOnline(Callback, PlayerController, ApiPortalMetadata.Destination.Id);
		}
	}
}

void AVePortal::SetPortalState(const EVePortalState InState) {
	if (WidgetComponent) {
		if (const UUIPortalInfoWidget* UIPortalInfoWidget = Cast<UUIPortalInfoWidget>(WidgetComponent->GetWidget())) {
			UIPortalInfoWidget->UpdateState(InState);

			switch (InState) {
			case EVePortalState::Disabled:
				UIPortalInfoWidget->UpdateStateColor(DefaultStateTextColor);
				break;
			case EVePortalState::Offline:
			case EVePortalState::Error:
				UIPortalInfoWidget->UpdateStateColor(OfflineStateTextColor);
				break;
			case EVePortalState::Loading:
			case EVePortalState::Downloading:
			case EVePortalState::Editor:
				UIPortalInfoWidget->UpdateStateColor(LoadingStateTextColor);
				break;
			case EVePortalState::WaitServer:
				UIPortalInfoWidget->UpdateStateColor(WaitServerStateTextColor);
				break;
			case EVePortalState::Online:
			case EVePortalState::Local:
				UIPortalInfoWidget->UpdateStateColor(OnlineStateTextColor);
				break;
			case EVePortalState::ConfirmDownload:
				UIPortalInfoWidget->UpdateStateColor(ConfirmationStateTextColor);
				break;
			case EVePortalState::NoPak:
				UIPortalInfoWidget->UpdateStateColor(OfflineStateTextColor);
				break;
			}
		}
	}

	if (State != InState) {
		State = InState;
		NativeOnPortalStateChanged(State);
	}
}

void AVePortal::NativeOnPortalStateChanged(EVePortalState InState) {
	OnPortalStateChanged.Broadcast(InState);
	OnStateChanged_BP(InState);
}

const FGuid& AVePortal::GetId() const {
	if (PlaceableComponent->GetMetadata().EntityId.IsValid()) {
		return PlaceableComponent->GetMetadata().EntityId;
	}
	return Id;
}

const FVePlaceableMetadata& AVePortal::GetPlaceableMetadata() const {
	return PlaceableComponent->GetMetadata();
}

void AVePortal::UpdatePortalMetadata() {

	// Get PortalId from placeable metadata
	const auto& PortalId = GetId();

	if (!PortalId.IsValid()) {
		VeLogErrorFunc("Invalid portal id");
		SetPortalState(EVePortalState::NoPak);
		return;
	}

	GET_MODULE_SUBSYSTEM(PortalSubsystem, Api2, Portal);
	if (!PortalSubsystem) {
		SetPortalState(EVePortalState::Error);
		return;
	}

	const auto OnCompleted = MakeShared<FOnPortalRequestCompleted2>();
	OnCompleted->BindWeakLambda(this, [=](const FApiPortalMetadata& InMetadata, const EApi2ResponseCode InResponseCode, const FString& InError) {
		SetPlaceableState(EPlaceableState::None);

		if (InResponseCode == EApi2ResponseCode::Ok) {
			SetPortalMetadata(InMetadata);
		} else {
			VeLogErrorFunc("Failed to get portal metadata: %s", *InError);
			SetPortalState(EVePortalState::Error);
		}
	});

	SetPlaceableState(EPlaceableState::Downloading);
	PortalSubsystem->Get(PortalId, OnCompleted);
}

void AVePortal::SetPortalMetadata(const FApiPortalMetadata& InMetadata) {
	ApiPortalMetadata = InMetadata;

	GET_MODULE_SUBSYSTEM(WorldSubsystem, GameFramework, World);
	if (WorldSubsystem) {
		auto Item = WorldSubsystem->GetItem(ApiPortalMetadata.Destination.Space.Id, ApiPortalMetadata.Destination.Space);
		SetWorldItem(Item);
	}

	PlayerStartTag = FName(InMetadata.Id.ToString(EGuidFormats::DigitsWithHyphens));

	RequestPreviewCubemap();

	if (WidgetComponent) {
		if (const UUIPortalInfoWidget* UIPortalInfoWidget = Cast<UUIPortalInfoWidget>(WidgetComponent->GetWidget())) {
			UIPortalInfoWidget->UpdateName(InMetadata.Destination.Space.Name);
		}
	}
}

void AVePortal::SetPlaceableState(EPlaceableState InState, float InRatio) {
	SetPlaceableStateHelper(InState, InRatio);
}
