// 

#include "VeAiSimpleFsmManager.h"
#include "Api2AiSimpleFsmRequest.h"
#include "Api2AiSubsystem.h"
#include "EngineUtils.h"
#include "VeAiSimpleFsmEntityComponent.h"
#include "VeApi2.h"
#include "VeShared.h"
#include "Kismet/GameplayStatics.h"
#include "VeAI.h"
#include "VeAiSimpleFsmBehaviorComponent.h"
#include "VeAiSimpleFsmLocationComponent.h"
#include "VeAiSimpleFsmLocationLink.h"
#include "VeAiSimpleFsmState.h"
#include "VeAiSimpleFsmSubjectComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/PlayerState.h"


AVeAiSimpleFsmManager::AVeAiSimpleFsmManager() {
	// Allow the actor to tick.
	PrimaryActorTick.bCanEverTick = true;
	// Set the tick interval to 1 second, it is required to check and process the next action.
	PrimaryActorTick.TickInterval = 1.0f;
}

void AVeAiSimpleFsmManager::OnSimpleFsmStatesRequestCompleted(const FApi2AiSimpleFsmMetadata& InMetadata, const EApi2ResponseCode InResponseCode, const FString& InErrorMessage) {
	// Check if the request was successful.
	if (InResponseCode == EApi2ResponseCode::Ok) {
		// Add an empty state to the beginning of the list, it will be used to reset the FSM and deleted on the first state execution.
		States.Add({});

		// Store states in the manager.
		for (const auto& StateMetadata : InMetadata.States) {
			FVeAiSimpleFsmState State;
			State.FromApiStateMetadata(StateMetadata);

			// Subject is required.
			State.SubjectEntity = GetSubject(State.NPC);
			if (!IsValid(State.SubjectEntity)) {
				VeLogErrorFunc("failed to find the subject entity: %s", *State.NPC);
				continue;
			}

			switch (State.ActionType) {
			case EVeAiSimpleFsmActionType::Go: {
				// Target is required, but can be any or empty for random location.
				State.TargetEntity = GetEntity(State.Target, true);
				break;
			}
			case EVeAiSimpleFsmActionType::Say: {
				// Target is required, but can be any or empty for self.
				State.TargetEntity = GetEntity(State.Target, true);
				State.Metadata = StateMetadata.Metadata;
				State.Emotion = StateMetadata.Emotion;
				break;
			}
			case EVeAiSimpleFsmActionType::Emote: {
				// Target is required, but can be any or empty for self.
				State.TargetEntity = GetEntity(State.Target, true);
				State.Metadata = StateMetadata.Metadata;
				State.Emotion = StateMetadata.Emotion;
				break;
			}
			case EVeAiSimpleFsmActionType::Give: {
				// Target is required, skip the state if it is not found.
				State.TargetEntity = GetEntity(State.Target, false);
				if (!IsValid(State.TargetEntity)) {
					VeLogErrorFunc("failed to find the target entity");
					continue;
				}
				// Context is required, skip the state if it is not found.
				State.ContextEntity = GetEntity(State.Context, false);
				if (!IsValid(State.ContextEntity)) {
					VeLogErrorFunc("failed to find the context entity");
					continue;
				}
				break;
			}
			case EVeAiSimpleFsmActionType::Use: {
				// Target is required, skip the state if it is not found.
				State.TargetEntity = GetEntity(State.Target, false);
				if (!IsValid(State.TargetEntity)) {
					VeLogErrorFunc("failed to find the target entity");
					continue;
				}
				// Context is optional, is object that should be used with the target.
				State.ContextEntity = GetEntity(State.Context, false);
				break;
			}
			default:
				// Skip the state if the action type is not supported.
				continue;
			}

			States.Add(State);
		}

		// Allow blueprints to execute the first state.
		OnStatesRequestCompleted();
	} else {
		VeLogErrorFunc("failed to get the simple FSM states: %s", *InErrorMessage);

		// Allow blueprints to handle the error and retry the request.
		OnStatesRequestFailed(InErrorMessage);
	}
}

