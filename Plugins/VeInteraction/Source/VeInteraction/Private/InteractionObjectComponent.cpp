// Author: Egor A. Pristavka. 
// Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#include "Components/InteractionObjectComponent.h"

#include "VeShared.h"
#include "Interaction.h"
#include "Components/InteractionSubjectComponent.h"

void UInteractionObjectComponent::Server_FocusBegin(UInteractionSubjectComponent* Subject,
	const FVector_NetQuantize& Origin,
	const FVector_NetQuantize& Location,
	const FVector_NetQuantizeNormal& Normal) {

	OnServerFocusBegin.Broadcast(Subject, this, Origin, Location, Normal);
}

void UInteractionObjectComponent::Client_FocusBegin(UInteractionSubjectComponent* Subject,
	const FVector_NetQuantize& Origin,
	const FVector_NetQuantize& Location,
	const FVector_NetQuantizeNormal& Normal) {

	OnClientFocusBegin.Broadcast(Subject, this, Origin, Location, Normal);
}

void UInteractionObjectComponent::Server_FocusEnd(UInteractionSubjectComponent* Subject) {
	OnServerFocusEnd.Broadcast(Subject, this);
}

void UInteractionObjectComponent::Client_FocusEnd(UInteractionSubjectComponent* Subject) {
	OnClientFocusEnd.Broadcast(Subject, this);
}

void UInteractionObjectComponent::Server_OverlapBegin(UInteractionSubjectComponent* Subject) {
	OnServerOverlapBegin.Broadcast(Subject, this);
}

void UInteractionObjectComponent::Client_OverlapBegin(UInteractionSubjectComponent* Subject) {
	OnClientOverlapBegin.Broadcast(Subject, this);
}

void UInteractionObjectComponent::Server_OverlapEnd(UInteractionSubjectComponent* Subject) {
	OnServerOverlapEnd.Broadcast(Subject, this);
}

void UInteractionObjectComponent::Client_OverlapEnd(UInteractionSubjectComponent* Subject) {
	OnClientOverlapEnd.Broadcast(Subject, this);
}

void UInteractionObjectComponent::Server_TargetBegin(UInteractionSubjectComponent* Subject) {
	OnServerTargetBegin.Broadcast(Subject, this);
}

void UInteractionObjectComponent::Client_TargetBegin(UInteractionSubjectComponent* Subject) {
	OnClientTargetBegin.Broadcast(Subject, this);
}

void UInteractionObjectComponent::Server_TargetEnd(UInteractionSubjectComponent* Subject) {
	OnServerTargetEnd.Broadcast(Subject, this);
}

void UInteractionObjectComponent::Client_TargetEnd(UInteractionSubjectComponent* Subject) {
	OnClientTargetEnd.Broadcast(Subject, this);
}

void UInteractionObjectComponent::Client_InputAction(UInteractionSubjectComponent* Subject, ETriggerEvent TriggerEvent, FVector ActionValue, float ElapsedSeconds, float TriggeredSeconds, const UInputAction* InputAction) {
	OnClientInputAction.Broadcast(Subject, this, TriggerEvent, ActionValue, ElapsedSeconds, TriggeredSeconds, InputAction);
}

void UInteractionObjectComponent::Server_InputAction(UInteractionSubjectComponent* Subject, ETriggerEvent TriggerEvent, FVector ActionValue, float ElapsedSeconds, float TriggeredSeconds, const UInputAction* InputAction) {
	OnServerInputAction.Broadcast(Subject, this, TriggerEvent, ActionValue, ElapsedSeconds, TriggeredSeconds, InputAction);
}

void UInteractionObjectComponent::Client_Event_Binary(UInteractionSubjectComponent* Subject, const FName& Name, const TArray<uint8>& Payload) {
	OnClientBinaryEvent.Broadcast(Subject, this, Name, Payload);
}

void UInteractionObjectComponent::Server_Event_Binary(UInteractionSubjectComponent* Subject, const FName& Name, const TArray<uint8>& Payload) {
	OnServerBinaryEvent.Broadcast(Subject, this, Name, Payload);
}

void UInteractionObjectComponent::Client_Event_Object(UInteractionSubjectComponent* Subject, const FName& Name, UObject* Payload) {
	OnClientObjectEvent.Broadcast(Subject, this, Name, Payload);
}

void UInteractionObjectComponent::Server_Event_Object(UInteractionSubjectComponent* Subject, const FName& Name, UObject* Payload) {
	OnServerObjectEvent.Broadcast(Subject, this, Name, Payload);
}

void UInteractionObjectComponent::Client_Event_String(UInteractionSubjectComponent* Subject, const FName& Name, const FString& Payload) {
	OnClientStringEvent.Broadcast(Subject, this, Name, Payload);
}

void UInteractionObjectComponent::Server_Event_String(UInteractionSubjectComponent* Subject, const FName& Name, const FString& Payload) {
	OnServerStringEvent.Broadcast(Subject, this, Name, Payload);
}
