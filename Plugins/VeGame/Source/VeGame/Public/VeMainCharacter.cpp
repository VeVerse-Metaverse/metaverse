// Author: Egor A. Pristavka. Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.


#include "VeMainCharacter.h"

#include "UIWidgetInteractionComponent.h"
#include "VePlayerStateBase.h"
#include "Components/InteractiveTargetComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Net/UnrealNetwork.h"
#include "Subsystems/UICharacterCustomizationSubsystem.h"
#include "VeUI.h"
#include "VeApi.h"
#include "VeApi2.h"
#include "VeGameModule.h"
#include "Api2AuthSubsystem.h"
#include "VeLiveLinkComponent.h"
#include "Character/ALSBaseCharacter.h"
// #include "Character/ALSPlayerCameraManager.h"
#include "Components/ALSDebugComponent.h"
#include "EnhancedInputComponent.h"
// #include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"


// Sets default values
AVeMainCharacter::AVeMainCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer) {
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

#pragma region Widget interaction
	WidgetInteractionComponent = CreateDefaultSubobject<UUIWidgetInteractionComponent>(FName("WidgetInteraction"));
	WidgetInteractionComponent->InteractionDistance = 2000.0f;
	WidgetInteractionComponent->bEnableHitTesting = true;
#pragma endregion Widget interaction

#pragma region Persona preview rendering
	PersonaFacePreviewCaptureComponent = CreateDefaultSubobject<USceneCaptureComponent2D>(FName("PersonaFacePreviewCapture"));
	if (IsValid(PersonaFacePreviewCaptureComponent)) {
		PersonaFacePreviewCaptureComponent->SetupAttachment(RootComponent);
	}

	PersonaFullPreviewCaptureComponent = CreateDefaultSubobject<USceneCaptureComponent2D>(FName("PersonaFullPreviewCapture"));
	if (IsValid(PersonaFullPreviewCaptureComponent)) {
		PersonaFullPreviewCaptureComponent->SetupAttachment(RootComponent);
	}

	PersonaPreviewLightComponent = CreateDefaultSubobject<USpotLightComponent>(FName("PersonaPreviewLightComponent"));
	if (PersonaPreviewLightComponent) {
		PersonaPreviewLightComponent->Intensity = 5000.0f;
		PersonaPreviewLightComponent->AttenuationRadius = 1000.0f;
		PersonaPreviewLightComponent->InnerConeAngle = 0.0f;
		PersonaPreviewLightComponent->OuterConeAngle = 20.0f;
		PersonaPreviewLightComponent->CastShadows = false;
		PersonaPreviewLightComponent->CastStaticShadows = false;
		PersonaPreviewLightComponent->CastDynamicShadows = false;
		PersonaPreviewLightComponent->bUseRayTracedDistanceFieldShadows = false;
		PersonaPreviewLightComponent->SetCastRaytracedShadow(false);
		PersonaPreviewLightComponent->bAffectTranslucentLighting = 0;
		PersonaPreviewLightComponent->bAffectGlobalIllumination = 0;
		if (auto* SkeletalMeshComponent = GetMesh()) {
			PersonaPreviewLightComponent->SetupAttachment(SkeletalMeshComponent);
			PersonaPreviewLightComponent->SetRelativeLocation(FVector(0.0f, 300.0f, 0.0f));
			PersonaPreviewLightComponent->SetRelativeRotation(FRotator(-10.0f, -90.0f, 0.0f));
		}
		PersonaPreviewLightComponent->SetHiddenInGame(true);
	}
#pragma endregion Persona preview rendering

#pragma region Look target
	LookTargetTrigger = CreateDefaultSubobject<UBoxComponent>(FName("LookTargetTrigger"));
	if (LookTargetTrigger) {
		LookTargetTrigger->SetBoxExtent(FVector(300.0f, 300.0f, 150.0f), false);
		LookTargetTrigger->SetupAttachment(RootComponent);
		LookTargetTrigger->SetRelativeLocation(FVector(425.0f, 0.0f, 0.0f));
		LookTargetTrigger->SetRelativeRotation(FRotator(0.0f, 0.0f, 45.0f));
		LookTargetTrigger->SetGenerateOverlapEvents(true);
	}

	if (!ActorHasTag(FName("LookTarget"))) {
		Tags.AddUnique(FName("LookTarget"));
	}
#pragma endregion Look target

#pragma region VeLiveLink
	VeLiveLinkComponent = CreateDefaultSubobject<UVeLiveLinkComponent>(FName("VeLiveLink"));
	VeLiveLinkComponent->SetupAttachment(GetMesh());
#pragma endregion VeLiveLink
}

