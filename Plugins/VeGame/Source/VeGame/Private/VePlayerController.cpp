// Author: Egor A. Pristavka. Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#include "VePlayerController.h"

#include "EditorComponent.h"
#include "PlaceableComponent.h"
#include "GameFramework/GameModeBase.h"
#include "Kismet/KismetSystemLibrary.h"
#include "VeShared.h"
#include "VeHUD.h"
#include "VeApi2.h"
#include "Api2AuthSubsystem.h"
#include "Api2SpeechRecognitionSubsystem.h"
#include "AudioCaptureComponent.h"
#include "AudioDevice.h"
#include "AudioMixerDevice.h"
#include "VeGameStateBase.h"
#include "VePlayerStateBase.h"
#include "Actors/VRHUD.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Character/ALSBaseCharacter.h"
#include "Character/ALSPlayerCameraManager.h"
#include "Components/ALSDebugComponent.h"
#include "Net/UnrealNetwork.h"
#include "VeGameFramework.h"
#include "Structs/PlaceableAssetMetadata.h"
#include "VeGameModule.h"
#include "Inspector/InspectComponent.h"
#include "Inspector/UIInspectorRootWidget.h"
#include "GameFrameworkWorldItem.h"
#include "GameFrameworkWorldSubsystem.h"
#include "EnhancedInputComponent.h"
#include "UIRootWidget.h"
#include "VeGameState.h"
#include "Components/InteractionSubjectComponent.h"
#include "Pawns/InteractiveCharacter.h"
#include "VeCharacter.h"
#include "VeCharacterBase.h"
#include "VeLiveLinkComponent.h"
#include "VePawnBase.h"
#include "VePlayerState.h"
#include "Perception/AISense_Hearing.h"
#include "VeWorldSettings.h"

#if WITH_VIVOX
#include "Voice/VivoxService.h"
#endif


namespace Options {
	const FName InputPinMenu = TEXT("PinVRMenu");
	const FName InputVoicePushToTalk = TEXT("VoicePushToTalk");
	const FName InputVoiceToggleSpeaking = TEXT("VoiceToggleSpeaking");
	const FName InputVoiceSwitchToEcho = TEXT("VoiceSwitchToEcho");
	const FName Input_InspectResetView = TEXT("InspectResetView");
}

// Sets default values
AVePlayerController::AVePlayerController()
	: Super() {
	PrimaryActorTick.bCanEverTick = true;

	EditorComponent = CreateDefaultSubobject<UEditorComponent>(TEXT("EditorComponent"));
	InspectComponent = CreateDefaultSubobject<UInspectComponent>(TEXT("InspectComponent"));

#pragma region Voice Capture and Speech Recognition

	VoiceFeedbackAudioComponent = CreateDefaultSubobject<UAudioComponent>("AudioComponent");
	VoiceFeedbackAudioComponent->SetupAttachment(RootComponent);
	VoiceFeedbackAudioComponent->bAutoActivate = false;

	AudioCaptureComponent = CreateDefaultSubobject<UAudioCaptureComponent>(TEXT("AudioCaptureComponent"));
	if (IsValid(AudioCaptureComponent)) {
		AudioCaptureComponent->SetupAttachment(GetRootComponent());
		AudioCaptureComponent->SetAutoActivate(false);
	}

	// Get the output directory for the project.
	VoiceCaptureOutputDir = FPaths::ProjectSavedDir() / TEXT("VoiceCapture");
	VoiceCaptureOutputFileName = TEXT("VoiceCapture.wav");

	// InteractionSubjectComponent->OnServerInputActionNative.AddUObject(this, &AVePlayerController::OnServerInputActionCallback);

#pragma endregion
}

void AVePlayerController::Tick(const float DeltaTime) {
	Super::Tick(DeltaTime);
}

void AVePlayerController::OnPossess(APawn* NewPawn) {
	Super::OnPossess(NewPawn);

	if (IsValid(NewPawn) && NewPawn != EditorPawn) {
		MainPawn = NewPawn;
	}

	PossessedCharacter = Cast<AALSBaseCharacter>(NewPawn);
	if (!IsRunningDedicatedServer()) {
		// Servers want to setup camera only in listen servers.
		SetupCamera();
	}

	SetupInputs();
	if (IsValid(PossessedCharacter)) {
		auto* DebugComp = Cast<UALSDebugComponent>(PossessedCharacter->GetComponentByClass(UALSDebugComponent::StaticClass()));
		if (IsValid(DebugComp)) {
			DebugComp->OnPlayerControllerInitialized(this);
		}
	}
}

void AVePlayerController::OnRep_Pawn() {
	Super::OnRep_Pawn();

	if (!IsValid(GetPawn())) {
		return;
	}

	if (EditorPawnClass == GetPawn()->GetClass()) {
		// editor pawn
		// Client_EnableEditMode_Implementation();
	} else {
		SetupCamera();
		SetupInputs();
		PossessedCharacter = Cast<AALSBaseCharacter>(GetPawn());
		if (IsValid(PossessedCharacter)) {
			auto* DebugComp = Cast<UALSDebugComponent>(PossessedCharacter->GetComponentByClass(UALSDebugComponent::StaticClass()));
			if (IsValid(DebugComp)) {
				DebugComp->OnPlayerControllerInitialized(this);
			}
		}
	}
}

