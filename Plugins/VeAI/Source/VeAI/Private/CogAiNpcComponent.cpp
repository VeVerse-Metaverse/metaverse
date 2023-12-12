// Authors: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022-2023 VeVerse AS. All rights reserved.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "CogAiNpcComponent.h"

#include "Api2AiSubsystem.h"
#include "Api2CogAiRequest.h"
#include "CogAiActionDataAsset.h"
#include "CogAiContextComponent.h"
#include "CogAiNpcSettingsComponent.h"
#include "CogAiPerceptibleActor.h"
#include "CogAiPersonalityDataAsset.h"
#include "EngineUtils.h"
#include "NavigationSystem.h"
#include "VeApi2.h"
#include "VeAiCharacter.h"
#include "VeAiPlayerState.h"
#include "VePlayerStateBase.h"
#include "VeShared.h"
#include "VeAI.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/GameStateBase.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISense_Hearing.h"

UCogAiNpcComponent::UCogAiNpcComponent() {}

void UCogAiNpcComponent::OnPerceptionUpdated(const TArray<AActor*>& Actors) {
	for (auto* Actor : Actors) {
		FActorPerceptionBlueprintInfo PerceptionInfo;
		if (PerceptionComponentRef->GetActorsPerception(Actor, PerceptionInfo)) {
			for (auto Stimulus : PerceptionInfo.LastSensedStimuli) {
				if (Stimulus.WasSuccessfullySensed()) {
					// Check if the stimulus is a sound.
					if (bEnableHearing && Stimulus.Type == UAISense::GetSenseID(UAISense_Hearing::StaticClass())) {
						// Check if the stimulus is an alert.
						if (Stimulus.Tag == TEXT("Alert")) {
							if (auto* PerceptibleComponent = Actor->FindComponentByClass<UCogAiPerceptibleComponent>(); IsValid(PerceptibleComponent)) {
								auto* Name = PerceptibleComponent->PerceptibleProperties.Find(TEXT("name"));
								auto* Alert = PerceptibleComponent->PerceptibleProperties.Find(TEXT("alert"));
								FString Message;
								if (Alert) {
									Message = FString::Printf(TEXT("sounds: %s"), **Alert);
								} else {
									Message = FString::Printf(TEXT("sounds"));
								}

								FCogAiPerception Perception;
								Perception.Type = EAiPerception::Audio;
								Perception.Name = Name ? *Name : TEXT("");
								Perception.Description = Message;
								Perception.Strength = Stimulus.Strength; // todo: Calculate the strength based on the distance and other properties.
								if (!Perception.Name.IsEmpty() || Perception.Strength > 0) {
									UpdatePerception(Perception);
								}
							}
						}

						// Check if the stimulus is a speech.
						else if (Stimulus.Tag == TEXT("SpeeshStarted")) {
							// Someone is about to say something to us! (This originates from this component class).
							if (IsValid(BlackboardRef)) {
								// Notify the blackboard that someone is about to say something to us.
								BlackboardRef->SetValueAsObject(TEXT("Speaker"), Actor);
							}

							Speaker = Actor;
						} else if (Stimulus.Tag == TEXT("SpeechFinished")) {
							// Someone is about to say something to us! (This originates from this component class).
							if (IsValid(BlackboardRef)) {
								// Notify the blackboard that someone is about to say something to us.
								BlackboardRef->SetValueAsObject(TEXT("Speaker"), nullptr);
							}

							Speaker = nullptr;

							// Someone just said something to us! (This is called when the sound is finished playing at the AVeAiCharacter class.)
							if (const auto* PlayerController = Cast<APlayerController>(Actor); IsValid(PlayerController)) {
								if (auto* State = PlayerController->GetPlayerState<AVePlayerStateBase>(); IsValid(State)) {
									const auto Metadata = State->GetUserMetadata();
									const auto PlayerName = Metadata.Name;
									const auto Message = FString::Printf(TEXT("says: %s"), *State->SpeechRecognitionMessage);

									FCogAiPerception Perception;
									Perception.Type = EAiPerception::Audio;
									Perception.Name = PlayerName;
									Perception.Description = Message;
									Perception.Strength = Stimulus.Strength; // todo: Calculate the strength based on the distance and other properties.
									if (!Perception.Name.IsEmpty() || Perception.Strength > 0) {
										UpdatePerception(Perception);
									}
								}
							} else if (auto* AiController = Cast<ACogAiController>(Actor); IsValid(AiController)) {
								if (AiController == GetOwner()) {
									return;
								}

								if (auto* State = AiController->GetAiPlayerState<AVeAiPlayerState>(); IsValid(State)) {
									const auto Metadata = State->GetUserMetadata();
									const auto PlayerName = Metadata.Name;
									auto* OtherNpcComponent = AiController->FindComponentByClass<UCogAiNpcComponent>();
									const auto Message = OtherNpcComponent->LastMessage;

									FCogAiPerception Perception;
									Perception.Type = EAiPerception::Audio;
									Perception.Name = PlayerName;
									Perception.Description = Message;
									Perception.Strength = Stimulus.Strength; // todo: Calculate the strength based on the distance and other properties.
									if (!Perception.Name.IsEmpty() || Perception.Strength > 0) {
										UpdatePerception(Perception);
									}
								}
							} else if (auto* Pawn = Cast<APawn>(Actor); IsValid(Pawn)) {
								// Handle case when the event originates from a pawn, not from a controller.

								if (auto* PawnAiController = Cast<ACogAiController>(Pawn->GetController()); IsValid(PawnAiController)) {
									if (PawnAiController == GetOwner()) {
										return;
									}

									if (auto* State = PawnAiController->GetAiPlayerState<AVeAiPlayerState>(); IsValid(State)) {
										const auto Metadata = State->GetUserMetadata();
										const auto PlayerName = Metadata.Name;
										auto* OtherNpcComponent = PawnAiController->FindComponentByClass<UCogAiNpcComponent>();
										const auto Message = OtherNpcComponent->LastMessage;

										FCogAiPerception Perception;
										Perception.Type = EAiPerception::Audio;
										Perception.Name = PlayerName;
										Perception.Description = Message;
										Perception.Strength = Stimulus.Strength; // todo: Calculate the strength based on the distance and other properties.
										if (!Perception.Name.IsEmpty() || Perception.Strength > 0) {
											UpdatePerception(Perception);
										}
									}
								} else {
									if (auto* PawnPlayerController = Cast<APlayerController>(Pawn->GetController()); IsValid(PawnPlayerController)) {
										if (auto* State = PawnPlayerController->GetPlayerState<AVePlayerStateBase>(); IsValid(State)) {
											const auto Metadata = State->GetUserMetadata();
											const auto PlayerName = Metadata.Name;
											const auto Message = FString::Printf(TEXT("says: %s"), *State->SpeechRecognitionMessage);

											FCogAiPerception Perception;
											Perception.Type = EAiPerception::Audio;
											Perception.Name = PlayerName;
											Perception.Description = Message;
											Perception.Strength = Stimulus.Strength; // todo: Calculate the strength based on the distance and other properties.
											if (!Perception.Name.IsEmpty() || Perception.Strength > 0) {
												UpdatePerception(Perception);
											}
										}
									}
								}
							}
						}
					} else if (bEnableSight && Stimulus.Type == UAISense::GetSenseID(UAISense_Sight::StaticClass())) {
						if (auto* Pawn = Cast<APawn>(Actor); IsValid(Pawn)) {
							if (auto* State = Pawn->GetPlayerState<AVePlayerStateBase>()) {
								if (IsValid(State)) {
									const auto Metadata = State->GetUserMetadata();

									FCogAiPerception Perception;
									Perception.Type = EAiPerception::Visual;
									Perception.Name = Metadata.Name;
									Perception.Description = Metadata.Description;
									Perception.Strength = Stimulus.Strength; // todo: Calculate the strength based on the distance and other properties.
									if (!Perception.Name.IsEmpty() || Perception.Strength > 0) {
										UpdatePerception(Perception);
									}
								}
							} else if (auto* ControllerAi = Pawn->GetController<ACogAiController>()) {
								if (ControllerAi == GetOwner()) {
									return;
								}

								if (auto* StateAi = ControllerAi->GetAiPlayerState<AVeAiPlayerState>(); IsValid(StateAi)) {
									const auto Metadata = StateAi->GetUserMetadata();

									FCogAiPerception Perception;
									Perception.Type = EAiPerception::Visual;
									Perception.Name = Metadata.Name;
									Perception.Description = Metadata.Description;
									Perception.Strength = Stimulus.Strength; // todo: Calculate the strength based on the distance and other properties.
									if (!Perception.Name.IsEmpty() || Perception.Strength > 0) {
										UpdatePerception(Perception);
									}
								}
							}
						} else if (auto* PerceptibleActor = Cast<ACogAiPerceptibleActor>(Actor); IsValid(PerceptibleActor)) {
							FCogAiPerception Perception;
							Perception.Type = EAiPerception::Visual;
							Perception.Name = PerceptibleActor->GetPerceptibleProperty(TEXT("name"));
							Perception.Description = PerceptibleActor->GetPerceptibleProperty(TEXT("description"));
							Perception.Strength = Stimulus.Strength; // todo: Calculate the strength based on the distance and other properties.
							if (!Perception.Name.IsEmpty() || Perception.Strength > 0) {
								UpdatePerception(Perception);
							}
						} else {
							VeLogVerboseFunc("unknown actor: %s", *Actor->GetName());
						}
					}
				}
			}
		} else {
			VeLogVerboseFunc("failed to get perception info for actor: %s", *Actor->GetName());
		}
	}
}

