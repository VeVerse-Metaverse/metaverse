// Authors: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022-2023 VeVerse AS. All rights reserved.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "VeAiCharacter.h"

#include "UIAIInfoWidget.h"
#include "VeAiPlayerState.h"
#include "AiInfoWidgetComponent.h"
#include "CogAiEmotionMappingDataAsset.h"
#include "CogAiFloatingThought.h"
#include "CogAiNpcSettingsComponent.h"
#include "FileDownloadComponent.h"
#include "RpcWebSocketsTextChatSubsystem.h"
#include "RuntimeAudioImporterLibrary.h"
#include "Components/InteractionSubjectComponent.h"
#include "Net/UnrealNetwork.h"
#include "VeApi.h"
#include "VeAI.h"
#include "VeAiBlueprintFunctionLibrary.h"
#include "VeAiController.h"
#include "VeRpc.h"
// #include "YnnkLipsyncController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/AudioComponent.h"
#include "Components/CapsuleComponent.h"
#include "Engine/CurveTable.h"
#include "CogAiMetaHuman.h"
#include "Components/ArrowComponent.h"
#include "Perception/AISense_Hearing.h"

// Sets default values
AVeAiCharacter::AVeAiCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer) {
	PrimaryActorTick.bCanEverTick = true;

	bAlwaysRelevant = true;

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

#if 0 // breaks ALS, sprinting staggers when using controller rotation yaw
	// bUseControllerRotationPitch = false;
	// bUseControllerRotationYaw = true;
	// bUseControllerRotationRoll = false;
#endif

	AiInfoWidgetComponent = CreateDefaultSubobject<UAiInfoWidgetComponent>("AiInfoWidgetComponent");
	AiInfoWidgetComponent->SetupAttachment(RootComponent);
	AiInfoWidgetComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 120.0f));

	FileDownloadComponent = CreateDefaultSubobject<UFileDownloadComponent>("FileDownloadComponent");

	AudioComponent = CreateDefaultSubobject<UAudioComponent>("AudioComponent");
	AudioComponent->SetupAttachment(RootComponent);

	FloatingThoughtsSpawnLocation = CreateDefaultSubobject<UArrowComponent>("FloatingThoughtsSpawnLocation");
	FloatingThoughtsSpawnLocation->SetupAttachment(RootComponent);
	FloatingThoughtsSpawnLocation->SetArrowColor(FColor::Black);

	if (InteractionSubjectComponent) {
		InteractionSubjectComponent->SetControlMode(EInteractionControlMode::Pawn);
	}

	if (!IsRunningDedicatedServer()) {
		BlinkTimer = 0.0f;
		CurrentBlinkInterval = FMath::RandRange(MinBlinkInterval, MaxBlinkInterval);
		BlinkAlpha = 0.0f;
		bIsBlinking = false;
	}
}

void AVeAiCharacter::BeginPlay() {
	Super::BeginPlay();

	// Moved to OnAiPlayerStateChange
	// if (HasAuthority()) {
	// 	if (IsValid(AiPlayerState)) {
	// 		AiPlayerState->GetOnAiUserMetadataUpdated().AddWeakLambda(this, [=](const FVeUserMetadata& InUserMetadata) {
	// 			if (HasAuthority()) {
	// 				Authority_SetCurrentPersonaId(InUserMetadata.DefaultPersonaMetadata.Id);
	// 			}
	// 		});
	// 	}
	// }
}

void AVeAiCharacter::Tick(float DeltaSeconds) {
	Super::Tick(DeltaSeconds);

	// Do not run on dedicated server having no RHI.
	if (!IsRunningDedicatedServer()) {

		// Update morph targets related to speech.
		if (bUseSayMorphTargets) {
			SayModifyMorphTargets(DeltaSeconds);
		}

		// Update morph targets related to emotions.
		if (bUseEmotions) {
			EmoteModifyMorphTargets(DeltaSeconds);
		}

		// Update eye blinking.
		if (bUseEyeBlinking) {
			UpdateBlinking(DeltaSeconds);
		}

		// Update eye looking.
		if (bUseEyeLooking) {
			UpdateEyeLooking(DeltaSeconds);
		}
	}
}

void AVeAiCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason) {
	Super::EndPlay(EndPlayReason);
}

void AVeAiCharacter::PossessedBy(AController* NewController) {
	Super::PossessedBy(NewController);

	// Run on server only.
	if (HasAuthority()) {
		if (auto* AIController = Cast<AVeAiController>(NewController); IsValid(AIController)) {
			// Tell the AI controller what user ID to use. This will make controller to request user metadata from the API and set it on the player state (including persona).
			if (UserId.IsValid()) {
				AIController->SetUserId(UserId);
			} else {
				if (!CustomPersonaAvatarMeshUrl.IsEmpty()) {
					Authority_SetCurrentPersonaFileUrl(CustomPersonaAvatarMeshUrl);
				}
			}

			// Register with AI player state received from the controller.
			if (auto* State = AIController->GetAiPlayerState()) {
				SetAiPlayerState(State);
			}
		}
	}
}

void AVeAiCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate AI player state to clients.
	DOREPLIFETIME(AVeAiCharacter, AiPlayerState);

	// Replicate persona id and file URL to clients so that they can load the correct persona.
	DOREPLIFETIME(AVeAiCharacter, CurrentPersonaId);
	DOREPLIFETIME(AVeAiCharacter, CurrentPersonaFileUrl);

	// Replicate use emotions flag to clients.
	DOREPLIFETIME(AVeAiCharacter, bUseEmotions);

	// Replicate current emotion and intensity to clients.
	DOREPLIFETIME(AVeAiCharacter, CurrentEmotion);
	DOREPLIFETIME(AVeAiCharacter, CurrentEmotionIntensity);

	// Replicate current TTS URL to clients (audio).
	DOREPLIFETIME(AVeAiCharacter, CurrentTtsUrl);

	// Replicate current TTS message (subtitles) to clients.
	DOREPLIFETIME(AVeAiCharacter, CurrentTtsMessage);
}

void AVeAiCharacter::OnRep_AiPlayerState() {
	// Do not run on dedicated server having no RHI.
	if (!IsRunningDedicatedServer()) {
		// Set AI player state on the clients (should be already set on the server as this is a replication callback).
		SetAiPlayerState(AiPlayerState);

		// Get the widget component and set the AI player state on the widget to update the Name and Title.
		if (const auto WidgetComponent = AiInfoWidgetComponent; IsValid(WidgetComponent)) {
			if (auto* AiInfoWidget = Cast<UUIAIInfoWidget>(WidgetComponent->GetWidget()); IsValid(AiInfoWidget)) {
				auto* State = GetAiPlayerState<AVeAiPlayerState>();
				AiInfoWidget->SetAIPlayerState(State);
			}
		}
	}
}

void AVeAiCharacter::SetAiPlayerState(AVeAiPlayerState* NewPlayerState) {
	// Unregister from the old AI player state.
	if (AiPlayerState && AiPlayerState->GetPawn() == this) {
		AiPlayerState->SetPawn(nullptr);
	}

	// Set the new AI player state.
	AiPlayerState = NewPlayerState;

	// Register with the new AI player state, notifying that it is now associated with this pawn.
	if (AiPlayerState) {
		AiPlayerState->SetPawn(this);
	}

	// Notify that the AI player state has changed.
	OnAiPlayerStateChange();
}

void AVeAiCharacter::OnAiPlayerStateChange() {
	// Notify that the AI player state has changed.
	OnAiPlayerStateChanged.Broadcast(AiPlayerState);

	// Run on server only.
	if (HasAuthority()) {
		if (IsValid(AiPlayerState)) {
			// Register with the AI player state to receive updates on user metadata.
			AiPlayerState->GetOnAiUserMetadataUpdated().AddWeakLambda(this, [=](const FVeUserMetadata& InUserMetadata) {
				if (HasAuthority()) {
					// Set the current persona ID to the default persona ID received from the API.
					Authority_SetCurrentPersonaId(InUserMetadata.DefaultPersonaMetadata.Id);

					// Find the NPC settings component. It should be enough to set the perceptible properties only at the server.
					if (auto* SettingsComponent = FindComponentByClass<UCogAiNpcSettingsComponent>(); IsValid(SettingsComponent)) {
						if (InUserMetadata.DefaultPersonaMetadata.Name.IsEmpty()) {
							// Use the user name if the default persona name is not set.
							SettingsComponent->PerceptibleProperties.Add(TEXT("name"), InUserMetadata.Name);
						} else {
							// Use the default persona name if it is set.
							SettingsComponent->PerceptibleProperties.Add(TEXT("name"), InUserMetadata.DefaultPersonaMetadata.Name);
						}

						if (InUserMetadata.DefaultPersonaMetadata.Description.IsEmpty()) {
							// Use the user description if the default persona description is not set.
							SettingsComponent->PerceptibleProperties.Add(TEXT("description"), InUserMetadata.Description);
						} else {
							// Use the default persona description if it is set.
							SettingsComponent->PerceptibleProperties.Add(TEXT("description"), InUserMetadata.DefaultPersonaMetadata.Description);
						}
					}
				}
			});
		}
	}

	// Clients can override the default behavior if required.
}