void AVePlayerController::NativeOnPawnChanged(APawn* NewPawn, APawn* PrevPawn) {
	if (PrevPawn) {
		if (auto* InteractiveCharacter = Cast<AInteractiveCharacter>(PrevPawn)) {
			InteractiveCharacter->SetPlayerController(nullptr);
		}

		if (IsLocalPlayerController()) {
			if (auto* LiveLinkComponent = PrevPawn->FindComponentByClass<UVeLiveLinkComponent>()) {
				LiveLinkComponent->SetEnableInput(false);
			}
		}
	}

	if (NewPawn) {
		if (auto* InteractiveCharacter = Cast<AInteractiveCharacter>(NewPawn)) {
			InteractiveCharacter->SetPlayerController(this);
		}

		// Show the pawn HUD
		if (auto* UIHUD = GetHUD<AVeHUD>()) {
			if (auto* VeCharacter = Cast<AVeCharacter>(NewPawn)) {
				UIHUD->ShowHUDWidget(VeCharacter->HUDWidgetClass);
			} else if (auto* VeCharacterBase = Cast<AVeCharacterBase>(NewPawn)) {
				UIHUD->ShowHUDWidget(VeCharacterBase->HUDWidgetClass);
			} else if (auto* VePawnBase = Cast<AVePawnBase>(NewPawn)) {
				UIHUD->ShowHUDWidget(VePawnBase->HUDWidgetClass);
			} else {
				UIHUD->ShowHUDWidget(nullptr);
			}
		}

		if (IsLocalPlayerController()) {
			if (auto* LiveLinkComponent = NewPawn->FindComponentByClass<UVeLiveLinkComponent>()) {
				LiveLinkComponent->SetEnableInput(true);
			}
		}
	}

	Super::NativeOnPawnChanged(NewPawn, PrevPawn);
}

void AVePlayerController::Input_InspectOrbitAxis(const FVector Vector) {
	if (InspectComponent) {
		if (InspectComponent->bPanning) {
			InspectComponent->PanUpdate(Vector.X, Vector.Y);
		}

		if (InspectComponent->bOrbiting) {
			InspectComponent->OrbitUpdate(Vector.X, Vector.Y);
		}
	}
}

void AVePlayerController::Input_InspectOrbitYaw(float X) {
	OrbitAxes.X = GetInputAxisValue(TEXT("InspectOrbitYaw"));
	OrbitAxes.Y = GetInputAxisValue(TEXT("InspectOrbitPitch"));
	Input_InspectOrbitAxis(OrbitAxes);
}

void AVePlayerController::Input_InspectOrbitPitch(float Y) {
	OrbitAxes.X = GetInputAxisValue(TEXT("InspectOrbitYaw"));
	OrbitAxes.Y = GetInputAxisValue(TEXT("InspectOrbitPitch"));
	Input_InspectOrbitAxis(OrbitAxes);
}

void AVePlayerController::Input_InspectWheelAxis(float ZoomValue) {
	if (ZoomValue != 0) {
		if (InspectComponent) {
			InspectComponent->Zoom(ZoomValue);
		}
	}
}

void AVePlayerController::Input_InspectResetView() {
	if (InspectComponent) {
		InspectComponent->ResetView();
	}
}

void AVePlayerController::SetupInputComponent() {
	Super::SetupInputComponent();

	if (!InputComponent) {
		LogErrorF("failed to get the input component for the player controller");
		return;
	}

	// InputComponent->BindAction(Options::InputToggleMenu, IE_Pressed, this, &AVePlayerController::Input_ToggleMenu);
	// InputComponent->BindAction(Options::InputToggleTextChat, IE_Pressed, this, &AVePlayerController::Input_ToggleTextChat);
	InputComponent->BindAction(Options::InputPinMenu, IE_Pressed, this, &AVePlayerController::Input_PinVRMenu);

#if WITH_VIVOX
	{
		auto& Binding = InputComponent->BindAction(Options::InputVoicePushToTalk, IE_Pressed, this, &AVePlayerController::Input_StartSpeaking);
		Binding.bConsumeInput = false;
	}
	{
		auto& Binding = InputComponent->BindAction(Options::InputVoicePushToTalk, IE_Released, this, &AVePlayerController::Input_StopSpeaking);
		Binding.bConsumeInput = false;
	}
	InputComponent->BindAction(Options::InputVoiceToggleSpeaking, IE_Pressed, this, &AVePlayerController::Input_ToggleSpeaking);
	InputComponent->BindAction(Options::InputVoiceSwitchToEcho, IE_Pressed, this, &AVePlayerController::Input_SwitchToEchoChannel);
#endif

	InputComponent->BindAction("CurrentWorldInfo", IE_Pressed, this, &AVePlayerController::Input_CurrentWorldInfo);

	// InspectorMode
	{
		// InputComponent->BindAction(Options::InputToggleInspectorMode, IE_Pressed, this, &AVePlayerController::Input_ToggleInspectMode);
		{
			auto& AxisBinding = InputComponent->BindAxis("InspectOrbitYaw", this, &AVePlayerController::Input_InspectOrbitYaw);
			AxisBinding.bConsumeInput = false;
		}
		{
			auto& AxisBinding = InputComponent->BindAxis("InspectOrbitPitch", this, &AVePlayerController::Input_InspectOrbitPitch);
			AxisBinding.bConsumeInput = false;
		}

		InputComponent->BindAxis("InspectWheelAxis", this, &AVePlayerController::Input_InspectWheelAxis);
		InputComponent->BindAction(Options::Input_InspectResetView, IE_Pressed, this, &AVePlayerController::Input_InspectResetView);

		{
			auto& ActionBinding = InputComponent->BindAction("Inspect Orbit", IE_Pressed, this, &AVePlayerController::Input_InspectOrbitPressed);
			ActionBinding.bConsumeInput = false;
		}
		{
			auto& ActionBinding = InputComponent->BindAction("Inspect Orbit", IE_Released, this, &AVePlayerController::Input_InspectOrbitReleased);
			ActionBinding.bConsumeInput = false;
		}

		InputComponent->BindAction("Inspect Pan", IE_Pressed, this, &AVePlayerController::Input_InspectPanPressed);
		InputComponent->BindAction("Inspect Pan", IE_Released, this, &AVePlayerController::Input_InspectPanReleased);
	}

	// EditorMode
	{
		// InputComponent->BindAction(Options::InputToggleEditorMode, IE_Pressed, this, &AVePlayerController::Input_ToggleEditMode);

		InputComponent->BindAction("Clone Toggle", IE_Pressed, this, &AVePlayerController::Input_CloningPressed);
		InputComponent->BindAction("Clone Toggle", IE_Released, this, &AVePlayerController::Input_CloningReleased);
		InputComponent->BindAction("MultiSelect", IE_Pressed, this, &AVePlayerController::Input_MultiSelectPressed);
		InputComponent->BindAction("MultiSelect", IE_Released, this, &AVePlayerController::Input_MultiSelectReleased);

		InputComponent->BindAction("Destroy All", IE_Pressed, this, &AVePlayerController::Input_DestroyAll);
		InputComponent->BindAction("Deselect All", IE_Pressed, this, &AVePlayerController::Input_DeselectAll);

		{
			auto& ActionBinding = InputComponent->BindAction("Select", IE_Pressed, this, &AVePlayerController::Input_SelectPressed);
			ActionBinding.bConsumeInput = false;
		}
		{
			auto& ActionBinding = InputComponent->BindAction("Select", IE_Released, this, &AVePlayerController::Input_SelectReleased);
			ActionBinding.bConsumeInput = false;
		}
	}

#pragma region Voice Capture and Speech Recognition
	// Todo: Use enhanced input system
	{
		auto& Binding = InputComponent->BindAction("VoiceCapture", IE_Pressed, this, &AVePlayerController::Input_VoiceCapturePressed);
		Binding.bConsumeInput = false;
	}
	{
		auto& Binding = InputComponent->BindAction("VoiceCapture", IE_Released, this, &AVePlayerController::Input_VoiceCaptureReleased);
		Binding.bConsumeInput = false;
	}
#pragma endregion
}