bool UCogAiNpcComponent::HasAuthority() const {
	const auto* Owner = GetOwner();
	if (!IsValid(Owner)) {
		VeLogErrorFunc("invalid owner");
		return false;
	}

	return Owner->HasAuthority();
}

void UCogAiNpcComponent::BeginPlay() {
	Super::BeginPlay();

	// Only the server should run the AI.
	if (!HasAuthority()) {
		return;
	}

	if (const auto* const World = GetWorld(); !IsValid(World)) {
		VeLogErrorFunc("invalid world");
		return;
	}

	IdleTimerDelegate.BindWeakLambda(this, [&] {
		if (bEnableProcessing && bEnableIdleMessages) {
			// Send the idle message.
			SendIdleMessage();
		}
	});

	PerceptionTimerDelegate.BindWeakLambda(this, [&] {
		if (bEnableProcessing) {
			// Send the accumulated perception.
			SendPerceptionMessage();
		}
	});
}

void UCogAiNpcComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Take the pending say action if there is one.
	if (PendingSayActions.Num() > 0) {
		// We can now freely speak as we won't interrupt any other person speaking.
		if (Speaker == nullptr) {
			TakeAction(PendingSayActions[0].Key);
			PendingSayActions.RemoveAt(0);
		}

		for (TPair<UCogAiActionMessage*, float>& PendingSayAction : PendingSayActions) {
			PendingSayAction.Value -= DeltaTime;
			if (PendingSayAction.Value <= 0) {
				PendingSayActions.RemoveAt(0);
			}
		}
	}
}