void AVeAiSimpleFsmManager::RequestStates() {
	if (!HasAuthority()) {
		return;
	}

	const auto* const World = GetWorld();
	if (!IsValid(World)) {
		return;
	}

	// Check if there are any players in the game.
	bool bHasPlayers = false;
	for (FConstPlayerControllerIterator It = World->GetPlayerControllerIterator(); It; ++It) {
		APlayerController* PlayerController = It->Get();
		if (PlayerController && PlayerController->GetPlayerState<APlayerState>()) {
			// There is at least one player in the game
			bHasPlayers = true;
		}
	}

	// Skip the request if there are no players.
	if (!bHasPlayers) {
		return;
	}

	// Collect all the AI entities.
	TArray<UVeAiSimpleFsmEntityComponent*> OutEntities;
	for (FActorIterator It(World); It; ++It) {
		if (const auto Actor = *It; IsValid(Actor)) {
			if (auto Comp = Actor->FindComponentByClass<UVeAiSimpleFsmEntityComponent>(); IsValid(Comp)) {
				if (IsValid(Comp)) {
					OutEntities.Add(Comp);
				}
			}
		}
	}

	// Get the AI API subsystem.
	GET_MODULE_SUBSYSTEM(AiSubsystem, Api2, Ai);
	if (!AiSubsystem) {
		return;
	}

	// Create the request.
	FApi2AiSimpleFsmRequestMetadata RequestMetadata;

	RequestMetadata.Context = ContextDescription;
	RequestMetadata.StateNum = DesiredStateNum;

	// Reset all the known entities.
	KnownEntities.Empty();
	KnownLocations.Empty();
	KnownObjects.Empty();
	KnownSubjects.Empty();
	KnownPlayers.Empty();

	// Process all the entities and refill the known entities.
	for (auto* Entity : OutEntities) {
		if (Entity->Id.IsEmpty()) {
			continue;
		}

		// Process locations.
		switch (Entity->Type) {
		case EVeAiSimpleFsmEntityType::Location: {
			// Check if the location actor is valid.
			const auto* EntityOwner = Entity->GetOwner();
			if (!IsValid(EntityOwner)) {
				continue;
			}

			// Cast the generic entity to the location entity.
			const auto* LocationEntity = Cast<UVeAiSimpleFsmLocationComponent>(Entity);
			if (!IsValid(LocationEntity)) {
				continue;
			}

			// Create the location metadata.
			FApi2AiSimpleFsmLocation Location;

			// Assign the location id.
			Location.Name = Entity->GetId();

			// Assign the location description.
			Location.Description = Entity->GetDescription();

			// Process the location links if any present.
			for (const auto& [Name, Description, Target, Object] : LocationEntity->LinkedLocations) {
				// Create the link metadata.
				FApi2AiSimpleFsmLocationLink FsmLink;

				// Assign the link id.
				FsmLink.Name = Name;

				// Assign the link description.
				FsmLink.Description = Description;

				// Assign the link target.
				FsmLink.Target = Target;

				// Assign the link object.
				FsmLink.Object = Object;

				// Add the link to the location links.
				Location.Links.Add(FsmLink);
			}

			// Process the location entities if any present.
			TArray<AActor*> LocationActors;

			// Find all actor overlapping the location actor.
			EntityOwner->GetOverlappingActors(LocationActors);

			// For each actor overlapping the location actor.
			for (auto* LocationActor : LocationActors) {
				// Check if the actor is a valid entity.
				if (auto* LocationChildEntity = LocationActor->FindComponentByClass<UVeAiSimpleFsmEntityComponent>(); IsValid(LocationChildEntity)) {
					// Create the entity metadata.
					FApi2AiSimpleFsmEntity FsmEntity;
					FsmEntity.Name = LocationChildEntity->GetId();
					FsmEntity.Description = LocationChildEntity->GetDescription();

					// Add the entity to the location entities of the corresponding type.
					if (LocationChildEntity->Type == EVeAiSimpleFsmEntityType::Object) {
						Location.Entities.Objects.Add(FsmEntity);
					} else if (LocationChildEntity->Type == EVeAiSimpleFsmEntityType::NPC) {
						Location.Entities.NPCs.Add(FsmEntity);
					} else if (LocationChildEntity->Type == EVeAiSimpleFsmEntityType::Player) {
						Location.Entities.Players.Add(FsmEntity);
					}
				}
			}

			// Add the location to the request locations.
			RequestMetadata.Locations.Add(Location);
		}
		break;
		case EVeAiSimpleFsmEntityType::NPC: {
			auto* NPCEntity = Cast<UVeAiSimpleFsmSubjectComponent>(Entity);
			auto* BehaviorComp = NPCEntity->GetBehaviorComponent();

			// Make sure the subject has a behavior component.
			if (IsValid(NPCEntity) && IsValid(BehaviorComp)) {
				// Set the manager in the behavior component.
				BehaviorComp->SetFsmManager(this);

				KnownEntities.Add(Entity);
				KnownSubjects.Add(NPCEntity);
			}
		}
		break;
		case EVeAiSimpleFsmEntityType::Object: {
			KnownEntities.Add(Entity);
			KnownObjects.Add(Entity);
		}
		break;
		case EVeAiSimpleFsmEntityType::Player: {
			KnownEntities.Add(Entity);
			KnownPlayers.Add(Entity);
		}
		break;
		default:
			VeLogWarning("Entity type not supported: %s", *Entity->Id);
			break;
		}
	}

	const auto Callback = MakeShared<FOnAiSimpleFsmRequestCompleted2>();
	Callback->BindUObject(this, &AVeAiSimpleFsmManager::OnSimpleFsmStatesRequestCompleted);
	AiSubsystem->GetSimpleFsm(RequestMetadata, Callback);
}