void AVePlayerController::OnVivoxVoiceStateChanged(const FGuid& InUserId, const bool bInVoiceDetected) {
	if (InUserId != GetUserId()) {
		return;
	}

	// Replicate voice state to the server.
	if (IsLocalController()) {
		ServerRPC_SetIsSpeaking(bInVoiceDetected);

		if (const auto* HUD = Cast<AVeHUD>(GetHUD())) {
			HUD->SetVoiceState(bInVoiceDetected);
		}
	}
}

// ReSharper disable once CppMemberFunctionMayBeConst
void AVePlayerController::OnVivoxVoiceServerIdChanged(const FGuid& InServerId) {
#if WITH_VIVOX
	if (const auto* GameInstance = GetGameInstance()) {
		if (auto* VivoxService = GameInstance->GetSubsystem<UVivoxService>()) {
			if (VivoxService->GetIsConnectedToPositionalChannel()) {
				VivoxService->LeaveServer();
			}

			// Always join the server, even if it's the default one.
			if (InServerId.IsValid()) {
				VivoxService->JoinServer(InServerId);
			} else if (FVeConfigLibrary::GetAllowDefaultVoiceServer()) {
				VivoxService->JoinServer(FVeConfigLibrary::GetDefaultVoiceServerId());
			}
		}
	}
#endif
}

void AVePlayerController::ServerRPC_SetIsSpeaking_Implementation(const bool bInIsSpeaking) {
	auto* VePlayerState = GetPlayerState<AVePlayerStateBase>();
	if (IsValid(VePlayerState)) {
		VePlayerState->Authority_SetIsSpeaking(bInIsSpeaking);
	}
}

void AVePlayerController::PostInitializeComponents() {
	Super::PostInitializeComponents();

	if (IsLocalPlayerController()) {
		GET_MODULE_SUBSYSTEM(EditorSubsystem, GameFramework, Editor);
		if (EditorSubsystem) {
			EditorSubsystem->SetIsEditorModeEnabled(false);
		}
	}

	if (GetNetMode() != NM_DedicatedServer) {
		GET_MODULE_SUBSYSTEM(AuthSubsystem, Api2, Auth);
		if (AuthSubsystem) {
			auto OnChangeAuthorizationStateCallback = [=](EAuthorizationState AuthorizationState) {
				if (AuthorizationState == EAuthorizationState::LogoutSuccess) {
					const FString HomeUrl = FVeConfigLibrary::GetStartupMap();
					ClientTravel(HomeUrl, TRAVEL_Absolute);
				}
			};

			AuthSubsystem->GetOnChangeAuthorizationState().AddWeakLambda(this, OnChangeAuthorizationStateCallback);
		}

#if WITH_VIVOX
		const auto* MyWorld = GetWorld();
		if (MyWorld && !MyWorld->IsEditorWorld()) {
			if (const auto VivoxService = GetGameInstance()->GetSubsystem<UVivoxService>()) {
				if (!OnVivoxVoiceStateChangedDelegateHandle.IsValid()) {
					OnVivoxVoiceStateChangedDelegateHandle = VivoxService->OnVivoxVoiceStateChanged.AddUObject(this, &AVePlayerController::OnVivoxVoiceStateChanged);
				}
			}
		}
#endif
	}
}

void AVePlayerController::NativeOnGameStateChanged(AGameStateBase* InGameState) {
	Super::NativeOnGameStateChanged(InGameState);

#if WITH_VIVOX
	if (auto* VeWorldSettings = Cast<AVeWorldSettings>(GetWorldSettings())) {
		auto OnServerId = AVeWorldSettings::FServerIdDelegate::CreateUObject(this, &AVePlayerController::OnVivoxVoiceServerIdChanged);
		VeWorldSettings->OnReady_ServerId(OnServerId);
	}
#endif
}