void AVeAiCharacter::Authority_SetCurrentPersonaId(const FGuid& InPersonaId) {
	// Run on server only.
	if (!HasAuthority()) {
		return;
	}

	// Set the current persona ID. This property is replicated to clients, so they can load the correct persona.
	CurrentPersonaId = InPersonaId;

	GET_MODULE_SUBSYSTEM(PersonaSubsystem, Api, Persona);
	if (PersonaSubsystem) {
		const auto Callback = MakeShared<FOnPersonaRequestCompleted>();
		Callback->BindWeakLambda(this, [=](FApiPersonaMetadata InPersonaMetadata, const bool bInSuccessful, const FString& InErrorString) {
			if (!bInSuccessful) {
				VeLogErrorFunc("Request failed: %s", *InErrorString);
			} else {
				// Try to find the avatar mesh file.
				const auto* FileMetadata = InPersonaMetadata.Files.FindByPredicate([](const FApiFileMetadata& Item) {
					return Item.Type == EApiFileType::MeshAvatar;
				});

				if (FileMetadata && !FileMetadata->Url.IsEmpty()) {
					// Set the current persona file URL at the server.
					Authority_SetCurrentPersonaFileUrl(FileMetadata->Url);
				} else {
					VeLogErrorFunc("Persona %s has no avatar mesh", *InPersonaMetadata.Name);
				}
			}
		});

		// Request the persona metadata from the API.
		PersonaSubsystem->GetPersona(InPersonaId, Callback);
	}
}

void AVeAiCharacter::Authority_SetCurrentPersonaFileUrl(const FString& InPersonaFileUrl) {
	// Run on server only.
	if (!HasAuthority()) {
		return;
	}

	// Set the current persona file URL. This property is replicated to clients, so they can load the correct persona.
	CurrentPersonaFileUrl = InPersonaFileUrl;

	// Notify that the current persona file URL has changed, so that standalone and listen servers can load the the persona mesh.
	OnRep_CurrentPersonaFileUrl();
}

void AVeAiCharacter::OnRep_CurrentPersonaFileUrl() {
	if (!IsRunningDedicatedServer()) {
		// Run on clients only.
		Remote_OnPersonaFileUrlChangedBP(CurrentPersonaFileUrl);
	}
}

void AVeAiCharacter::OnRep_CurrentTtsUrl() {
	if (!IsRunningDedicatedServer()) {
		const FOnDownloadProgress OnDownloadProgress;
		FOnDownloadComplete OnDownloadComplete;
		OnDownloadComplete.BindUObject(this, &AVeAiCharacter::OnDownloadCompleted);
		FileDownloadComponent->StartDownload(CurrentTtsUrl, OnDownloadProgress, OnDownloadComplete);
	}
}

void AVeAiCharacter::UpdateBlinking(float DeltaTime) {
	auto* SkeletalMeshComponent = GetMesh();
	if (!SkeletalMeshComponent) {
		return;
	}

	BlinkTimer += DeltaTime;

	if (!bIsBlinking && BlinkTimer >= CurrentBlinkInterval) {
		StartBlinking();
	}

	if (bIsBlinking) {
		BlinkAlpha = FMath::Clamp(BlinkAlpha + DeltaTime * BlinkCloseSpeed, 0.0f, 1.0f);
		if (BlinkAlpha >= 1.0f) {
			bIsBlinking = false;
			BlinkTimer = 0.0f;
			CurrentBlinkInterval = FMath::RandRange(MinBlinkInterval, MaxBlinkInterval);
		}
	} else {
		BlinkAlpha = FMath::Clamp(BlinkAlpha - DeltaTime * BlinkOpenSpeed, 0.0f, 1.0f);
	}

	SkeletalMeshComponent->SetMorphTarget(BlinkLeftMorphTargetName, BlinkAlpha);
	SkeletalMeshComponent->SetMorphTarget(BlinkRightMorphTargetName, BlinkAlpha);
}

void AVeAiCharacter::StartBlinking() {
	bIsBlinking = true;
}