// Code is Unreal Engine 5 C++. Change the code so that the lambda function in this code is a separate function bound to the same callback
void AVeAiSimpleFsmManager::BeginPlay() {
	Super::BeginPlay();
}

// Called every frame
void AVeAiSimpleFsmManager::Tick(const float DeltaTime) {
	Super::Tick(DeltaTime);
}

UVeAiSimpleFsmEntityComponent* AVeAiSimpleFsmManager::GetEntity(const FString& Id, const bool bFallbackToAny) {
	if (!HasAuthority()) {
		return nullptr;
	}

	if (auto** EntityPtr = KnownEntities.FindByPredicate([=](const UVeAiSimpleFsmEntityComponent* InComp) {
		return InComp->Id == Id;
	}); EntityPtr && IsValid(*EntityPtr)) {
		return *EntityPtr;
	}

	if (bFallbackToAny) {
		return KnownEntities[FMath::RandRange(0, KnownEntities.Num() - 1)];
	}

	return nullptr;
}

UVeAiSimpleFsmSubjectComponent* AVeAiSimpleFsmManager::GetSubject(const FString& Id, const bool bFallbackToAny) {
	if (!HasAuthority()) {
		return nullptr;
	}

	if (auto** EntityPtr = KnownSubjects.FindByPredicate([=](const UVeAiSimpleFsmSubjectComponent* InComp) {
		return InComp->Id == Id;
	}); EntityPtr && IsValid(*EntityPtr)) {
		return *EntityPtr;
	}

	if (bFallbackToAny) {
		return KnownSubjects[FMath::RandRange(0, KnownSubjects.Num() - 1)];
	}

	return nullptr;
}

UVeAiSimpleFsmEntityComponent* AVeAiSimpleFsmManager::GetObject(const FString& Id, const bool bFallbackToAny) {
	if (!HasAuthority()) {
		return nullptr;
	}

	if (auto** EntityPtr = KnownObjects.FindByPredicate([=](const UVeAiSimpleFsmEntityComponent* InComp) {
		return InComp->Id == Id;
	}); EntityPtr && IsValid(*EntityPtr)) {
		return *EntityPtr;
	}

	if (bFallbackToAny) {
		return KnownObjects[FMath::RandRange(0, KnownObjects.Num() - 1)];
	}

	return nullptr;
}