void UCogAiNpcComponent::ScheduleIdleRequest() {
	if (const auto* const World = GetWorld(); IsValid(World)) {
		World->GetTimerManager().ClearTimer(IdleTimerHandle);
		World->GetTimerManager().SetTimer(IdleTimerHandle, IdleTimerDelegate, IdleTimerDelay, false);
	}
}

void UCogAiNpcComponent::SchedulePerceptionRequest() {
	if (const auto* const World = GetWorld(); IsValid(World)) {
		// Schedule the timer if it's not already scheduled.
		if (!PerceptionTimerHandle.IsValid()) {
			World->GetTimerManager().SetTimer(PerceptionTimerHandle, PerceptionTimerDelegate, PerceptionAccumulationTime, false);
		}
	}
}

void UCogAiNpcComponent::ResetPerceptionSchedule() {
	if (const auto* const World = GetWorld(); IsValid(World)) {
		// Clear the timer if it's already scheduled, so that it can be scheduled again.
		if (PerceptionTimerHandle.IsValid()) {
			World->GetTimerManager().ClearTimer(PerceptionTimerHandle);
		}
	}
}

void UCogAiNpcComponent::EndPlay(const EEndPlayReason::Type EndPlayReason) {
	if (IsValid(PerceptionComponentRef)) {
		PerceptionComponentRef->OnPerceptionUpdated.RemoveDynamic(this, &UCogAiNpcComponent::OnPerceptionUpdated);
	}

	if (const auto* const World = GetWorld(); IsValid(World)) {
		if (IdleTimerHandle.IsValid()) {
			World->GetTimerManager().ClearTimer(IdleTimerHandle);
		}
	}

	Super::EndPlay(EndPlayReason);
}

void UCogAiNpcComponent::InitializeWithController(ACogAiController* CogAiController) {
	if (!IsValid(CogAiController)) {
		VeLogErrorFunc("failed to initialize with controller: invalid reference to controller");
		return;
	}

	ControllerRef = CogAiController;

	PerceptionComponentRef = ControllerRef->GetAIPerceptionComponent();

	if (IsValid(PerceptionComponentRef)) {
		PerceptionComponentRef->OnPerceptionUpdated.AddDynamic(this, &UCogAiNpcComponent::OnPerceptionUpdated);
	}

	BlackboardRef = ControllerRef->GetBlackboardComponent();

	if (!IsValid(BehaviorTreeRef)) {
		VeLogErrorFunc("failed to run behavior tree: invalid reference to behavior tree");
		return;
	}

	ControllerRef->RunBehaviorTree(BehaviorTreeRef);

	SendInitialRequest();
}

void UCogAiNpcComponent::InitializeWithPawn(APawn* Pawn) {
	if (AVeAiCharacter* AiCharacter = Cast<AVeAiCharacter>(Pawn); IsValid(AiCharacter)) {
		AiCharacter->OnAiPlayerStateChanged.AddWeakLambda(this, [this](AVeAiPlayerState* AiPlayerState) {
			if (IsValid(AiPlayerState)) {
				AiPlayerState->GetOnAiUserMetadataUpdated().AddWeakLambda(this, [=](const FVeUserMetadata&) {
					SendInitialRequest();
				});
			}
		});
	} else {
		SendInitialRequest();
	}
}