void AVeAiCharacter::UpdateEyeLooking(float DeltaTime) {
	if (!bUseEyeLooking) {
		return;
	}

	// Currently entity of interest
	const AActor* TargetActor = nullptr;

	if (EyeLookingDebugTargetActor) {
		// Try to use the debug target actor.
		TargetActor = EyeLookingDebugTargetActor;
	} else if (auto* AIController = Cast<AAIController>(GetController())) {
		// Get blackboard component from AI controller.
		if (const auto* BlackboardComponent = AIController->GetBlackboardComponent()) {
			// Get target actor from blackboard so we can look at it.
			TargetActor = Cast<AActor>(BlackboardComponent->GetValueAsObject("Target"));
		}
	}

	if (RandomEyeMovementEnabled) {
		// Update random eye movement timer
		RandomEyeMovementTimer -= DeltaTime;

		// Generate new random eye movement offsets
		if (RandomEyeMovementTimer <= 0.0f) {
			// Generate random timer for next eye movement.
			RandomEyeMovementTimer = FMath::RandRange(EyeRandomMovementTimeMin, EyeRandomMovementTimeMax);
			// Generate random offset for right eye.
			RightEyeRandomOffset = FVector2D(FMath::RandRange(EyeRandomMovementOffsetMin, EyeRandomMovementOffsetMax), FMath::RandRange(EyeRandomMovementOffsetMin, EyeRandomMovementOffsetMax));
			// Mirror left eye offset.
			LeftEyeRandomOffset = FVector2D(-RightEyeRandomOffset.X, RightEyeRandomOffset.Y);
		}
	} else {
		LeftEyeRandomOffset.X = LeftEyeRandomOffset.Y = 0.0f;
		RightEyeRandomOffset.X = RightEyeRandomOffset.Y = 0.0f;
	}

	// Get the skeletal mesh component.
	auto* SkeletalMeshComponent = GetMesh();

	// Get current left eye look values.
	const float CurrentLeftEyeLookUp = SkeletalMeshComponent->GetMorphTarget(LeftEyeLookUpMorphTargetName);
	const float CurrentLeftEyeLookDown = SkeletalMeshComponent->GetMorphTarget(LeftEyeLookDownMorphTargetName);
	const float CurrentLeftEyeLookIn = SkeletalMeshComponent->GetMorphTarget(LeftEyeLookInMorphTargetName);
	const float CurrentLeftEyeLookOut = SkeletalMeshComponent->GetMorphTarget(LeftEyeLookOutMorphTargetName);

	// Right eye look values.
	const float CurrentRightEyeLookUp = SkeletalMeshComponent->GetMorphTarget(RightEyeLookUpMorphTargetName);
	const float CurrentRightEyeLookDown = SkeletalMeshComponent->GetMorphTarget(RightEyeLookDownMorphTargetName);
	const float CurrentRightEyeLookIn = SkeletalMeshComponent->GetMorphTarget(RightEyeLookInMorphTargetName);
	const float CurrentRightEyeLookOut = SkeletalMeshComponent->GetMorphTarget(RightEyeLookOutMorphTargetName);

	// Check if we have a target actor to look at.
	if (TargetActor) {
		// Calculate target direction vectors for left and right eyes.
		const FVector LeftEyeTargetDirection = (TargetActor->GetActorLocation() - SkeletalMeshComponent->GetBoneLocation(LeftEyeBoneName)).GetSafeNormal();
		const FVector RightEyeTargetDirection = (TargetActor->GetActorLocation() - SkeletalMeshComponent->GetBoneLocation(RightEyeBoneName)).GetSafeNormal();

		// Calculate dot products between target direction and eye bone up and right vectors, to get cosine of angle between them.
		const float LeftEyeUpAngleCos = FVector::DotProduct(LeftEyeTargetDirection, SkeletalMeshComponent->GetBoneQuaternion(LeftEyeBoneName).GetUpVector());
		const float LeftEyeRightAngleCos = FVector::DotProduct(LeftEyeTargetDirection, SkeletalMeshComponent->GetBoneQuaternion(LeftEyeBoneName).GetRightVector());
		const float RightEyeUpAngleCos = FVector::DotProduct(RightEyeTargetDirection, SkeletalMeshComponent->GetBoneQuaternion(RightEyeBoneName).GetUpVector());
		const float RightEyeRightAngleCos = FVector::DotProduct(RightEyeTargetDirection, SkeletalMeshComponent->GetBoneQuaternion(RightEyeBoneName).GetRightVector());

		// Calculate target eye look values.
		const float TargetLeftEyeLookUpDown = FMath::Clamp(LeftEyeUpAngleCos, -EyeLookUpDownClamp, EyeLookUpDownClamp);
		const float TargetLeftEyeLookLeftRight = FMath::Clamp(LeftEyeRightAngleCos, -EyeLookLeftRightClamp, EyeLookLeftRightClamp);
		const float TargetRightEyeLookUpDown = FMath::Clamp(RightEyeUpAngleCos, -EyeLookUpDownClamp, EyeLookUpDownClamp);
		const float TargetRightEyeLookLeftRight = FMath::Clamp(RightEyeRightAngleCos, -EyeLookLeftRightClamp, EyeLookLeftRightClamp);

		// Apply morph targets for left eye.
		if (LeftEyeUpAngleCos >= 0) {
			// If the angle is positive, we need to look up.
			SkeletalMeshComponent->SetMorphTarget(LeftEyeLookUpMorphTargetName, FMath::Lerp(CurrentLeftEyeLookUp, TargetLeftEyeLookUpDown, EyeLookLerpSpeed * DeltaTime));
			SkeletalMeshComponent->SetMorphTarget(LeftEyeLookDownMorphTargetName, 0);
		} else {
			// If the angle is negative, we need to look down.
			SkeletalMeshComponent->SetMorphTarget(LeftEyeLookUpMorphTargetName, 0);
			SkeletalMeshComponent->SetMorphTarget(LeftEyeLookDownMorphTargetName, FMath::Lerp(CurrentLeftEyeLookDown, -TargetLeftEyeLookUpDown, EyeLookLerpSpeed * DeltaTime));
		}
		if (LeftEyeRightAngleCos >= 0) {
			// If the angle is positive, we need to look right.
			SkeletalMeshComponent->SetMorphTarget(LeftEyeLookInMorphTargetName, FMath::Lerp(CurrentLeftEyeLookIn, TargetLeftEyeLookLeftRight, EyeLookLerpSpeed * DeltaTime));
			SkeletalMeshComponent->SetMorphTarget(LeftEyeLookOutMorphTargetName, 0);
		} else {
			// If the angle is negative, we need to look left.
			SkeletalMeshComponent->SetMorphTarget(LeftEyeLookInMorphTargetName, 0);
			SkeletalMeshComponent->SetMorphTarget(LeftEyeLookOutMorphTargetName, FMath::Lerp(CurrentLeftEyeLookOut, -TargetLeftEyeLookLeftRight, EyeLookLerpSpeed * DeltaTime));
		}

		// Apply morph targets for right eye.
		if (RightEyeUpAngleCos >= 0) {
			// If the angle is positive, we need to look up.
			SkeletalMeshComponent->SetMorphTarget(RightEyeLookUpMorphTargetName, FMath::Lerp(CurrentRightEyeLookUp, TargetRightEyeLookUpDown, EyeLookLerpSpeed * DeltaTime));
			SkeletalMeshComponent->SetMorphTarget(RightEyeLookDownMorphTargetName, 0);
		} else {
			// If the angle is negative, we need to look down.
			SkeletalMeshComponent->SetMorphTarget(RightEyeLookUpMorphTargetName, 0);
			SkeletalMeshComponent->SetMorphTarget(RightEyeLookDownMorphTargetName, FMath::Lerp(CurrentRightEyeLookDown, -TargetRightEyeLookUpDown, EyeLookLerpSpeed * DeltaTime));
		}
		if (RightEyeRightAngleCos >= 0) {
			// If the angle is positive, we need to look right.
			SkeletalMeshComponent->SetMorphTarget(RightEyeLookInMorphTargetName, FMath::Lerp(CurrentRightEyeLookIn, TargetRightEyeLookLeftRight, EyeLookLerpSpeed * DeltaTime));
			SkeletalMeshComponent->SetMorphTarget(RightEyeLookOutMorphTargetName, 0);
		} else {
			// If the angle is negative, we need to look left.
			SkeletalMeshComponent->SetMorphTarget(RightEyeLookInMorphTargetName, 0);
			SkeletalMeshComponent->SetMorphTarget(RightEyeLookOutMorphTargetName, FMath::Lerp(CurrentRightEyeLookOut, -TargetRightEyeLookLeftRight, EyeLookLerpSpeed * DeltaTime));
		}

		// Apply random eye movement offsets
		// const float NewLeftEyeLookUpDown = FMath::Lerp((CurrentLeftEyeLookUpDown + CurrentRightEyeLookUpDown) * 0.5f, TargetEyeLookUpDown + LeftEyeRandomOffset.X, EyeLookLerpSpeed * DeltaTime);
		// const float NewLeftEyeLookLeftRight = -FMath::Lerp((CurrentLeftEyeLookLeftRight + CurrentRightEyeLookLeftRight) * 0.5f, TargetEyeLookLeftRight + LeftEyeRandomOffset.Y, EyeLookLerpSpeed * DeltaTime);
		// const float NewRightEyeLookUpDown = FMath::Lerp((CurrentLeftEyeLookUpDown + CurrentRightEyeLookUpDown) * 0.5f, TargetEyeLookUpDown + RightEyeRandomOffset.X, EyeLookLerpSpeed * DeltaTime);
		// const float NewRightEyeLookLeftRight = FMath::Lerp((CurrentLeftEyeLookLeftRight + CurrentRightEyeLookLeftRight) * 0.5f, TargetEyeLookLeftRight + RightEyeRandomOffset.Y, EyeLookLerpSpeed * DeltaTime);
	}
}