void AVePlayerController::BeginPlay() {
	// GET_MODULE_SUBSYSTEM(PlaceableSubsystem, Api, Placeable);
	// if (PlaceableSubsystem) {
	// 	FApiPlaceableClassBatchQueryRequestMetadata RequestMetadata;
	// 	RequestMetadata.Offset = 0;
	// 	RequestMetadata.Limit = 10;
	// 	const auto Callback = MakeShared<FOnPlaceableClassBatchRequestCompleted>();
	// 	Callback->BindLambda([=](const FApiPlaceableClassBatchMetadata& InMetadata, const bool bInSuccessful, const FString& InError) {
	// 		VeLog("Result: %d, %s", bInSuccessful, *InError);
	// 	});
	// 	PlaceableSubsystem->GetPlaceableClassBatch(RequestMetadata, Callback);
	// }

	if (auto* State = GetPlayerState<AVePlayerStateBase>(); IsValid(State)) {
		SpeechRecognizedEvent = State->SpeechRecognizedEvent.AddUObject(this, &AVePlayerController::OnSpeechRecognized);
	}

	Super::BeginPlay();
}

void AVePlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason) {
	Super::EndPlay(EndPlayReason);

	// Unregister speech recognition event.
	if (auto* State = GetPlayerState<AVePlayerStateBase>(); IsValid(State)) {
		State->SpeechRecognizedEvent.Remove(SpeechRecognizedEvent);
	}

	// If EditorMode (spectator pawn possessed)
	if (GetPawn() != MainPawn && IsValid(MainPawn)) {
		MainPawn->Destroy();
	}

	if (!IsRunningDedicatedServer()) {
		GET_MODULE_SUBSYSTEM(AuthSubsystem, Api2, Auth);
		if (AuthSubsystem) {
			AuthSubsystem->GetOnChangeAuthorizationState().RemoveAll(this);
		}

#if WITH_VIVOX
		// Stop listening to voice state change events.
		if (const auto VivoxService = GetGameInstance()->GetSubsystem<UVivoxService>()) {
			if (OnVivoxVoiceStateChangedDelegateHandle.IsValid()) {
				VivoxService->OnVivoxVoiceStateChanged.Remove(OnVivoxVoiceStateChangedDelegateHandle);
			}
		}

		OnVivoxVoiceServerIdChanged(FGuid());
#endif
	}
}

void AVePlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void AVePlayerController::Authority_OnLogout() {
#if WITH_VIVOX
	OnVivoxVoiceServerIdChanged(FGuid());
#endif
}

void AVePlayerController::SetAppMode(EAppMode InAppMode) {
	switch (InAppMode) {
	case EAppMode::Inspector: {
		if (!Input_EnableInspectMode()) {
			return;
		}
		break;
	}
	case EAppMode::Editor: {
		if (!Input_ToggleEditMode()) {
			return;
		}
		break;
	}
	case EAppMode::Menu: {
		if (!Input_ShowMenu()) {
			return;
		}
		BeforeMenuMode = GetAppMode();
		break;
	}
	case EAppMode::TextChat: {
		if (!Input_ShowTextChat()) {
			return;
		}
		break;
	}
	case EAppMode::Game: {
		if (GetAppMode() == EAppMode::Menu) {
			InAppMode = BeforeMenuMode;
			break;
		}
		if (GetAppMode() == EAppMode::Inspector) {
			if (!Input_DisableInspectMode()) {
				return;
			}
		}
		if (GetAppMode() == EAppMode::Editor) {
			if (!Input_ToggleEditMode()) {
				return;
			}
		}
		break;
	}

	default: {
		break;
	}
	}

	Super::SetAppMode(InAppMode);
}

void AVePlayerController::ServerRPC_SpawnActor_Implementation(const TSubclassOf<AActor> Class, FVector Location, FRotator Rotation) {
	const auto World = GetWorld();
	if (!World) {
		return;
	}

	FActorSpawnParameters Parameters;
	Parameters.Owner = this;

	auto* Actor = World->SpawnActor(Class.Get(), &Location, &Rotation, Parameters);

	OnActorSpawn.Broadcast(Actor);
}

FGuid AVePlayerController::GetUserId() const {
	const auto* VePlayerState = GetPlayerState<AVePlayerStateBase>();
	if (IsValid(VePlayerState)) {
		return VePlayerState->GetUserId();
	}
	return {};
}

void AVePlayerController::PlayerTick(const float DeltaTime) {
	Super::PlayerTick(DeltaTime);

#if WITH_VIVOX
	const auto GameInstance = GetGameInstance();
	if (!GameInstance) {
		return;
	}

	const auto VivoxService = GameInstance->GetSubsystem<UVivoxService>();
	if (!VivoxService) {
		return;
	}

	VivoxService->Update3DPosition(GetPawnOrSpectator());
#endif
}

void AVePlayerController::SwitchToEchoChannel() {
#if WITH_VIVOX
	const auto GameInstance = GetGameInstance();
	if (!GameInstance) {
		return;
	}
	const auto VivoxService = GameInstance->GetSubsystem<UVivoxService>();
	if (!VivoxService) {
		return;
	}

	VivoxService->Exec_ConnectToEchoChannel(TArray<FString>{}, GetWorld());
	if (!IsValid(this)) {
		if (const UWorld* const World = GetWorld()) {
			FTimerHandle __tempTimerHandle;
			World->GetTimerManager().SetTimer(__tempTimerHandle, FTimerDelegate().CreateWeakLambda(this, [=]() {
				VivoxService->Exec_ChangeActiveChannel(TArray<FString>{TEXT("e")}, GetWorld());
				LogVerboseF("Vivox: Switched to echo channel.");
			}), 3, false);
		}
	}
#endif
}