void UCogAiNpcComponent::TakeAction(UCogAiActionMessage* ActionMessage) {
	if (!IsValid(ActionMessage)) {
		return;
	}

	if (!IsValid(BlackboardRef)) {
		OnActionTaken.Broadcast(ActionMessage);
		VeLogErrorFunc("failed to set action message: invalid reference to blackboard");
		return;
	}

	// Set the action message in the blackboard for convenience.
	BlackboardRef->SetValueAsObject(TEXT("ActionMessage"), ActionMessage);

	// Set the target in the blackboard.
	const auto* TargetName = ActionMessage->Parameters.Find(TEXT("target"));
	AActor* TargetRef = nullptr;
	if (TargetName) {
		TargetRef = SetPerceptibleTarget(*TargetName);
	} else {
		TargetName = ActionMessage->Parameters.Find(TEXT("to"));
		if (TargetName) {
			// Sometimes the neural network can use "to" instead of "target", so handle that case too.
			TargetRef = SetPerceptibleTarget(*TargetName);
		}
	}

	// Set the emotion in the blackboard.
	if (const auto* Emotion = ActionMessage->Parameters.Find(TEXT("emotion")); Emotion) {
		if (auto* Owner = GetOwner(); IsValid(Owner)) {
			if (auto* VeAiCharacter = Cast<AVeAiCharacter>(Owner); IsValid(VeAiCharacter)) {
				VeAiCharacter->SetEmotion(FName(*Emotion));
			}
		}
		BlackboardRef->SetValueAsString(TEXT("Emotion"), *Emotion);
	}

	// Set the thoughts in the blackboard.
	if (const auto* Thoughts = ActionMessage->Parameters.Find(TEXT("thoughts")); Thoughts) {
		BlackboardRef->SetValueAsString(TEXT("Thoughts"), *Thoughts);
	}

	// Set the message in the blackboard.
	if (const auto* Message = ActionMessage->Parameters.Find(TEXT("message")); Message) {
		BlackboardRef->SetValueAsString(TEXT("Message"), *Message);
	}

	// Determine the action type.
	FString ActionType;
	if (ActionMessage->Type.IsEmpty()) {
		if (const auto* Action = ActionMessage->Parameters.Find(TEXT("type")); Action) {
			ActionType = Action->ToLower();
		} else if (Action = ActionMessage->Parameters.Find(TEXT("action")); Action) {
			// Sometimes the neural network can use "action" instead of "type", so handle that case too.
			ActionType = Action->ToLower();
		}
	} else {
		ActionType = ActionMessage->Type.ToLower();
	}

	// Set the action in the blackboard.
	BlackboardRef->SetValueAsString(TEXT("Action"), *ActionType);

	// Do additional processing based on the action type.
	if (ActionType == TEXT("explore") || ActionType == TEXT("roam") || ActionType == TEXT("wander")) {
		if (const auto ExploreLocation = FindRandomLocationInWorld(); ExploreLocation != FVector::ZeroVector) {
			BlackboardRef->SetValueAsVector(TEXT("TargetLocation"), ExploreLocation);
			BlackboardRef->SetValueAsObject(TEXT("FollowTarget"), nullptr);
		}
	} else if (ActionType == TEXT("follow")) {
		// Set the follow target in the blackboard to start following.
		if (IsValid(TargetRef)) {
			BlackboardRef->SetValueAsObject(TEXT("FollowTarget"), TargetRef);
		}
	} else if (ActionType == TEXT("unfollow")) {
		// Clear the follow target in the blackboard to stop following.
		BlackboardRef->SetValueAsObject(TEXT("FollowTarget"), nullptr);
	} else if (ActionType == TEXT("say")) {
		if (!IsValid(TargetRef)) {
			TargetRef = SetPerceptibleTarget("self");
			if (!IsValid(TargetRef)) {
				VeLogErrorFunc("failed to set action target: invalid reference to self");
			}
		}
	}

	const auto* MessagePtr = ActionMessage->Parameters.Find(TEXT("message"));
	if (ActionType == TEXT("say") || MessagePtr) {
		if (MessagePtr) {
			LastMessage = *MessagePtr;

			if (GetOwner()) {
				UAISense_Hearing::ReportNoiseEvent(this, GetOwner()->GetActorLocation(), 1.0f, GetOwner(), 0.0f, FName("SpeechStarted"));
			}
		} else {
			LastMessage = "";
		}
	} else {
		LastMessage = "";
	}

	if (IsValid(ControllerRef)) {
		// Send the action taken to the AI controller.
		if (AVeAiController* AiController = Cast<AVeAiController>(ControllerRef); IsValid(AiController)) {
			AiController->OnActionTaken(ActionMessage);
		}

		// Send the action taken to the AI character.
		if (AVeAiCharacter* AiCharacter = Cast<AVeAiCharacter>(ControllerRef->GetPawn()); IsValid(AiCharacter)) {
			AiCharacter->OnActionTaken(ActionMessage);
		}
	}

	// Broadcast the action taken.
	OnActionTaken.Broadcast(ActionMessage);
}

AActor* UCogAiNpcComponent::SetPerceptibleTarget(const FString& Name) const {
	if (IsValid(BlackboardRef)) {
		if (auto* Perceptible = FindPerceptibleInWorld(Name)) {
			BlackboardRef->SetValueAsObject(TEXT("Perceptible"), Perceptible);
			if (auto* Actor = Perceptible->GetOwner()) {
				BlackboardRef->SetValueAsObject(TEXT("Target"), Actor);
				return Actor;
			}
		}
	}
	return nullptr;
}

void UCogAiNpcComponent::SetPerceptibleTarget(AActor* Actor) const {
	if (IsValid(BlackboardRef)) {
		BlackboardRef->SetValueAsObject(TEXT("Target"), Actor);

		if (IsValid(Actor)) {
			if (auto* PerceptibleComponent = Actor->FindComponentByClass<UCogAiPerceptibleComponent>(); IsValid(PerceptibleComponent)) {
				BlackboardRef->SetValueAsObject(TEXT("Perceptible"), PerceptibleComponent);
			}
		}
	}
}

void UCogAiNpcComponent::UpdatePerception(const FCogAiPerception& Perception) {
	// Skip duplicate perceptions.
	if (PendingPerceptions.Contains(Perception)) {
		return;
	}

	if (ProcessedPerceptions.Contains(Perception)) {
		return;
	}

	if (Perception.Strength > 0) {
		// Add the perception to the pending perceptions.
		PendingPerceptions.Add(Perception);

		if (PendingPerceptions.Num() > MaxPerceptionsToKeep) {
			// Remove the oldest perception.
			PendingPerceptions.RemoveAt(0);
		}

		// Collect all the pending perceptions and filter most relevant ones.
		UCogAiPerceptionSystemMessage* PerceptionMessage = NewObject<UCogAiPerceptionSystemMessage>(this);
		PerceptionMessage->Role = ECogAiMessageRole::System;

		switch (Perception.Type) {
		case EAiPerception::Audio:
			PerceptionMessage->Audio.Add(Perception);
			break;
		case EAiPerception::Visual:
			PerceptionMessage->Visual.Add(Perception);
			break;
		case EAiPerception::Other:
			PerceptionMessage->Other.Add(Perception);
			break;
		default:
			VeLogWarningFunc("Unknown perception type: %d", Perception.Type);
			break;
		}

		// Add the perception message to the history.
		if (StoreMessage(PerceptionMessage)) {
			// Schedule the next request to be sent to the AI.
			SchedulePerceptionRequest();
		}
	}
}