void AVeAiCharacter::EmoteModifyMorphTargets(const float DeltaSeconds) {
	ElapsedEmotionTime += DeltaSeconds;

	if (auto* MeshComponent = GetMesh(); EmotionMappingDataAssetRef && IsValid(MeshComponent)) {
		if (const auto* EmotionMapping = EmotionMappingDataAssetRef->Emotions.Find(FName(CurrentEmotion)); EmotionMapping) {
			if (const auto* CurveTable = EmotionMapping->CurveTable; IsValid(CurveTable)) {
				switch (CurveTable->GetCurveTableMode()) {
				case ECurveTableMode::RichCurves:
					for (const auto Curve : CurveTable->GetRichCurveRowMap()) {
						auto MorphTargetName = Curve.Key;
						if (MorphTargetName.IsNone()) {
							continue;
						}

						if (Curve.Value) {
							MeshComponent->SetMorphTarget(MorphTargetName, Curve.Value->Eval(ElapsedEmotionTime));
						}
					}
					break;
				case ECurveTableMode::SimpleCurves:
					for (const auto Curve : CurveTable->GetSimpleCurveRowMap()) {
						auto MorphTargetName = Curve.Key;
						if (MorphTargetName.IsNone()) {
							continue;
						}

						if (Curve.Value) {
							MeshComponent->SetMorphTarget(MorphTargetName, Curve.Value->Eval(ElapsedEmotionTime));
						}
					}
					break;
				default:
					VeLogWarningFunc("Unknown curve table mode");
					break;
				}
			}
		}
	}
}