void AVeMainCharacter::RegisterCallbacks() {
	Super::RegisterCallbacks();

	// Run only on the owning client.
	if (!IsRunningDedicatedServer() && IsLocallyControlled()) {

		GET_MODULE_SUBSYSTEM(Auth2Subsystem, Api2, Auth);
		if (Auth2Subsystem) {
			auto OnChangeAuthorizationStateCallback = [=](EAuthorizationState AuthorizationState) {
				if (AuthorizationState == EAuthorizationState::Unknown) {
					return;
				}

				if (Auth2Subsystem->IsAuthorized()) {
					Owner_SetCurrentPersonaId(Auth2Subsystem->GetUserMetadata().DefaultPersonaMetadata.Id);
					return;
				}

			};

			Auth2Subsystem->GetOnChangeAuthorizationState().AddWeakLambda(this, OnChangeAuthorizationStateCallback);
			OnChangeAuthorizationStateCallback(Auth2Subsystem->GetAuthorizationState());
		}

		if (FVeUIModule* UIModule = FVeUIModule::Get()) {
			if (const TSharedPtr<FUICharacterCustomizationSubsystem> CharacterCustomizationSubsystem = UIModule->GetCharacterCustomizationSubsystem()) {
				// Called when user clicks at the persona icon in the persona list. 
				if (!OnPersonaSelectedDelegateHandle.IsValid()) {
					OnPersonaSelectedDelegateHandle = CharacterCustomizationSubsystem->GetNotifyPersonaSelected().AddWeakLambda(
						this, [=](const FGuid& PersonaId) {
							Owner_SetCurrentPersonaId(PersonaId);
						});
				}

				if (!OnUpdatePersonaFullPreviewDelegateHandle.IsValid()) {
					OnUpdatePersonaFullPreviewDelegateHandle = CharacterCustomizationSubsystem->GetNotifyUpdatePersonaFullPreview().AddWeakLambda(this, [=]() {
						BP_UpdatePersonaFullPreview();
					});
				}

				if (!OnUpdatePersonaFacePreviewDelegateHandle.IsValid()) {
					OnUpdatePersonaFacePreviewDelegateHandle = CharacterCustomizationSubsystem->GetNotifyUpdatePersonaFacePreview().AddWeakLambda(this, [=]() {
						BP_UpdatePersonaFacePreview();
					});
				}

				// Called when a create new persona request completes.
				if (!OnPersonaCreateRequestDelegateHandle.IsValid()) {
					OnPersonaCreateRequestDelegateHandle = CharacterCustomizationSubsystem->GetNotifyCreatePersonaRequest().AddWeakLambda(
						this, [=](const FApiPersonaMetadata& InMetadata) {
							Owner_SetCurrentPersonaId(InMetadata.Id);
						});
				}
			} else { LogErrorF("failed to get subsystem F%s%sSubsystem", STRINGIFY(UI), STRINGIFY(CharacterCustomization)); }
		} else { LogErrorF("failed to get module FVe%sModule", STRINGIFY(UI)); }
	}
}

void AVeMainCharacter::UnregisterCallbacks() {

	Super::UnregisterCallbacks();

	auto* VePlayerState = GetPlayerState<AVePlayerStateBase>();
	if (IsValid(VePlayerState)) {
		VePlayerState->GetOnUserMetadataUpdated().RemoveAll(this);
	}

	if (!IsRunningDedicatedServer() && IsLocallyControlled()) {

		if (!OnPersonaCreateRequestDelegateHandle.IsValid()) {
			if (FVeUIModule* UIModule = FVeUIModule::Get()) {
				if (const TSharedPtr<FUICharacterCustomizationSubsystem> CharacterCustomizationSubsystem = UIModule->GetCharacterCustomizationSubsystem()) {
					CharacterCustomizationSubsystem->GetNotifyCreatePersonaRequest().Remove(OnPersonaCreateRequestDelegateHandle);
					OnPersonaCreateRequestDelegateHandle.Reset();
				} else { LogErrorF("failed to get subsystem F%s%sSubsystem", STRINGIFY(UI), STRINGIFY(CharacterCustomization)); }
			} else { LogErrorF("failed to get module FVe%sModule", STRINGIFY(UI)); }
		}

		if (!OnUpdatePersonaFacePreviewDelegateHandle.IsValid()) {
			if (FVeUIModule* UIModule = FVeUIModule::Get()) {
				if (const TSharedPtr<FUICharacterCustomizationSubsystem> CharacterCustomizationSubsystem = UIModule->GetCharacterCustomizationSubsystem()) {
					CharacterCustomizationSubsystem->GetNotifyUpdatePersonaFacePreview().Remove(OnUpdatePersonaFacePreviewDelegateHandle);
					OnUpdatePersonaFacePreviewDelegateHandle.Reset();
				} else { LogErrorF("failed to get subsystem F%s%sSubsystem", STRINGIFY(UI), STRINGIFY(CharacterCustomization)); }
			} else { LogErrorF("failed to get module FVe%sModule", STRINGIFY(UI)); }
		}

		if (!OnUpdatePersonaFullPreviewDelegateHandle.IsValid()) {
			if (FVeUIModule* UIModule = FVeUIModule::Get()) {
				if (const TSharedPtr<FUICharacterCustomizationSubsystem> CharacterCustomizationSubsystem = UIModule->GetCharacterCustomizationSubsystem()) {
					CharacterCustomizationSubsystem->GetNotifyUpdatePersonaFullPreview().Remove(OnUpdatePersonaFullPreviewDelegateHandle);
					OnUpdatePersonaFullPreviewDelegateHandle.Reset();
				} else { LogErrorF("failed to get subsystem F%s%sSubsystem", STRINGIFY(UI), STRINGIFY(CharacterCustomization)); }
			} else { LogErrorF("failed to get module FVe%sModule", STRINGIFY(UI)); }
		}

		if (!OnPersonaSelectedDelegateHandle.IsValid()) {
			if (FVeUIModule* UIModule = FVeUIModule::Get()) {
				if (const TSharedPtr<FUICharacterCustomizationSubsystem> CharacterCustomizationSubsystem = UIModule->GetCharacterCustomizationSubsystem()) {
					CharacterCustomizationSubsystem->GetNotifyPersonaSelected().Remove(OnPersonaSelectedDelegateHandle);
					OnPersonaSelectedDelegateHandle.Reset();
				} else { LogErrorF("failed to get subsystem F%s%sSubsystem", STRINGIFY(UI), STRINGIFY(CharacterCustomization)); }
			} else { LogErrorF("failed to get module FVe%sModule", STRINGIFY(UI)); }
		}
	}
}