void AVePlayerController::SwitchSpeakingState() {
#if WITH_VIVOX
	const auto GameInstance = GetGameInstance();
	if (!GameInstance) {
		return;
	}

	if (bVoiceEnabled) {
		LogWarningF("stop speaking");
		StopSpeaking();
	} else {
		LogWarningF("start speaking");
		StartSpeaking();
	}
#endif
}

void AVePlayerController::StartSpeaking() {
#if WITH_VIVOX
	LogF("Vivox: Start speaking");
	bVoiceEnabled = true;

	const auto GameInstance = GetGameInstance();
	if (!GameInstance) {
		return;
	}

	if (!IsRunningDedicatedServer()) {
		const auto* HUD = Cast<AVeHUD>(GetHUD());
		if (IsValid(HUD)) {
			HUD->SetVoiceState(true);
		}

		const auto VivoxService = GameInstance->GetSubsystem<UVivoxService>();
		if (!VivoxService) {
			return;
		}

		const auto Status = VivoxService->StartSpeaking(SelectedVoiceChannel);
		if (Status == VxErrorSuccess) {
			if (IsLocalController()) {
				LogWarningF("should replicate speaking state to server");
				// Server_UpdateSpeakingState(true);
			}
		} else {
			LogWarningF("Vivox: Failed to update speaking state {%d}.", Status);
		}
	}
#endif
}

void AVePlayerController::StopSpeaking() {
#if WITH_VIVOX
	LogF("Vivox: Stop speaking");
	bVoiceEnabled = false;

	if (IsLocalController()) {
		LogWarningF("should replicate speaking state to server");
		// Server_UpdateSpeakingState(false);
	}

	const auto GameInstance = GetGameInstance();
	if (!GameInstance) {
		return;
	}

	if (!IsRunningDedicatedServer()) {
		const auto* HUD = Cast<AVeHUD>(GetHUD());
		if (IsValid(HUD)) {
			HUD->SetVoiceState(false);
		}

		const auto VivoxService = GameInstance->GetSubsystem<UVivoxService>();
		if (!VivoxService) {
			return;
		}

		const auto Status = VivoxService->StopSpeaking();
		if (Status != VxErrorSuccess) {
			LogWarningF("Vivox: Failed to update speaking state {%d}.", Status);
		}
	}
#endif
}

// ReSharper disable once CppMemberFunctionMayBeConst
void AVePlayerController::Input_PinVRMenu() {
	if (InteractionMode != EUIInteractionMode::VR) {
		return;
	}

	if (!MyVRHUD) {
		return;
	}

	MyVRHUD->SetIsPinned(!MyVRHUD->GetIsPinned());
}

// ReSharper disable once CppMemberFunctionMayBeConst
void AVePlayerController::Input_StartSpeaking() {
	StartSpeaking();
}

// ReSharper disable once CppMemberFunctionMayBeConst
void AVePlayerController::Input_StopSpeaking() {
	StopSpeaking();
}

// ReSharper disable once CppMemberFunctionMayBeConst
void AVePlayerController::Input_ToggleSpeaking() {}

// ReSharper disable once CppMemberFunctionMayBeConst
void AVePlayerController::Input_SwitchToEchoChannel() {}

void AVePlayerController::SetupCamera() const {
	// Call "OnPossess" in Player Camera Manager when possessing a pawn
	auto* CastedMgr = Cast<AALSPlayerCameraManager>(PlayerCameraManager);
	if (IsValid(PossessedCharacter) && IsValid(CastedMgr)) {
		CastedMgr->OnPossess(PossessedCharacter);
	}
}

void AVePlayerController::SetupInputs() {}

#pragma region EditorMode

bool AVePlayerController::Input_ToggleEditMode() {
	Server_ToggleEditMode();
	return true;
}

void AVePlayerController::Server_ToggleEditMode_Implementation() {
	if (IsValid(EditorPawn)) {
		Authority_DisableEditMode();
	} else {
		//No editor pawn
		Authority_EnableEditMode();
	}
}

void AVePlayerController::Authority_EnableEditMode() {

	FActorSpawnParameters SpawnInfo;
	SpawnInfo.Instigator = GetInstigator();
	SpawnInfo.ObjectFlags |= RF_Transient;

	auto ActorLocation = MainPawn->GetActorLocation();
	auto ActorRotation = MainPawn->GetActorRotation();
	auto SpawnTransform = FTransform();
	ActorLocation.Z = ActorLocation.Z + MainPawn->GetDefaultHalfHeight() * 2 + OffsetZ;
	ActorLocation += MainPawn->GetActorForwardVector() * OffsetF;
	SpawnTransform.SetLocation(ActorLocation);
	SpawnTransform.SetRotation(ActorRotation.Quaternion());
	SpawnTransform.SetScale3D(FVector(1.f, 1.f, 1.f)); // ActorLocation * -100.f;

	EditorPawn = GetWorld()->SpawnActor<APawn>(EditorPawnClass, SpawnTransform, SpawnInfo);
	if (!IsValid(EditorPawn)) {
		UE_LOG(LogGameMode, Warning, TEXT("SpawnDefaultPawnAtTransform: Couldn't spawn Pawn of type %s at %s"), *GetNameSafe(EditorPawn), *SpawnTransform.ToHumanReadableString());
	} else {
		Possess(EditorPawn);
		Client_EnableEditMode();
	}

}

void AVePlayerController::Authority_DisableEditMode() {
	if (IsValid(MainPawn)) {
		Possess(MainPawn);
	}
	EditorPawn->Destroy();
	Client_DisableEditMode();
}