void AVeAiCharacter::SetEmotion(const FName& InEmotionName, float InIntensity) {
	CurrentEmotion = InEmotionName;
	CurrentEmotionIntensity = InIntensity;
	ElapsedEmotionTime = 0.0f;

	if (MetaHuman && bUseMetaHuman) {
		MetaHuman->SetNextEmotion(InEmotionName, InIntensity);
	}

	OnEmotionSet(InEmotionName, InIntensity);
}

void AVeAiCharacter::ResetEmotion() {
	CurrentEmotion = NAME_None;
	CurrentEmotionIntensity = 0.0f;
	ElapsedEmotionTime = 0.0f;
}

void AVeAiCharacter::OnActionTaken_Implementation(UCogAiActionMessage* ActionMessage) {
	if (auto* Emotion = ActionMessage->Parameters.Find(TEXT("emotion"))) {
		SetEmotion(FName(*Emotion));
	}

	if (HasAuthority()) {
		// Notify clients.
		Client_OnActionTaken(FCogAiActionMessageReplicated(ActionMessage));
	}
}

void AVeAiCharacter::Client_OnActionTaken_Implementation(const FCogAiActionMessageReplicated& InActionMessageReplicated) {
	if (IsRunningDedicatedServer()) {
		return;
	}

	Client_OnActionTakenBP(InActionMessageReplicated.ToClientMessage());
}

void AVeAiCharacter::Client_OnActionTakenBP_Implementation(const FCogAiActionMessageClient& ActionMessage) {
	// Set emotion on clients, skip the server as it already has the emotion set.
	if (!HasAuthority()) {
		if (auto* Emotion = ActionMessage.Parameters.Find(TEXT("emotion"))) {
			SetEmotion(FName(*Emotion));
		}
	}

	// Spawn floating thoughts.
	if (bUseFloatingThoughts) {
		if (const auto* Thought = ActionMessage.Parameters.Find(TEXT("thoughts"))) {
			if (CurrentEmotion != NAME_None) {
				SpawnFloatingThoughts(*Thought, CurrentEmotion.ToString());
			} else {
				SpawnFloatingThoughts(*Thought, TEXT(""));
			}
		}
	}
}

void AVeAiCharacter::SayTtsMessage(const FString& InText, const ECogAiVoice InVoice) {
	if (!HasAuthority()) {
		return;
	}
	
	CurrentCogAiVoice = InVoice;
	CurrentTtsMessage = InText;

	FOnGenerateTtsUrlCompleteNative OnComplete;
	if (GetNetMode() == NM_Standalone && bStreamTTS) {
		OnComplete.BindUObject(this, &AVeAiCharacter::OnStreamDownloadCompleted);
		UVeAiBlueprintFunctionLibrary::StreamTts(this, TEXT("mp3"), TEXT("24000"), InText, InVoice, OnComplete);
	} else {
		OnComplete.BindUObject(this, &AVeAiCharacter::OnTtsUrlGenerated);
		UVeAiBlueprintFunctionLibrary::GenerateTtsUrl(this, TEXT("mp3"), TEXT("24000"), InText, InVoice, OnComplete);
	}
}