void AVeMainCharacter::OnLookTargetTriggerBeginOverlap(UPrimitiveComponent* /*OverlappedComponent*/, AActor* OtherActor, UPrimitiveComponent* /*OtherComp*/,
	int32 /*OtherBodyIndex*/,
	bool /*bFromSweep*/, const FHitResult& /*SweepResult*/) {
	if ((OtherActor->ActorHasTag(FName("LookTarget")) || OtherActor->FindComponentByClass(UInteractiveTargetComponent::StaticClass())) && OtherActor != this) {
		RegisterLookTarget(OtherActor);
	}
}

void AVeMainCharacter::OnLookTargetTriggerEndOverlap(UPrimitiveComponent* /*OverlappedComponent*/, AActor* OtherActor, UPrimitiveComponent* /*OtherComp*/,
	int32 /*OtherBodyIndex*/) {
	if ((OtherActor->ActorHasTag(FName("LookTarget")) || OtherActor->FindComponentByClass(UInteractiveTargetComponent::StaticClass())) && OtherActor != this) {
		UnregisterLookTarget(OtherActor);
	}
}

// Called when the game starts or when spawned
void AVeMainCharacter::BeginPlay() {
	Super::BeginPlay();

	// Check for the server.
	if (GetNetMode() != NM_DedicatedServer) {
		if (IsValid(LookTargetTrigger)) {
			if (!LookTargetTrigger->OnComponentBeginOverlap.IsAlreadyBound(this, &AVeMainCharacter::OnLookTargetTriggerBeginOverlap)) {
				LookTargetTrigger->OnComponentBeginOverlap.AddDynamic(this, &AVeMainCharacter::OnLookTargetTriggerBeginOverlap);
			}

			if (!LookTargetTrigger->OnComponentEndOverlap.IsAlreadyBound(this, &AVeMainCharacter::OnLookTargetTriggerEndOverlap)) {
				LookTargetTrigger->OnComponentEndOverlap.AddDynamic(this, &AVeMainCharacter::OnLookTargetTriggerEndOverlap);
			}

			if (!LookTargetTimerHandle.IsValid()) {
				GetWorld()->GetTimerManager().SetTimer(LookTargetTimerHandle, [=] {
					LookTarget = FindLookTarget();
					UpdateLookTargetBP(LookTarget);
				}, 1.0f, true);
			}
		}
	}
}

void AVeMainCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason) {
	Super::EndPlay(EndPlayReason);

	if (LookTargetTimerHandle.IsValid()) {
		GetWorld()->GetTimerManager().ClearTimer(LookTargetTimerHandle);
		LookTargetTimerHandle.Invalidate();
	}
}

void AVeMainCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AVeMainCharacter, CurrentPersonaId);
	DOREPLIFETIME(AVeMainCharacter, CurrentPersonaFileUrl);
}

void AVeMainCharacter::RegisterLookTarget(AActor* InLookTarget) {
	RegisteredLookTargets.Add(InLookTarget);
}