void AVePlayerController::Client_EnableEditMode_Implementation() {
	if (!IsLocalPlayerController() || bEditorMode) {
		return;
	}

	bEditorMode = true;

	GET_MODULE_SUBSYSTEM(EditorSubsystem, GameFramework, Editor);
	if (EditorSubsystem) {
		EditorSubsystem->SetIsEditorModeEnabled(true);
	}

	auto* HudActor = Cast<AVeHUD>(GetHUD());
	if (HudActor) {
		HudActor->ShowEditor();
	}
}

void AVePlayerController::Client_DisableEditMode_Implementation() {
	if (!IsLocalPlayerController() || !bEditorMode) {
		return;
	}

	bEditorMode = false;

	GET_MODULE_SUBSYSTEM(EditorSubsystem, GameFramework, Editor);
	if (EditorSubsystem) {
		EditorSubsystem->SetIsEditorModeEnabled(false);
	}

	auto* HudActor = Cast<AVeHUD>(GetHUD());
	if (HudActor) {
		HudActor->HideEditor();
		EditorComponent->DeselectAll();
	}
}

void AVePlayerController::Input_CloningPressed() {
	if (!IsLocalPlayerController() || !bEditorMode) {
		return;
	}

	bCloning = true;
}

void AVePlayerController::Input_CloningReleased() {
	if (!IsLocalPlayerController() || !bEditorMode) {
		return;
	}

	bCloning = false;
}

void AVePlayerController::Input_MultiSelectPressed() {
	if (!IsLocalPlayerController() || !bEditorMode) {
		return;
	}

	bMultiSelect = true;
}

void AVePlayerController::Input_MultiSelectReleased() {
	if (!IsLocalPlayerController() || !bEditorMode) {
		return;
	}

	bMultiSelect = false;
}

void AVePlayerController::Input_DeselectAll() {
	if (!IsLocalPlayerController() || !bEditorMode) {
		return;
	}

	// todo: replication
	if (EditorComponent) {
		EditorComponent->DeselectAll();
	}
}

void AVePlayerController::Input_DestroyAll() {
	if (!IsLocalPlayerController() || !bEditorMode) {
		return;
	}

	// todo: replication
	if (EditorComponent) {
		EditorComponent->DestroySelected();
	}
}

void AVePlayerController::Input_SelectPressed() {
	if (!IsLocalPlayerController() || !bEditorMode) {
		return;
	}

	if (EditorComponent) {
		TArray<TEnumAsByte<ECollisionChannel>> CollisionChannels = {ECC_WorldStatic, ECC_WorldDynamic, ECC_Pawn};
		EditorComponent->Owner_MouseTraceByObjectTypes(CollisionChannels, {}, bMultiSelect);
	}
}

void AVePlayerController::Input_SelectReleased() {
	if (!IsLocalPlayerController() || !bEditorMode) {
		return;
	}

	if (EditorComponent) {
		EditorComponent->Owner_FinishTransform();
	}
}

#pragma endregion EditorMode

#pragma region InspectMode

bool AVePlayerController::Input_EnableInspectMode() {
	auto* HUDActor = GetHUD<AVeHUD>();
	if (!HUDActor) {
		return false;
	}

	auto* InspectorRootWidget = HUDActor->GetInspectorRootWidget();
	if (!InspectorRootWidget) {
		return false;
	}

	if (auto* InspectActor = InspectComponent->BeginInspect()) {
		if (InspectorRootWidget->StartInspection(InspectActor)) {
			HUDActor->ShowInspector();
			return true;
		}
	}

	return false;
}

bool AVePlayerController::Input_DisableInspectMode() {
	auto* HUDActor = GetHUD<AVeHUD>();
	if (!HUDActor) {
		return false;
	}

	auto* InspectorRootWidget = HUDActor->GetInspectorRootWidget();
	if (!InspectorRootWidget) {
		return false;
	}

	HUDActor->HideInspector();
	InspectComponent->EndInspect();
	return true;
}

void AVePlayerController::Input_InspectPanPressed() {
	if (bEditorMode) {
		return;
	}

	if (!InspectComponent) {
		VeLogErrorFunc("no inspect component");
		return;
	}

	InspectComponent->OnPanBegin();
}

void AVePlayerController::Input_InspectPanReleased() {
	if (bEditorMode) {
		return;
	}

	if (!InspectComponent) {
		VeLogErrorFunc("no inspect component");
		return;
	}

	InspectComponent->OnPanEnd();
}

void AVePlayerController::Input_InspectOrbitPressed() {
	if (bEditorMode) {
		return;
	}

	if (!InspectComponent) {
		VeLogErrorFunc("no inspect component");
		return;
	}

	InspectComponent->OnOrbitBegin();
}

void AVePlayerController::Input_InspectOrbitReleased() {
	if (bEditorMode) {
		return;
	}

	if (!InspectComponent) {
		VeLogErrorFunc("no inspect component");
		return;
	}

	InspectComponent->OnOrbitEnd();
}

#pragma endregion InspectMode

#pragma region Utility

void AVePlayerController::OpenWorldOnline(const FString& InSpaceId) {
	FGuid WorldId(InSpaceId);
	if (!WorldId.IsValid()) {
		return;
	}

	GET_MODULE_SUBSYSTEM(WorldSubsystem, GameFramework, World);
	if (WorldSubsystem) {
		const auto RefreshCallback = MakeShared<FOnGameFrameworkWorldItemCreated>();
		RefreshCallback->BindWeakLambda(this, [=](TSharedRef<FGameFrameworkWorldItem> InWorldItem, const bool bIsValid, const FString& InError) {
			if (bIsValid && InWorldItem->CanStartTeleport()) {
				InWorldItem->StartTeleportOnline(MakeShared<FOnOpenWorldState>(), this);
			}
		});
		WorldSubsystem->GetItem(WorldId, RefreshCallback);
	}
}