void AVeAiCharacter::OnSayTtsMessageStart_Implementation(const FString& Message) {
	if (IsRunningDedicatedServer()) {
		// Don't play any animations or use chat widgets on dedicated server.
		return;
	}

	FString Name = TEXT("Unknown");
	if (auto* NpcSettingsComponent = FindComponentByClass<UCogAiNpcSettingsComponent>(); IsValid(NpcSettingsComponent)) {
		if (auto* NamePtr = NpcSettingsComponent->PerceptibleProperties.Find(TEXT("name"))) {
			Name = *NamePtr;
		}
	}

	GET_MODULE_SUBSYSTEM(WebSocketsTextChatSubsystem, Rpc, WebSocketsTextChat);
	if (WebSocketsTextChatSubsystem) {
		WebSocketsTextChatSubsystem->ReceiveAiChatMessage(Name, Message);
	}

	bIsPlayingCustomSpeechAnimation = true;

	GetWorldTimerManager().SetTimer(MorphTargetTimerHandle, this, &AVeAiCharacter::SaySetRandomMorphTargetValue, SayMorphTargetTimerInterval, true);

	if (MetaHuman && bUseMetaHuman) {
		MetaHuman->StartSay(CurrentSpeechSoundWave, CurrentTtsMessage);
	}
}

void AVeAiCharacter::SaySetRandomMorphTargetValue() {
	SayMorphTargetValue = FMath::RandRange(SayMorphTargetMinValue, SayMorphTargetMaxValue);
}

void AVeAiCharacter::SayResetMorphTargetValue() {
	SayMorphTargetValue = 0.0f;
}

void AVeAiCharacter::SayModifyMorphTargets(const float DeltaSeconds) const {
	if (IsRunningDedicatedServer()) {
		// Don't use morph targets on dedicated server.
		return;
	}

	if (auto* MeshComponent = GetMesh(); IsValid(MeshComponent)) {
		if (const auto* SkeletalMeshAsset = MeshComponent->GetSkeletalMeshAsset(); IsValid(SkeletalMeshAsset)) {
			if (const auto* MorphTarget = SkeletalMeshAsset->FindMorphTarget(SayMorphTargetName); IsValid(MorphTarget)) {
				const float MorphTargetWeight = FMath::Lerp(MeshComponent->GetMorphTarget(SayMorphTargetName), SayMorphTargetValue, DeltaSeconds * SayMorphTargetSpeed);
				MeshComponent->SetMorphTarget(SayMorphTargetName, MorphTargetWeight);
			}
		}
	}
}

void AVeAiCharacter::OnSayTtsMessageFinish_Implementation() {
	bIsPlayingCustomSpeechAnimation = false;

	GetWorldTimerManager().ClearTimer(MorphTargetTimerHandle);

	if (HasAuthority()) {
		UAISense_Hearing::ReportNoiseEvent(this, GetActorLocation(), 1.0f, this, 0.0f, FName("SpeechFinished"));
	}

	if (MetaHuman && bUseMetaHuman) {
		MetaHuman->StopSay();
	}
}

void AVeAiCharacter::SpawnFloatingThoughts_Implementation(const FString& Thoughts, const FString& Emotion) {
	// Run on server only.
	if (IsRunningDedicatedServer()) {
		return;
	}

	// Spawn floating thoughts.
	if (FloatingThoughtClass && IsValid(GetWorld())) {
		FVector Location = FloatingThoughtsSpawnLocation->GetComponentLocation();

		if (bFloatingThoughtsUseRandomLocation) {
			// Add random offset.
			Location.X += FMath::RandRange(-FloatingThoughtsSpawnOffset, FloatingThoughtsSpawnOffset);
			Location.Y += FMath::RandRange(-FloatingThoughtsSpawnOffset, FloatingThoughtsSpawnOffset);
		}

		const auto Transform = FTransform(FRotator::ZeroRotator, Location, FloatingThoughtsSpawnLocation->GetComponentScale());

		if (auto* FloatingThoughts = GetWorld()->SpawnActor<ACogAiFloatingThought>(FloatingThoughtClass, Transform); IsValid(FloatingThoughts)) {
			FloatingThoughts->SetThoughtAndEmotion(Thoughts, Emotion);
		}
	}
}

