// Authors: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022-2023 VeVerse AS. All rights reserved.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "VeAiSimpleFsmBehaviorComponent.h"

#include "AIController.h"
#include "VeAI.h"
#include "VeAiBlackboardKeys.h"
#include "VeAiSimpleFsmEntityComponent.h"
#include "VeAiSimpleFsmSubjectComponent.h"
#include "VeShared.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values for this component's properties
UVeAiSimpleFsmBehaviorComponent::UVeAiSimpleFsmBehaviorComponent() {
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UVeAiSimpleFsmBehaviorComponent::BeginPlay() {
	Super::BeginPlay();

	SetIsReplicated(true);

}


// Called every frame
void UVeAiSimpleFsmBehaviorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UVeAiSimpleFsmBehaviorComponent::OnTargetPerceptionUpdated(AActor* Actor, const FAIStimulus& Stimulus) {
	// todo: process visual and audio stimuli
}

void UVeAiSimpleFsmBehaviorComponent::InitializeWithController(AAIController* NewController) {
	ControllerRef = NewController;

	if (auto* Pawn = ControllerRef->GetPawn(); IsValid(Pawn) && Pawn->IsA(ACharacter::StaticClass())) {
		CharacterRef = Cast<ACharacter>(Pawn);
	}

	if (!IsValid(BehaviorTreeRef)) {
		VeLogErrorFunc("failed to run behavior tree: behavior tree reference is invalid");
		return;
	}

	ControllerRef->RunBehaviorTree(BehaviorTreeRef);
}

AVeAiSimpleFsmManager* UVeAiSimpleFsmBehaviorComponent::GetFsmManager() const {
	return FsmManagerRef;
}

void UVeAiSimpleFsmBehaviorComponent::SetFsmManager(AVeAiSimpleFsmManager* NewManager) {
	FsmManagerRef = NewManager;
}

FVeAiSimpleFsmState UVeAiSimpleFsmBehaviorComponent::GetState() const {
	return CurrentState;
}

void UVeAiSimpleFsmBehaviorComponent::SetState(const FVeAiSimpleFsmState NewState) {
	CurrentState = NewState;

	SetSubject(NewState.NPC);
	SetAction(NewState.Action);
	SetTarget(NewState.Target);
	SetContext(NewState.Context);
	SetActionType(NewState.ActionType);
	SetMetadata(NewState.Metadata);
	SetEmotion(NewState.Emotion);

	if (IsValid(NewState.SubjectEntity)) {
		SetSubjectEntity(NewState.SubjectEntity);
		SetSubjectActor(NewState.SubjectEntity->GetOwner());
	}

	if (IsValid(NewState.TargetEntity)) {
		SetTargetEntity(NewState.TargetEntity);
		SetTargetActor(NewState.TargetEntity->GetOwner());
	}

	if (IsValid(NewState.ContextEntity)) {
		SetContextEntity(NewState.ContextEntity);
		SetContextActor(NewState.ContextEntity->GetOwner());
	}
}

void UVeAiSimpleFsmBehaviorComponent::SetSubject(const FString& Subject) const {
	if (IsValid(ControllerRef)) {
		const auto KeyName = VeAiBlackboardKeys::Subject;
		if (auto* BlackboardComponent = ControllerRef->GetBlackboardComponent(); IsValid(BlackboardComponent)) {
			BlackboardComponent->SetValueAsString(KeyName, Subject);
		}
	}
}

void UVeAiSimpleFsmBehaviorComponent::SetAction(const FString& Action) const {
	if (IsValid(ControllerRef)) {
		const auto KeyName = VeAiBlackboardKeys::Action;
		if (auto* BlackboardComponent = ControllerRef->GetBlackboardComponent(); IsValid(BlackboardComponent)) {
			BlackboardComponent->SetValueAsString(KeyName, Action);
		}
	}
}

void UVeAiSimpleFsmBehaviorComponent::SetContext(const FString& Context) const {
	if (IsValid(ControllerRef)) {
		const auto KeyName = VeAiBlackboardKeys::Context;
		if (auto* BlackboardComponent = ControllerRef->GetBlackboardComponent(); IsValid(BlackboardComponent)) {
			BlackboardComponent->SetValueAsString(KeyName, Context);
		}
	}
}

void UVeAiSimpleFsmBehaviorComponent::SetTarget(const FString& Target) const {
	if (IsValid(ControllerRef)) {
		const auto KeyName = VeAiBlackboardKeys::Target;
		if (auto* BlackboardComponent = ControllerRef->GetBlackboardComponent(); IsValid(BlackboardComponent)) {
			BlackboardComponent->SetValueAsString(KeyName, Target);
		}
	}
}

void UVeAiSimpleFsmBehaviorComponent::SetMetadata(const FString& Metadata) const {
	if (IsValid(ControllerRef)) {
		const auto KeyName = VeAiBlackboardKeys::Metadata;
		if (auto* BlackboardComponent = ControllerRef->GetBlackboardComponent(); IsValid(BlackboardComponent)) {
			BlackboardComponent->SetValueAsString(KeyName, Metadata);
		}
	}
}

void UVeAiSimpleFsmBehaviorComponent::SetEmotion(const FString& Emotion) const {
	if (IsValid(ControllerRef)) {
		const auto KeyName = VeAiBlackboardKeys::Emotion;
		if (auto* BlackboardComponent = ControllerRef->GetBlackboardComponent(); IsValid(BlackboardComponent)) {
			BlackboardComponent->SetValueAsString(KeyName, Emotion);
		}
	}
}

void UVeAiSimpleFsmBehaviorComponent::SetActionType(EVeAiSimpleFsmActionType NewAction) const {
	if (IsValid(ControllerRef)) {
		const auto KeyName = VeAiBlackboardKeys::ActionType;
		if (auto* BlackboardComponent = ControllerRef->GetBlackboardComponent(); IsValid(BlackboardComponent)) {
			BlackboardComponent->SetValueAsEnum(KeyName, static_cast<uint8>(NewAction));
		}
	}
}

void UVeAiSimpleFsmBehaviorComponent::SetSubjectEntity(UVeAiSimpleFsmSubjectComponent* NewSubject) const {
	if (IsValid(ControllerRef)) {
		const auto KeyName = VeAiBlackboardKeys::SubjectEntity;
		if (auto* BlackboardComponent = ControllerRef->GetBlackboardComponent(); IsValid(BlackboardComponent)) {
			BlackboardComponent->SetValueAsObject(KeyName, NewSubject);
		}
	}
}

void UVeAiSimpleFsmBehaviorComponent::SetTargetEntity(UVeAiSimpleFsmEntityComponent* NewTarget) const {
	if (IsValid(ControllerRef)) {
		const auto KeyName = VeAiBlackboardKeys::TargetEntity;
		if (auto* BlackboardComponent = ControllerRef->GetBlackboardComponent(); IsValid(BlackboardComponent)) {
			BlackboardComponent->SetValueAsObject(KeyName, NewTarget);
		}
	}
}

void UVeAiSimpleFsmBehaviorComponent::SetContextEntity(UVeAiSimpleFsmEntityComponent* NewContext) const {
	if (IsValid(ControllerRef)) {
		const auto KeyName = VeAiBlackboardKeys::ContextEntity;
		if (auto* BlackboardComponent = ControllerRef->GetBlackboardComponent(); IsValid(BlackboardComponent)) {
			BlackboardComponent->SetValueAsObject(KeyName, NewContext);
		}
	}
}

void UVeAiSimpleFsmBehaviorComponent::SetSubjectActor(AActor* NewSubject) const {
	if (IsValid(ControllerRef)) {
		const auto KeyName = VeAiBlackboardKeys::SubjectActor;
		if (auto* BlackboardComponent = ControllerRef->GetBlackboardComponent(); IsValid(BlackboardComponent)) {
			BlackboardComponent->SetValueAsObject(KeyName, NewSubject);
		}
	}
}

void UVeAiSimpleFsmBehaviorComponent::SetTargetActor(AActor* NewTarget) const {
	if (IsValid(ControllerRef)) {
		const auto KeyName = VeAiBlackboardKeys::TargetActor;
		if (auto* BlackboardComponent = ControllerRef->GetBlackboardComponent(); IsValid(BlackboardComponent)) {
			BlackboardComponent->SetValueAsObject(KeyName, NewTarget);
		}
	}
}

void UVeAiSimpleFsmBehaviorComponent::SetContextActor(AActor* NewContext) const {
	if (IsValid(ControllerRef)) {
		const auto KeyName = VeAiBlackboardKeys::ContextActor;
		if (auto* BlackboardComponent = ControllerRef->GetBlackboardComponent(); IsValid(BlackboardComponent)) {
			BlackboardComponent->SetValueAsObject(KeyName, NewContext);
		}
	}
}

FString UVeAiSimpleFsmBehaviorComponent::GetSubject() const {
	if (IsValid(ControllerRef)) {
		const auto KeyName = VeAiBlackboardKeys::Subject;
		if (auto* BlackboardComponent = ControllerRef->GetBlackboardComponent(); IsValid(BlackboardComponent)) {
			return BlackboardComponent->GetValueAsString(KeyName);
		}
	}

	return {};
}

FString UVeAiSimpleFsmBehaviorComponent::GetAction() const {
	if (IsValid(ControllerRef)) {
		const auto KeyName = VeAiBlackboardKeys::Action;
		if (auto* BlackboardComponent = ControllerRef->GetBlackboardComponent(); IsValid(BlackboardComponent)) {
			return BlackboardComponent->GetValueAsString(KeyName);
		}
	}

	return {};
}

FString UVeAiSimpleFsmBehaviorComponent::GetContext() const {
	if (IsValid(ControllerRef)) {
		const auto KeyName = VeAiBlackboardKeys::Context;
		if (auto* BlackboardComponent = ControllerRef->GetBlackboardComponent(); IsValid(BlackboardComponent)) {
			return BlackboardComponent->GetValueAsString(KeyName);
		}
	}

	return {};
}

FString UVeAiSimpleFsmBehaviorComponent::GetTarget() const {
	if (IsValid(ControllerRef)) {
		const auto KeyName = VeAiBlackboardKeys::Target;
		if (auto* BlackboardComponent = ControllerRef->GetBlackboardComponent(); IsValid(BlackboardComponent)) {
			return BlackboardComponent->GetValueAsString(KeyName);
		}
	}

	return {};
}

FString UVeAiSimpleFsmBehaviorComponent::GetMetadata() const {
	if (IsValid(ControllerRef)) {
		const auto KeyName = VeAiBlackboardKeys::Metadata;
		if (auto* BlackboardComponent = ControllerRef->GetBlackboardComponent(); IsValid(BlackboardComponent)) {
			return BlackboardComponent->GetValueAsString(KeyName);
		}
	}

	return {};
}

EVeAiSimpleFsmActionType UVeAiSimpleFsmBehaviorComponent::GetActionType() const {
	if (IsValid(ControllerRef)) {
		const auto KeyName = VeAiBlackboardKeys::ActionType;
		if (const auto* BlackboardComponent = ControllerRef->GetBlackboardComponent(); IsValid(BlackboardComponent)) {
			return static_cast<EVeAiSimpleFsmActionType>(BlackboardComponent->GetValueAsEnum(KeyName));
		}
	}

	return EVeAiSimpleFsmActionType::None;
}

UVeAiSimpleFsmSubjectComponent* UVeAiSimpleFsmBehaviorComponent::GetSubjectEntity() const {
	if (IsValid(ControllerRef)) {
		const auto KeyName = VeAiBlackboardKeys::SubjectEntity;
		if (const auto* BlackboardComponent = ControllerRef->GetBlackboardComponent(); IsValid(BlackboardComponent)) {
			if (auto* Object = BlackboardComponent->GetValueAsObject(KeyName); IsValid(Object)) {
				return Cast<UVeAiSimpleFsmSubjectComponent>(Object);
			}
		}
	}

	return nullptr;
}

UVeAiSimpleFsmEntityComponent* UVeAiSimpleFsmBehaviorComponent::GetTargetEntity() const {
	if (IsValid(ControllerRef)) {
		const auto KeyName = VeAiBlackboardKeys::TargetEntity;
		if (const auto* BlackboardComponent = ControllerRef->GetBlackboardComponent(); IsValid(BlackboardComponent)) {
			if (auto* Object = BlackboardComponent->GetValueAsObject(KeyName); IsValid(Object)) {
				return Cast<UVeAiSimpleFsmEntityComponent>(Object);
			}
		}
	}

	return nullptr;
}

UVeAiSimpleFsmEntityComponent* UVeAiSimpleFsmBehaviorComponent::GetContextEntity() const {
	if (IsValid(ControllerRef)) {
		const auto KeyName = VeAiBlackboardKeys::ContextEntity;
		if (const auto* BlackboardComponent = ControllerRef->GetBlackboardComponent(); IsValid(BlackboardComponent)) {
			if (auto* Object = BlackboardComponent->GetValueAsObject(KeyName); IsValid(Object)) {
				return Cast<UVeAiSimpleFsmEntityComponent>(Object);
			}
		}
	}

	return nullptr;
}

AActor* UVeAiSimpleFsmBehaviorComponent::GetSubjectActor() const {
	if (IsValid(ControllerRef)) {
		const auto KeyName = VeAiBlackboardKeys::SubjectActor;
		if (const auto* BlackboardComponent = ControllerRef->GetBlackboardComponent(); IsValid(BlackboardComponent)) {
			if (auto* Object = BlackboardComponent->GetValueAsObject(KeyName); IsValid(Object)) {
				return Cast<AActor>(Object);
			}
		}
	}

	return nullptr;
}

AActor* UVeAiSimpleFsmBehaviorComponent::GetTargetActor() const {
	if (IsValid(ControllerRef)) {
		const auto KeyName = VeAiBlackboardKeys::TargetActor;
		if (const auto* BlackboardComponent = ControllerRef->GetBlackboardComponent(); IsValid(BlackboardComponent)) {
			if (auto* Object = BlackboardComponent->GetValueAsObject(KeyName); IsValid(Object)) {
				return Cast<AActor>(Object);
			}
		}
	}

	return nullptr;
}

AActor* UVeAiSimpleFsmBehaviorComponent::GetContextActor() const {
	if (IsValid(ControllerRef)) {
		const auto KeyName = VeAiBlackboardKeys::ContextActor;
		if (const auto* BlackboardComponent = ControllerRef->GetBlackboardComponent(); IsValid(BlackboardComponent)) {
			if (auto* Object = BlackboardComponent->GetValueAsObject(KeyName); IsValid(Object)) {
				return Cast<AActor>(Object);
			}
		}
	}

	return nullptr;
}