void AVePlayerController::OpenWorldOffline(const FString& InSpaceId) {
	FGuid WorldId(InSpaceId);
	if (!WorldId.IsValid()) {
		return;
	}

	GET_MODULE_SUBSYSTEM(WorldSubsystem, GameFramework, World);
	if (WorldSubsystem) {
		const auto RefreshCallback = MakeShared<FOnGameFrameworkWorldItemCreated>();
		RefreshCallback->BindWeakLambda(this, [=](TSharedRef<FGameFrameworkWorldItem> InWorldItem, const bool bIsValid, const FString& InError) {
			if (bIsValid && InWorldItem->CanStartTeleport()) {
				InWorldItem->StartTeleportOffline(MakeShared<FOnOpenWorldState>(), this);
			}
		});
		WorldSubsystem->GetItem(WorldId, RefreshCallback);
	}
}

void AVePlayerController::LogCurrentSpace() {
	if (const auto* VeWorldSettings = Cast<AVeWorldSettings>(GetWorldSettings())) {
		const FGuid& WorldId = VeWorldSettings->GetWorldId();
		ScreenColorF(FColor::Emerald, "Current World Id: %s", 3.0f, *WorldId.ToString(EGuidFormats::DigitsWithHyphensLower));
	}
	ScreenColorF(FColor::Orange, "Current Space: None", 3.0f);
}

void AVePlayerController::Input_CurrentWorldInfo() {
	if (const auto* VeWorldSettings = Cast<AVeWorldSettings>(GetWorldSettings())) {
		const FGuid& WorldId = VeWorldSettings->GetWorldId();
		if (WorldId.IsValid()) {
			Owner_ShowMenuCurrentWorld(WorldId);
		}
	}
}

#pragma endregion Utility

#pragma region Voice Capture and Speech Recognition

void AVePlayerController::Input_VoiceCapturePressed() {
	if (bIsSpeechToTextActive) {
		if (!bIsVoiceCaptureActive) {
			StartVoiceCapture();
		}
	}
}

void AVePlayerController::Input_VoiceCaptureReleased() {
	if (bIsSpeechToTextActive) {
		if (bIsVoiceCaptureActive) {
			FinishVoiceCapture();
		}
	}
}

// todo: use enhanced input system.
void AVePlayerController::OnServerInputActionCallback(UInteractionSubjectComponent* InInteractionSubjectComponent, UInteractionObjectComponent* InInteractionObjectComponent, ETriggerEvent TriggerEvent, UE::Math::TVector<double> Vector, float X, float Arg, const UInputAction* InputAction) {
	// Run only at standalone or listen server.
	if (IsRunningDedicatedServer()) {
		return;
	}

	// Check input action.
	if (!IsValid(InputAction)) {
		return;
	}

	// Filter only locally controlled characters.
	if (!IsLocalController()) {
		return;
	}

	// Speech to text, start/stop recording.
	if (InputAction == SpeechToTextInputAction) {
		if (TriggerEvent == ETriggerEvent::Triggered) {
			if (bIsSpeechToTextActive) {
				if (bIsVoiceCaptureActive) {
					FinishVoiceCapture();
				} else {
					StartVoiceCapture();
				}
			}
		}
	}
}

void AVePlayerController::OnSubmixRecordedFileDone(const USoundWave*) {
	GET_MODULE_SUBSYSTEM(SpeechRecognitionSubsystem, Api2, SpeechRecognition);
	if (!SpeechRecognitionSubsystem) {
		VeLogErrorFunc("SpeechRecognitionSubsystem is null");
	}

	// We need a valid world to get the audio device.
	const auto* const World = GetWorld();
	if (!IsValid(World)) {
		return;
	}

	World->GetTimerManager().SetTimer(SpeechRecognitionTimeoutTimerHandle, [=] {
		// todo: handle timeout
	}, SpeechRecognitionTimeout, false);

	FString FilePath = FPaths::ConvertRelativePathToFull(VoiceCaptureOutputDir / VoiceCaptureOutputFileName);

	FApi2SpeechRecognitionRequestMetadata RequestMetadata;
	RequestMetadata.FilePath = FilePath;

	const auto Callback = MakeShared<FOnSpeechRecognitionRequestCompleted2>();
	Callback->BindWeakLambda(this, [=](const FApi2SpeechRecognitionResponseMetadata& InMetadata, const EApi2ResponseCode InResponseCode, const FString& InError) {
		World->GetTimerManager().ClearTimer(SpeechRecognitionTimeoutTimerHandle);

		if (InResponseCode != EApi2ResponseCode::Ok) {
			VeLogErrorFunc("SpeechRecognition failed: %s", *InError);
			return;
		}

		if (InMetadata.Result.IsEmpty()) {
			VeLogErrorFunc("SpeechRecognition failed: no result");
			return;
		}

		OnSpeechRecognitionCompletedEvent.Broadcast(GetPlayerState<AVePlayerStateBase>(), InMetadata.Result);
		OnSpeechRecognitionCompleted.Broadcast(GetPlayerState<AVePlayerStateBase>(), InMetadata.Result);

		if (IsLocalController()) {
			switch (GetLocalRole()) {
			case ROLE_Authority:
				// Process standalone or listen server.
				if (auto* State = GetPlayerState<AVePlayerStateBase>(); IsValid(State)) {
					State->OnSpeechRecognitionCompleted(InMetadata.Result);
				}
				break;
			case ROLE_AutonomousProxy:
				// Send the result to the server.
				Server_OnSpeechRecognitionCompleted(InMetadata.Result);
				break;
			default:
				break;
			}
		}
	});

	SpeechRecognitionSubsystem->Recognize(RequestMetadata, Callback);
}