void UCogAiNpcComponent::ForceUpdateContext() {
	UpdateContext();
}

void UCogAiNpcComponent::SendInitialRequest() {
	if (!HasAuthority()) {
		return;
	}

	if (bSentInitialRequest) {
		return;
	}

	FApi2CogAiRequestMetadata Metadata;

	UpdateWhoAmI();
	if (!IsValid(WhoamiSystemMessage) || WhoamiSystemMessage->Name.IsEmpty()) {
		// Name is required.
		return;
	}
	Metadata.Messages.Add(*WhoamiSystemMessage);

	UpdateContext();
	if (IsValid(ContextSystemMessage) && ContextSystemMessage->Description.Len() > 0) {
		Metadata.Messages.Add(*ContextSystemMessage);
	}

	UpdateActions();
	if (!IsValid(ActionsSystemMessage) || ActionsSystemMessage->Actions.Num() == 0) {
		// Actions are required.
		return;
	}

	Metadata.Messages.Add(*ActionsSystemMessage);

	if (Metadata.Messages.Num() > 0) {
		SendMessage(Metadata);
	}

	bSentInitialRequest = true;
}

void UCogAiNpcComponent::SendMessage(const FApi2CogAiRequestMetadata& Metadata) {
	if (bBusy || !bEnableProcessing) {
		return;
	}

	if (!HasAuthority()) {
		return;
	}

	// Lock the component to prevent sending multiple requests at the same time. The lock will be released when the response is received.
	bBusy = true;

	GET_MODULE_SUBSYSTEM(AiSubsystem, Api2, Ai);
	if (AiSubsystem) {
		// Check if the user key should be used.
		if (IsValid(NpcSettingsComponentRef) && NpcSettingsComponentRef->bUseUserKey && !NpcSettingsComponentRef->UserKey.IsEmpty()) {
			// Form the user metadata.
			FApi2CogAiUserRequestMetadata UserMetadata;
			UserMetadata.Key = NpcSettingsComponentRef->UserKey;
			UserMetadata.Version = NpcSettingsComponentRef->UserVersion;
			UserMetadata.Messages = Metadata.Messages;

			// Send the user request.
			const TSharedRef<FOnCogAiRequestCompleted> InCallback = MakeShared<FOnCogAiRequestCompleted>();
			InCallback->BindWeakLambda(this, [&](const FApi2CogAiResponseMetadata& InMetadata, EApi2ResponseCode InResponseCode, const FString& InError) {
				bBusy = false;
				OnMessageReceived(InMetadata, InResponseCode, InError);
			});
			AiSubsystem->SendCogAiUserRequest(UserMetadata, InCallback);
		} else {
			// Send the default request.
			const TSharedRef<FOnCogAiRequestCompleted> InCallback = MakeShared<FOnCogAiRequestCompleted>();
			InCallback->BindWeakLambda(this, [&](const FApi2CogAiResponseMetadata& InMetadata, EApi2ResponseCode InResponseCode, const FString& InError) {
				bBusy = false;
				OnMessageReceived(InMetadata, InResponseCode, InError);
			});
			AiSubsystem->SendCogAiRequest(Metadata, InCallback);
		}
	}
}

void UCogAiNpcComponent::AddMessageHistoryToMetadata(FApi2CogAiRequestMetadata& Metadata, const int InMaxHistorySize) {
	const int StartIndex = FMath::Max(0, MessageHistory.Num() - InMaxHistorySize);
	int AddedMessages = 0;
	for (int i = StartIndex; i < MessageHistory.Num(); i++) {
		if (MessageHistory.IsValidIndex(i) && IsValid(MessageHistory[i])) {
			// Skip system messages from the history.
			if ((*MessageHistory[i]).Role != ECogAiMessageRole::System) {
				Metadata.Messages.Add(*MessageHistory[i]);
				AddedMessages++;
				if (AddedMessages >= InMaxHistorySize) {
					break;
				}
			}
		}
	}
}

void UCogAiNpcComponent::SendIdleMessage() {
	if (bBusy || !bEnableProcessing || !bEnableIdleMessages) {
		return;
	}

	UpdateWhoAmI();
	UpdateContext();
	UpdateActions();

	// Check if the component is running on a dedicated server.
	if (IsRunningDedicatedServer()) {
		const auto* const World = GetWorld();
		if (!IsValid(World)) {
			VeLogErrorFunc("Failed to get the world.");
			return;
		}

		// Check if the game state is valid.
		if (const auto* GameState = World->GetGameState(); IsValid(GameState)) {
			// Check if there are any players connected to the server.
			if (GameState->PlayerArray.Num() == 0) {
				// No players connected, don't send the idle message, but keep the timer running to try again later.
				VeLogFunc("No players connected, skipping idle message.");
				ScheduleIdleRequest();
				return;
			}
		} else {
			VeLogErrorFunc("Failed to get the game state.");
			return;
		}
	}

	// Create the idle system message.
	FApi2CogAiRequestMetadata Metadata;
	if (IsValid(WhoamiSystemMessage)) {
		Metadata.Messages.Add(*WhoamiSystemMessage);
	}
	if (IsValid(ContextSystemMessage)) {
		Metadata.Messages.Add(*ContextSystemMessage);
	}
	if (IsValid(ActionsSystemMessage)) {
		Metadata.Messages.Add(*ActionsSystemMessage);
	}

	// Add the history of messages to the request.
	AddMessageHistoryToMetadata(Metadata, MaxHistorySizeToProcess - 3);

	if (Metadata.Messages.Num() > 0) {
		SendMessage(Metadata);
	}
}