void AVeAiSimpleFsmManager::OnStateCompleted() {
	ExecuteNextState();
}

void AVeAiSimpleFsmManager::ExecuteNextState() {
	if (!HasAuthority()) {
		return;
	}

	if (States.Num() > 0) {
		if (IsValid(States[0].SubjectEntity)) {
			// Reset the state of the subject so it will know that it is not executing any state.
			States[0].SubjectEntity->GetBehaviorComponent()->SetState({});
		}

		if (IsValid(States[0].TargetEntity)) {
			// If the target is a subject, reset its state as well (for take and similar actions).
			if (const UVeAiSimpleFsmSubjectComponent* Target = Cast<UVeAiSimpleFsmSubjectComponent>(States[0].TargetEntity); IsValid(Target)) {
				// Reset the state of the target so it will know that it is not executing any state.
				Target->GetBehaviorComponent()->SetState({});
			}
		}

		// Remove the finished state.
		States.RemoveAt(0);

		// Execute the next state.
		if (States.Num() > 0) {
			ExecuteState(States[0]);
		} else {
			// Request a new set of states.
			RequestStates();
		}
	} else if (bUnlimited) {
		// Request a new set of states.
		RequestStates();
	}
}

void AVeAiSimpleFsmManager::ExecuteState(const FVeAiSimpleFsmState& State) {
	if (!HasAuthority()) {
		return;
	}

	// Get the subject entity.
	UVeAiSimpleFsmSubjectComponent* Subject = GetSubject(State.NPC, false);
	if (!IsValid(Subject)) {
		VeLogErrorFunc("failed to find the subject entity");
		ExecuteNextState();
	}

	// Get the blackboard component of the subject.
	auto* SubjectBehavior = Subject->GetBehaviorComponent();
	if (!IsValid(SubjectBehavior)) {
		VeLogErrorFunc("failed to find the behavior component of the subject");
		ExecuteNextState();
	}

	// Switch on the action.
	switch (State.ActionType) {
	case EVeAiSimpleFsmActionType::Go: {
		SubjectBehavior->SetState(State);
		break;
	}
	case EVeAiSimpleFsmActionType::Say: {
		SubjectBehavior->SetState(State);
		break;
	}
	case EVeAiSimpleFsmActionType::Emote: {
		SubjectBehavior->SetState(State);
		break;
	}
	case EVeAiSimpleFsmActionType::Give: {
		// Set the current action of the subject to "give".
		SubjectBehavior->SetState(State);

		// Get the target entity.
		UVeAiSimpleFsmSubjectComponent* TargetEntity = GetSubject(State.Target, true);
		if (!IsValid(TargetEntity)) {
			VeLogErrorFunc("failed to find the target entity");
			ExecuteNextState();
		}

		// Get the context entity.
		UVeAiSimpleFsmEntityComponent* ContextEntity = GetEntity(State.Context, false);
		if (!IsValid(ContextEntity)) {
			VeLogErrorFunc("failed to find the context entity");
			ExecuteNextState();
		}

		// Get the blackboard component of the target.
		auto* TargetBehavior = TargetEntity->GetBehaviorComponent();
		if (!IsValid(TargetBehavior)) {
			VeLogErrorFunc("failed to find the behavior component of the target");
			ExecuteNextState();
		}

		FVeAiSimpleFsmState TargetState;
		TargetState.ActionType = EVeAiSimpleFsmActionType::Take;
		TargetState.NPC = State.Target;
		TargetState.Target = State.NPC;
		TargetState.Context = State.Context;
		TargetState.SubjectEntity = TargetEntity;
		TargetState.TargetEntity = Subject;
		TargetState.ContextEntity = ContextEntity;

		// Update the current target state.
		TargetBehavior->SetState(TargetState);

		break;
	}
	case EVeAiSimpleFsmActionType::Use: {
		SubjectBehavior->SetState(State);
		break;
	}
	default:
		VeLogErrorFunc("unknown action: %s", *State.Action);
		ExecuteNextState();
		break;
	}
}