void AVePlayerController::Server_OnSpeechRecognitionCompleted_Implementation(const FString& Result) {
	if (auto* State = GetPlayerState<AVePlayerStateBase>(); IsValid(State)) {
		State->OnSpeechRecognitionCompleted(Result);
	}
}

void AVePlayerController::StartVoiceCapture() {
	if (bIsVoiceCaptureActive) {
		return;
	}

	// We need a valid world to get the audio device.
	const auto* const World = GetWorld();
	if (!IsValid(World)) {
		return;
	}

	bIsVoiceCaptureActive = true;

	// Activate the audio capture component to start recording.
	AudioCaptureComponent->Activate();

	// Start playing the audio component.
	if (bVoiceFeedbackEnabled) {
		VoiceFeedbackAudioComponent->Play();
	}

	// Set a timer to stop recording after a certain amount of time for cases where the user doesn't stop recording.
	World->GetTimerManager().SetTimer(VoiceCaptureTimeoutTimerHandle, this, &AVePlayerController::FinishVoiceCapture, VoiceCaptureMaxDuration, false);

	// Get the audio device for the world.
	FAudioDevice* AudioDevice = World->GetAudioDeviceRaw();

	// If the audio mixer is not enabled, we can't do voice capture.
	if (!AudioDevice->IsAudioMixerEnabled()) {
		return;
	}

	// Get the audio mixer device.
	Audio::FMixerDevice* MixerDevice = static_cast<Audio::FMixerDevice*>(AudioDevice);

	// Start recording.
	MixerDevice->StartRecording(SpeechToTextSoundSubmix, VoiceCaptureMaxDuration);

	// Bind the callback to the submix.
	//SpeechToTextSoundSubmix->OnSubmixRecordedFileDone.AddDynamic(this, &AVePlayerController::OnSubmixRecordedFileDone);
}

void AVePlayerController::FinishVoiceCapture() {
	// If voice capture is not active, return immediately.
	if (!bIsVoiceCaptureActive) {
		return;
	}

	// We need a valid world to get the audio device.
	const auto* const World = GetWorld();
	if (!IsValid(World)) {
		return;
	}

	// Set voice capture to inactive.
	bIsVoiceCaptureActive = false;

	// Stop the voice feedback audio component if enabled.
	if (bVoiceFeedbackEnabled) {
		VoiceFeedbackAudioComponent->Stop();
	}

	// Reset the max duration timer.
	if (VoiceCaptureTimeoutTimerHandle.IsValid()) {
		World->GetTimerManager().ClearTimer(VoiceCaptureTimeoutTimerHandle);
	}

	// Get the audio device for the world.
	FAudioDevice* AudioDevice = World->GetAudioDeviceRaw();

	// If the audio mixer is not enabled, we can't do voice capture.
	if (!AudioDevice->IsAudioMixerEnabled()) {
		return;
	}

	// Get the audio mixer device.
	Audio::FMixerDevice* MixerDevice = static_cast<Audio::FMixerDevice*>(AudioDevice);

	// Bind the callback to the submix.
	SpeechToTextSoundSubmix->OnSubmixRecordedFileDone.AddDynamic(this, &AVePlayerController::OnSubmixRecordedFileDone);

	float SampleRate;
	float ChannelCount;
	// Stop recording from the SpeechToTextSoundSubmix and get the recorded buffer.
	const Audio::FAlignedFloatBuffer& RecordedBuffer = MixerDevice->StopRecording(SpeechToTextSoundSubmix, ChannelCount, SampleRate);

	// Deactivate the audio capture component.
	AudioCaptureComponent->Deactivate();

	// If the recorded buffer is empty, return immediately.
	if (RecordedBuffer.Num() == 0) {
		// Remove the delegate if failed to record.
		SpeechToTextSoundSubmix->OnSubmixRecordedFileDone.RemoveDynamic(this, &AVePlayerController::OnSubmixRecordedFileDone);
		return;
	}

	// Create a new Audio::FAudioRecordingData instance and populate it with the recorded buffer.
	VoiceCaptureRecordingData.Reset(new Audio::FAudioRecordingData());
	VoiceCaptureRecordingData->InputBuffer = Audio::TSampleBuffer(RecordedBuffer, ChannelCount, SampleRate);
	VoiceCaptureRecordingData->InputBuffer.MixBufferToChannels(1);

	// Get the output directory for the project.
	FString OutputDir = VoiceCaptureOutputDir;

	// Write the recorded data to a WAV file and set up a callback for when the file is done writing.
	VoiceCaptureRecordingData->Writer.BeginWriteToWavFile(VoiceCaptureRecordingData->InputBuffer, TEXT("VoiceCapture"), OutputDir, [=] {
		// If the SpeechToTextSoundSubmix is valid and has a bound OnSubmixRecordedFileDone delegate, broadcast the event.
		if (IsValid(SpeechToTextSoundSubmix) && SpeechToTextSoundSubmix->OnSubmixRecordedFileDone.IsBound()) {
			// Send the nullptr as the data is written to a file.
			SpeechToTextSoundSubmix->OnSubmixRecordedFileDone.Broadcast(nullptr);

			// Remove the delegate after broadcasting the event.
			SpeechToTextSoundSubmix->OnSubmixRecordedFileDone.RemoveDynamic(this, &AVePlayerController::OnSubmixRecordedFileDone);
		}

		// Reset the VoiceCaptureRecordingData instance
		VoiceCaptureRecordingData.Reset();
	});
}

void AVePlayerController::OnSpeechRecognized(const FString& String) {
	if (HasAuthority()) {
		if (APawn* MyPawn = GetPawn(); IsValid(MyPawn)) {
			UAISense_Hearing::ReportNoiseEvent(this, MyPawn->GetActorLocation(), 1.0f, MyPawn, 0.0f, FName("SpeechFinished"));
		}
	}
}

#pragma endregion