void UCogAiNpcComponent::SendPerceptionMessage() {
	if (bBusy || !bEnableProcessing) {
		// Reset timer here to avoid spamming the server with requests but still keep the timer running to try again later.
		ResetPerceptionSchedule();
		SchedulePerceptionRequest();
		return;
	}

	UpdateWhoAmI();
	UpdateContext();
	UpdateActions();

	// Check if the component is running on a dedicated server.
	if (IsRunningDedicatedServer()) {
		const auto* const World = GetWorld();
		if (!IsValid(World)) {
			VeLogErrorFunc("Failed to get the world.");
			return;
		}

		// Check if the game state is valid.
		if (const auto* GameState = World->GetGameState(); IsValid(GameState)) {
			// Check if there are any players connected to the server.
			if (GameState->PlayerArray.Num() == 0) {
				// No players connected, don't send the perception message, but keep the timer running to try again later.
				VeLogFunc("No players connected, skipping perception message.");
				ResetPerceptionSchedule();
				SchedulePerceptionRequest();
				return;
			}
		} else {
			VeLogErrorFunc("Failed to get the game state.");
			return;
		}
	}

	// Create the perception system message.
	FApi2CogAiRequestMetadata Metadata;
	if (IsValid(WhoamiSystemMessage)) {
		Metadata.Messages.Add(*WhoamiSystemMessage);
	}
	if (IsValid(ContextSystemMessage)) {
		Metadata.Messages.Add(*ContextSystemMessage);
	}
	if (IsValid(ActionsSystemMessage)) {
		Metadata.Messages.Add(*ActionsSystemMessage);
	}

	AddMessageHistoryToMetadata(Metadata, MaxHistorySizeToProcess - 4);

	// Get all pending perceptions and form a request.
	const int Max = PendingPerceptions.Num() > MaxPerceptionsToKeep ? MaxPerceptionsToKeep : PendingPerceptions.Num();
	for (int i = 0; i < Max; i++) {
		if (PendingPerceptions.Num() > i) {
			if (PendingPerceptions.IsValidIndex(i)) {
				UCogAiPerceptionSystemMessage* PerceptionMessage = NewObject<UCogAiPerceptionSystemMessage>(this);
				PerceptionMessage->Role = ECogAiMessageRole::System;
				for (const auto& Perception : PendingPerceptions) {
					switch (Perception.Type) {
					case EAiPerception::Audio:
						PerceptionMessage->Audio.Add(Perception);
						break;
					case EAiPerception::Visual:
						PerceptionMessage->Visual.Add(Perception);
						break;
					case EAiPerception::Other:
						PerceptionMessage->Other.Add(Perception);
						break;
					default:
						VeLogWarningFunc("Unknown perception type: %d", Perception.Type);
						break;
					}

					// Store the processed perception.
					ProcessedPerceptions.Add(Perception);
					if (ProcessedPerceptions.Num() > MaxPerceptionsToKeep) {
						ProcessedPerceptions.RemoveAt(0);
					}
				}
				Metadata.Messages.Add(*PerceptionMessage);
			}
		}
	}

	// Send the request.
	if (Metadata.Messages.Num() > 0) {
		SendMessage(Metadata);
	}

	// Clear the pending perceptions.
	PendingPerceptions.Empty();

	// Clear the pending perceptions.
	ResetPerceptionSchedule();
}

