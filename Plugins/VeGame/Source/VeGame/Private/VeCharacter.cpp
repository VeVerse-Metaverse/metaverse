// Author: Egor A. Pristavka. Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#include "VeCharacter.h"

#include "Api2SpeechRecognitionSubsystem.h"
#include "UIUserInfoWidget.h"
#include "UserInfoWidgetComponent.h"
#include "VePlayerStateBase.h"
#include "Components/AudioComponent.h"
#include "AudioCaptureComponent.h"
#include "AudioDevice.h"
#include "AudioMixerDevice.h"
#include "CogAiPerceptibleComponent.h"
#include "VeApi2.h"
#include "VeShared.h"
#include "Components/InteractionSubjectComponent.h"
#include "Kismet/GameplayStatics.h"
#include "VeGameModule.h"
#include "VePlayerState.h"
#include "Components/PawnNoiseEmitterComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISense_Hearing.h"
#include "Perception/AISense_Sight.h"

#if WITH_VIVOX
#include "Voice/VivoxService.h"
#endif

// Sets default values
AVeCharacter::AVeCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer) {
	PrimaryActorTick.bCanEverTick = true;

	bAlwaysRelevant = true;

#if 0 // breaks ALS, sprinting staggers when using controller rotation yaw
	// bUseControllerRotationPitch = false;
	// bUseControllerRotationYaw = true;
	// bUseControllerRotationRoll = false;
#endif

	UserInfoWidgetComponent = CreateDefaultSubobject<UUserInfoWidgetComponent>("UserInfoWidgetComponent");
	UserInfoWidgetComponent->SetupAttachment(RootComponent);
	UserInfoWidgetComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 120.0f));

	AIPerceptionStimuliSourceComponent = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>("AIPerceptionStimuliSourceComponent");
	CogAiPerceptibleComponent = CreateDefaultSubobject<UCogAiPerceptibleComponent>("CogAiPerceptibleComponent");
}

void AVeCharacter::RegisterCallbacks() {
#if WITH_VIVOX
	if (const auto VivoxService = GetGameInstance()->GetSubsystem<UVivoxService>()) {
		VivoxService->OnVivoxVoiceStateChanged.AddUObject(this, &AVeCharacter::OnVivoxVoiceStateChangedCallback);
	}
#endif
}

void AVeCharacter::UnregisterCallbacks() {
#if WITH_VIVOX
	if (const auto VivoxService = GetGameInstance()->GetSubsystem<UVivoxService>()) {
		VivoxService->OnVivoxVoiceStateChanged.RemoveAll(this);
	}
#endif
}

// Called when the game starts or when spawned
void AVeCharacter::BeginPlay() {
	RegisterCallbacks();

	if (IsValid(AIPerceptionStimuliSourceComponent)) {
		AIPerceptionStimuliSourceComponent->RegisterForSense(UAISense_Sight::StaticClass());
		AIPerceptionStimuliSourceComponent->RegisterForSense(UAISense_Hearing::StaticClass());
	} else {
		VeLogErrorFunc("AIPerceptionStimuliSourceComponent is invalid");
	}

	const auto* VePlayerState = GetPlayerState<AVePlayerStateBase>();

	if (IsValid(VePlayerState)) {
		if (IsValid(CogAiPerceptibleComponent)) {
			CogAiPerceptibleComponent->SetName(VePlayerState->GetUserMetadata().Name);
			CogAiPerceptibleComponent->SetDescription(VePlayerState->GetUserMetadata().Description);
			CogAiPerceptibleComponent->SetInspectDescription(VePlayerState->GetUserMetadata().Description);
		}
	}

	Super::BeginPlay();
}

void AVeCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason) {
	Super::EndPlay(EndPlayReason);

	if (IsValid(AIPerceptionStimuliSourceComponent)) {
		AIPerceptionStimuliSourceComponent->UnregisterFromSense(UAISense_Sight::StaticClass());
		AIPerceptionStimuliSourceComponent->UnregisterFromSense(UAISense_Hearing::StaticClass());
	} else {
		VeLogErrorFunc("AIPerceptionStimuliSourceComponent is invalid");
	}

	UnregisterCallbacks();
}

void AVeCharacter::NativeOnPlayerStateChanged(APlayerState* InPlayerState) {
	Super::NativeOnPlayerStateChanged(InPlayerState);

	auto* VePlayerState = GetPlayerState<AVePlayerStateBase>();

	if (IsValid(CogAiPerceptibleComponent)) {
		if (IsValid(VePlayerState)) {
			CogAiPerceptibleComponent->SetName(VePlayerState->GetUserMetadata().Name);
			CogAiPerceptibleComponent->SetDescription(VePlayerState->GetUserMetadata().Description);
			CogAiPerceptibleComponent->SetInspectDescription(VePlayerState->GetUserMetadata().Description);
		}
	}

	if (GetNetMode() != NM_DedicatedServer) {
		if (IsValid(UserInfoWidgetComponent)) {
			UserInfoWidgetComponent->SetPlayerState(InPlayerState);
		}
	}
}

// ReSharper disable once CppMemberFunctionMayBeConst
void AVeCharacter::OnVivoxVoiceStateChangedCallback(const FGuid& /*UserId*/, const bool bIsSpeaking) {
	auto* VePlayerState = GetPlayerState<AVePlayerStateBase>();
	if (IsValid(VePlayerState)) {
		VePlayerState->Authority_SetIsSpeaking(bIsSpeaking);
	}
}