void AVeMainCharacter::UnregisterLookTarget(AActor* InLookTarget) {
	RegisteredLookTargets.Remove(InLookTarget);
}

AActor* AVeMainCharacter::FindLookTarget() {
	if (RegisteredLookTargets.Num() > 0) {
		AActor* ClosestTarget = nullptr;
		float LastDistance = TNumericLimits<float>::Max();

		for (AActor* Target : RegisteredLookTargets) {
			const float TargetDistance = GetDistanceTo(Target);
			if (TargetDistance < LastDistance) {
				LastDistance = TargetDistance;
				ClosestTarget = Target;
			}
		}

		LookTarget = ClosestTarget;

		return ClosestTarget;
	}

	return nullptr;
}

void AVeMainCharacter::Owner_SetCurrentPersonaId(const FGuid& InPersonaId) {
	if (IsLocallyControlled()) {
		if (GetNetMode() == NM_Standalone) {
			Authority_SetCurrentPersonaId(InPersonaId);
		} else {
			Server_SetCurrentPersonaId(InPersonaId);
		}
	}
}

void AVeMainCharacter::Authority_SetCurrentPersonaId(const FGuid& InPersonaId) {
	if (!HasAuthority()) {
		return;
	}

	CurrentPersonaId = InPersonaId;

	GET_MODULE_SUBSYSTEM(PersonaSubsystem, Api, Persona);
	if (PersonaSubsystem) {
		const auto Callback = MakeShared<FOnPersonaRequestCompleted>();
		Callback->BindWeakLambda(this, [=](FApiPersonaMetadata InPersonaMetadata, const bool bInSuccessful, const FString& InErrorString) {
			if (!bInSuccessful) {
				VeLogErrorFunc("Request failed: %s", *InErrorString);
			} else {
				// Get the avatar mesh file metadata.
				const auto* FileMetadata = InPersonaMetadata.Files.FindByPredicate([](const FApiFileMetadata& Item) {
					return Item.Type == EApiFileType::MeshAvatar;
				});

				if (FileMetadata && !FileMetadata->Url.IsEmpty()) {
					Authority_SetCurrentPersonaFileUrl(FileMetadata->Url);
				} else {
					VeLogErrorFunc("Persona %s has no avatar mesh", *InPersonaMetadata.Name);
				}
			}
		});

		PersonaSubsystem->GetPersona(InPersonaId, Callback);
	}
}

void AVeMainCharacter::Authority_SetCurrentPersonaFileUrl(const FString& InPersonaFileUrl) {
	if (!HasAuthority()) {
		return;
	}

	CurrentPersonaFileUrl = InPersonaFileUrl;
	OnRep_CurrentPersonaFileUrl();
}

void AVeMainCharacter::OnRep_CurrentPersonaFileUrl() {
	if (!IsRunningDedicatedServer()) {
		if (IsLocallyControlled()) {
			// Standalone, listen server or owning client
			Owner_OnPersonaFileUrlChangedBP(CurrentPersonaFileUrl);
		} else {
			// Remote clients
			Remote_OnPersonaFileUrlChangedBP(CurrentPersonaFileUrl);
		}
	}
}

FGuid AVeMainCharacter::GetCurrentPersonaId() {
	return CurrentPersonaId;
}

void AVeMainCharacter::NotifyPersonaLoaded(const FGuid& PersonaId) {
	if (FVeUIModule* UIModule = FVeUIModule::Get()) {
		if (const TSharedPtr<FUICharacterCustomizationSubsystem> CharacterCustomizationSubsystem = UIModule->GetCharacterCustomizationSubsystem()) {
			CharacterCustomizationSubsystem->NotifyPersonaLoaded(PersonaId);
		} else { LogErrorF("failed to get subsystem F%s%sSubsystem", STRINGIFY(UI), STRINGIFY(CharacterCustomization)); }
	} else { LogErrorF("failed to get module FVe%sModule", STRINGIFY(UI)); }
}

void AVeMainCharacter::Server_SetCurrentPersonaId_Implementation(const FGuid& InPersonaId) {
	Authority_SetCurrentPersonaId(InPersonaId);
}

// Called every frame
void AVeMainCharacter::Tick(const float DeltaTime) {
	Super::Tick(DeltaTime);

	// Update widget interaction origin only at local client.
	if (IsValid(WidgetInteractionComponent)) {
		const auto* PlayerController = Cast<APlayerController>(GetController());
		if (IsValid(PlayerController) && PlayerController->IsLocalController()) {
			FVector CameraLocation;
			FRotator CameraRotation;
			PlayerController->GetPlayerViewPoint(CameraLocation, CameraRotation);
			WidgetInteractionComponent->SetWorldLocationAndRotation(CameraLocation, CameraRotation);
		}
	}
}