void UCogAiNpcComponent::OnMessageReceived(const FApi2CogAiResponseMetadata& InMetadata, EApi2ResponseCode InResponseCode, const FString& InError) {
	if (InResponseCode != EApi2ResponseCode::Ok) {
		LogErrorF("Failed to send request to backend: %s", *InError);
		bBusy = false;

		// Schedule the retry.
		ScheduleIdleRequest();

		return;
	}

	// Check if we received a system query from the AI.
	for (const FApi2CogAiResponseMessage& Message : InMetadata.Messages) {

#if WITH_EDITORONLY_DATA
		// Serialize the FJsonObject to an FString and print it for debugging.
		FString JsonString;
		TSharedRef<TJsonWriter<>> JsonWriter = TJsonWriterFactory<>::Create(&JsonString);
		FJsonSerializer::Serialize(Message.Content.ToSharedRef(), JsonWriter);
		VeLogFunc("Message from %s: %s", *Message.From, *JsonString);
#endif

		// Check if the message is marked as it came from the system (which is not allowed).
		if (Message.From == TEXT("system")) {
			LogWarningF("Backend sent message from system, which is not allowed, dropping.");
			continue;
		}

		// Check if the message is marked as it came from the NPC (which is correct).
		if (Message.From == TEXT("npc")) {

			if (Message.Content->HasField(TEXT("action"))) {
				// Action is sent by the backend to the system to perform an action on the NPC.
				UCogAiActionMessage* Action = NewObject<UCogAiActionMessage>(this);
				Action->Role = ECogAiMessageRole::Npc;

				// Check if the action is wrapped in an object.
				if (Message.Content->HasTypedField<EJson::Object>(TEXT("action"))) {
					const TSharedPtr<FJsonObject> ActionJson = Message.Content->GetObjectField(TEXT("action"));
					if (ActionJson != nullptr) {
						// The action is a JSON object.
						for (auto ParameterValue : ActionJson->Values) {
							if (ParameterValue.Key == TEXT("type")) {
								// The action type is stored in the "type" field.
								Action->Type = ParameterValue.Value->AsString();
							} else if (ParameterValue.Key == TEXT("action")) {
								// But the AI sometimes can use "action" instead of "type".
								Action->Type = ParameterValue.Value->AsString();
							} else {
								// Process the rest of the fields as parameters.
								Action->Parameters.Add(ParameterValue.Key, ParameterValue.Value->AsString());
							}
						}
					}
				} else {
					// The action is not wrapped in an object, it is the message content itself.
					for (auto ParameterValue : Message.Content->Values) {
						if (ParameterValue.Key == TEXT("type")) {
							// The action type is stored in the "type" field.
							Action->Type = ParameterValue.Value->AsString();
						} else if (ParameterValue.Key == TEXT("action")) {
							// But the AI sometimes can use "action" instead of "type".
							Action->Type = ParameterValue.Value->AsString();
						} else {
							// Process the rest of the fields as parameters.
							Action->Parameters.Add(ParameterValue.Key, ParameterValue.Value->AsString());
						}
					}
				}

				if (Action->Type.IsEmpty()) {
					// Try to infer the action type from the parameters.

					if (Action->Parameters.Contains(TEXT("message"))) {
						// Pretty sure this is a say action, as it has a message parameter which is usually used by the say action.
						Action->Type = TEXT("say");
					} else {
						LogWarningF("Backend sent action message without type, and it is not possible to infer it from the parameters.");
					}
				}

				// Queue the action for the next batch.
				StoreMessage(Action);

				if (Action->Type == "say" && Speaker != nullptr) {
					// Wait for speaker to stop talking.
					PendingSayActions.Add(TPair<UCogAiActionMessage*, float>(Action, PendingSayTimeout));
				} else {
					// Perform the action.
					TakeAction(Action);
				}
			} else {
				LogWarningF("Backend sent unknown message, must be either system or action");
			}
		}
	}

	bBusy = false;

	ScheduleIdleRequest();
}

bool UCogAiNpcComponent::StoreMessage(UCogAiMessage* Message) {
	if (MessageHistory.Contains(Message)) {
		return false;
	}

	MessageHistory.Add(Message);

	// Rotate the message history.
	if (MessageHistory.Num() > MaxHistorySize) {
		MessageHistory.RemoveAt(0);
	}

	return true;
}

#pragma region Helpers

UCogAiContextComponent* UCogAiNpcComponent::FindClosestContextInWorld() const {
	float MinDistanceSquared = TNumericLimits<float>::Max();
	UCogAiContextComponent* ClosestContext = nullptr;
	for (TActorIterator<AActor> ActorItr(GetWorld()); ActorItr; ++ActorItr) {
		// Find the closest context actor.
		if (const AActor* FoundActor = *ActorItr; IsValid(FoundActor)) {
			if (auto* Context = FoundActor->FindComponentByClass<UCogAiContextComponent>(); IsValid(Context)) {
				const auto* Owner = Cast<AAIController>(GetOwner());
				if (Owner == nullptr) {
					break;
				}
				const auto* Pawn = Owner->GetPawn();
				if (Pawn == nullptr) {
					break;
				}
				const float DistanceSquared = (FoundActor->GetActorLocation() - Pawn->GetActorLocation()).SizeSquared();
				if (DistanceSquared < MinDistanceSquared) {
					MinDistanceSquared = DistanceSquared;
					ClosestContext = Context;
				}
			}
		}
	}

	return ClosestContext;
}

UCogAiPerceptibleComponent* UCogAiNpcComponent::FindPerceptibleInWorld(const FString& Name) const {
	FString SearchString = Name;

	if (SearchString == TEXT("me") || SearchString == TEXT("self")) {
		if (IsValid(ControllerRef)) {
			const auto* const Pawn = ControllerRef->GetPawn();
			return Pawn->FindComponentByClass<UCogAiPerceptibleComponent>();
		}
	}

	// Extract the entity name from the location string in form of "location of <entity name>" or "<entity name>'s location".
	if (SearchString.StartsWith(TEXT("location of "))) {
		SearchString = SearchString.RightChop(12);
	} else if (SearchString.EndsWith(TEXT("'s location"))) {
		SearchString = SearchString.LeftChop(12);
	}

	// Same for position.
	if (SearchString.StartsWith(TEXT("position of "))) {
		SearchString = SearchString.RightChop(12);
	} else if (SearchString.EndsWith(TEXT("'s position"))) {
		SearchString = SearchString.LeftChop(12);
	}

	if (SearchString.StartsWith(TEXT("the "))) {
		SearchString = SearchString.RightChop(4);
	} else if (SearchString.StartsWith(TEXT("a "))) {
		SearchString = SearchString.RightChop(2);
	}

	float MinDistanceSquared = TNumericLimits<float>::Max();
	UCogAiPerceptibleComponent* ClosestContext = nullptr;
	for (TActorIterator<AActor> ActorItr(GetWorld()); ActorItr; ++ActorItr) {
		// Find the closest context actor.
		if (const AActor* Actor = *ActorItr; IsValid(Actor)) {
			if (auto* Context = Actor->FindComponentByClass<UCogAiPerceptibleComponent>(); IsValid(Context)) {
				// Check if the name matches.
				if (const auto* NameProperty = Context->PerceptibleProperties.Find(TEXT("name")); !NameProperty || *NameProperty != Name) {
					continue;
				}

				// Select the closest one if there are multiple objects with the same name.
				const float DistanceSquared = (Actor->GetActorLocation() - GetOwner()->GetActorLocation()).SizeSquared();
				if (DistanceSquared < MinDistanceSquared) {
					MinDistanceSquared = DistanceSquared;
					ClosestContext = Context;
				}
			}
		}
	}

	return ClosestContext;
}