void AVeAiCharacter::OnTtsUrlGenerated(const FString& InUrl, const FString& InErrorString) {
	// On TTS URL generated callback.

	// Run on server only.
	if (!HasAuthority()) {
		return;
	}

	if (InUrl.IsEmpty()) {
		// Request failed or URL is empty.
		VeLogErrorFunc("TTS URL request failed: %s", *InErrorString);
	} else {
		CurrentTtsUrl = InUrl;

		// Download should be done at all clients and server (including dedicated).
		const FOnDownloadProgress OnDownloadProgress;
		FOnDownloadComplete OnDownloadComplete;
		OnDownloadComplete.BindUObject(this, &AVeAiCharacter::OnDownloadCompleted);
		FileDownloadComponent->StartDownload(CurrentTtsUrl, OnDownloadProgress, OnDownloadComplete);
	}
}

void AVeAiCharacter::OnDownloadCompleted(const FDownload1ResultMetadata& Metadata) {
	if (Metadata.DownloadCode != EDownload1Code::Completed) {
		VeLogErrorFunc("Download failed: %s", *Metadata.Error);
		return;
	}

	CurrentTtsFilePath = Metadata.RequestMetadata.Path;

	// Recreate the audio importer, if the previous one has been destroyed by the garbage collector.
	if (!IsValid(AudioImporter)) {
		AudioImporter = NewObject<URuntimeAudioImporterLibrary>(this);
		AudioImporter->OnResultNative.AddUObject(this, &AVeAiCharacter::OnAudioImportComplete);
	}
	AudioImporter->ImportAudioFromFile(CurrentTtsFilePath, ERuntimeAudioFormat::Auto);
}

void AVeAiCharacter::OnStreamDownloadCompleted(const FString& Path, const FString& ErrorString) {
	CurrentTtsFilePath = Path;

	if (CurrentTtsFilePath.IsEmpty()) {
		VeLogErrorFunc("Download failed: %s", *ErrorString);
		return;
	}

	// Recreate the audio importer, if the previous one has been destroyed by the garbage collector.
	if (!IsValid(AudioImporter)) {
		AudioImporter = NewObject<URuntimeAudioImporterLibrary>(this);
		AudioImporter->OnResultNative.AddUObject(this, &AVeAiCharacter::OnAudioImportComplete);
	}
	AudioImporter->ImportAudioFromFile(CurrentTtsFilePath, ERuntimeAudioFormat::Auto);
}

void AVeAiCharacter::OnAudioImportComplete(URuntimeAudioImporterLibrary* InAudioImporter, UImportedSoundWave* InSoundWave, ERuntimeImportStatus InStatus) {
	// On audio import complete callback.

	// Check the import status.
	if (InStatus != ERuntimeImportStatus::SuccessfulImport) {
		VeLogErrorFunc("Audio import failed");
		return;
	}

	// Notify the blueprint.
	OnSayTtsMessageStart(CurrentTtsMessage);

	if (IsRunningDedicatedServer()) {
		// On the dedicated server (no audio component) just wait for the speech timer to finish and report the noise event.
		if (const auto* const World = GetWorld(); IsValid(World)) {
			SpeechTimerDelegate.BindWeakLambda(this, [=]() mutable {
				// On speech timer callback.

				// Notify the blueprint.
				OnSayTtsMessageFinish();

				// Clear the timer handle.
				SpeechTimerHandle.Invalidate();
			});

			World->GetTimerManager().SetTimer(SpeechTimerHandle, SpeechTimerDelegate, InSoundWave->GetDuration() + SpeechExtraDelay, false);
		}
	} else {
		// Else play the audio directly.

		// Wait for the audio component to finish playing.
		SayAudioFinishedHandle = AudioComponent->OnAudioFinishedNative.AddWeakLambda(this, [=](UAudioComponent* InAudioComponent) mutable {
			// On audio finished callback.

			// Remove the sound wave from the audio component.
			InAudioComponent->SetSound(nullptr);

			// Remove the delegate.
			InAudioComponent->OnAudioFinishedNative.Remove(SayAudioFinishedHandle);

			OnAudioFinished();

			SayAudioFinishedHandle.Reset();
		});

		CurrentSpeechSoundWave = InSoundWave;

		// Set the sound wave to the audio component.
		AudioComponent->SetSound(InSoundWave);

		// Play the audio.
		AudioComponent->Play();
	}
}

void AVeAiCharacter::OnAudioFinished() {
	// On audio finished callback.

	// Remove the sound wave from the audio component.
	//	InAudioComponent->SetSound(nullptr);

	// Delete the audio file.
	IFileManager::Get().Delete(*CurrentTtsFilePath);

	// Clear the file path.
	CurrentTtsFilePath.Empty();

	// Destroy the audio importer.
	AudioImporter->ConditionalBeginDestroy();
	//
	// // Remove the delegate.
	// InAudioComponent->OnAudioFinishedNative.Remove(SayAudioFinishedHandle);

	SayAudioFinishedHandle.Reset();

	ResetEmotion();

	OnSayTtsMessageFinish();
}