FVector UCogAiNpcComponent::FindRandomLocationInWorld() const {
	// Get the navigation system
	const auto* NavigationSystem = UNavigationSystemV1::GetCurrent(ControllerRef->GetWorld());

	// Find a random point on the navigation mesh
	if (FNavLocation RandomLocation; NavigationSystem->GetRandomReachablePointInRadius(ControllerRef->GetPawn()->GetActorLocation(), NpcSettingsComponentRef->RoamRadius, RandomLocation)) {
		return RandomLocation.Location;
	}

	return FVector::ZeroVector;
}

void UCogAiNpcComponent::SaveMessageHistoryToFile(const FString& FilePath) const {
	FString FileContent;

	for (const UCogAiMessage* Message : MessageHistory) {
		FileContent += Message->ToString() + "\n";
	}

	FFileHelper::SaveStringToFile(FileContent, *FilePath);
}

void UCogAiNpcComponent::UpdateContext() {
	if (!IsValid(ContextSystemMessage)) {
		ContextSystemMessage = NewObject<UCogAiContextSystemMessage>(this);
	}

	ContextSystemMessage->Role = ECogAiMessageRole::System;
	if (const auto* Context = FindClosestContextInWorld(); IsValid(Context)) {
		if (auto* Description = Context->PerceptibleProperties.Find(TEXT("desc"))) {
			ContextSystemMessage->Description = *Description;
		}
		if (auto* Location = Context->PerceptibleProperties.Find(TEXT("location"))) {
			ContextSystemMessage->Location = *Location;
		}
		if (auto* Time = Context->PerceptibleProperties.Find(TEXT("time"))) {
			ContextSystemMessage->Time = *Time;
		}
		if (auto* Weather = Context->PerceptibleProperties.Find(TEXT("weather"))) {
			ContextSystemMessage->Weather = *Weather;
		}
		if (auto* Vibe = Context->PerceptibleProperties.Find(TEXT("vibe"))) {
			ContextSystemMessage->Vibe = *Vibe;
		}
	}
}

void UCogAiNpcComponent::UpdateActions() {
	if (!IsValid(ActionsSystemMessage)) {
		ActionsSystemMessage = NewObject<UCogAiActionsSystemMessage>(this);
	}

	ActionsSystemMessage->Role = ECogAiMessageRole::System;

	if (const auto* ContextRef = FindClosestContextInWorld(); IsValid(ContextRef) && IsValid(ContextRef->ActionDataAssetRef)) {
		if (IsValid(ContextRef->ActionDataAssetRef)) {
			// Try to use the action data asset from the context.
			ActionsSystemMessage->Actions = ContextRef->ActionDataAssetRef->SupportedActions;
		}
	}

	if (ActionsSystemMessage->Actions.Num() <= 0) {
		if (IsValid(NpcSettingsComponentRef) && IsValid(NpcSettingsComponentRef->ActionDataAssetRef)) {
			// Use the action data asset from the NPC settings.
			ActionsSystemMessage->Actions = NpcSettingsComponentRef->ActionDataAssetRef->SupportedActions;
		}
	}
}

void UCogAiNpcComponent::UpdateWhoAmI() {
	if (!IsValid(WhoamiSystemMessage)) {
		WhoamiSystemMessage = NewObject<UCogAiWhoamiSystemMessage>(this);
	}

	WhoamiSystemMessage->Role = ECogAiMessageRole::System;

	if (IsValid(NpcSettingsComponentRef)) {
		if (const auto* Name = NpcSettingsComponentRef->PerceptibleProperties.Find(TEXT("name"))) {
			WhoamiSystemMessage->Name = *Name;
		}

		if (const auto* Description = NpcSettingsComponentRef->PerceptibleProperties.Find(TEXT("desc"))) {
			WhoamiSystemMessage->Description = *Description;
		}

		if (const auto* Faction = NpcSettingsComponentRef->PerceptibleProperties.Find(TEXT("faction"))) {
			WhoamiSystemMessage->Faction = *Faction;
		}

		if (NpcSettingsComponentRef->bOverridePersonalityProperties) {
			WhoamiSystemMessage->Personality = NpcSettingsComponentRef->PersonalityPropertiesOverride;
		} else {
			if (IsValid(NpcSettingsComponentRef->PersonalityDataAssetRef) && NpcSettingsComponentRef->PersonalityDataAssetRef->PersonalityProperties.Num() > 0) {
				WhoamiSystemMessage->Personality = NpcSettingsComponentRef->PersonalityDataAssetRef->PersonalityProperties;
			}
		}
	}
}

#pragma endregion
