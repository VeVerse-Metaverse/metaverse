// Authors: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022-2023 VeVerse AS. All rights reserved.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "CogAiMessage.h"

#include "Api2AiSubsystem.h"
#include "Api2CogAiRequest.h"
#include "EngineUtils.h"
#include "VeShared.h"
#include "VeAI.h"
#include "VeGameFramework.h"

UCogAiMessage::UCogAiMessage(const FApi2CogAiMessage& Message) {
	// Determine the type of the message (it should be either "action" or "system")
	const FString Key = Message.GetKey();
	if (Key.IsEmpty()) {
		return;
	}

	// Get the map of parameters as a JSON object.
	const auto Params = Message.Content->GetObjectField(Key);
	if (!Params) {
		return;
	}

	// Enumerate all keys of the system object
	for (const auto& Param : Params->Values) {
		// Add the key and value to the parameters array
		Parameters.Add(Param.Key, Param.Value->AsString());
	}
}

UCogAiMessage::operator FApi2CogAiMessage() const {
	return ToApi2Message();
}

FApi2CogAiMessage UCogAiMessage::ToApi2Message() const {
	FApi2CogAiMessage Message;
	switch (Role) {
	case ECogAiMessageRole::System:
		Message.From = TEXT("system");
		break;
	case ECogAiMessageRole::Npc:
		Message.From = TEXT("npc");
		break;
	default:
		break;
	}
	Message.Content = MakeShareable(new FJsonObject);
	return Message;
}

FString UCogAiMessage::ToString() const {
	FString Result = "\n---\nMessage: Generic\nRole: " + RoleToString();

	if (Parameters.Num() > 0) {
		Result += "\nParameters:\n";
		for (const auto& Entry : Parameters) {
			Result += Entry.Key + ": " + Entry.Value + "\n";
		}
	}

	return Result;
}

bool UCogAiMessage::operator==(const UCogAiMessage& Other) const {
	bool bEqual = Role == Other.Role;

	for (const auto& Elem : Parameters) {
		const auto& Key = Elem.Key;
		const auto& Value1 = Elem.Value;
		if (const auto* Value2Ptr = Other.Parameters.Find(Key); !Value2Ptr || *Value2Ptr != Value1) {
			bEqual = false;
			break;
		}
	}

	if (bEqual && Parameters.Num() == Other.Parameters.Num()) {
		return true;
	}

	return false;
}

FString UCogAiMessage::RoleToString() const {
	switch (Role) {
	case ECogAiMessageRole::System:
		return TEXT("System");
	case ECogAiMessageRole::Npc:
		return TEXT("NPC");
	default:
		return TEXT("None");
	}
}

UCogAiActionMessage::UCogAiActionMessage(const FApi2CogAiMessage& Message)
	: UCogAiMessage(Message) {}

bool UCogAiActionMessage::operator==(const UCogAiMessage& Other) const {
	if (auto* OtherCast = Cast<UCogAiActionMessage>(&Other)) {
		return OtherCast->Type == Type && UCogAiMessage::operator==(Other);
	}

	return false;
}

FApi2CogAiMessage UCogAiActionMessage::ToApi2Message() const {
	const FApi2CogAiMessage Message = Super::ToApi2Message();
	const TSharedPtr<FJsonObject> ActionJson = MakeShareable(new FJsonObject);
	for (const auto& Parameter : Parameters) {
		ActionJson->SetStringField(Parameter.Key, Parameter.Value);
	}
	Message.Content->SetObjectField(TEXT("action"), ActionJson);
	return Message;
}

FString UCogAiActionMessage::ToString() const {
	FString Result = "\n---\nMessage: Action\nRole: " + RoleToString();

	Result += "\nType: " + Type;

	if (Parameters.Num() > 0) {
		Result += "\nParameters:\n";
		for (const auto& Entry : Parameters) {
			Result += Entry.Key + ": " + Entry.Value + "\n";
		}
	}

	return Result;
}

FCogAiActionMessageReplicatedParameter::FCogAiActionMessageReplicatedParameter(const FString& InName, const FString& InValue) {
	Name = InName;
	Value = InValue;
}

FCogAiActionMessageReplicated::FCogAiActionMessageReplicated(UCogAiActionMessage* InMessage) {
	if (IsValid(InMessage)) {
		Type = InMessage->Type;
		for (const auto& Param : InMessage->Parameters) {
			RawParameters.Add(FCogAiActionMessageReplicatedParameter(Param.Key, Param.Value));
		}
	}
}

FCogAiActionMessageClient FCogAiActionMessageReplicated::ToClientMessage() const {
	FCogAiActionMessageClient Result;
	Result.Type = Type;
	for (auto& RawParameter : RawParameters) {
		Result.Parameters.Add(RawParameter.Name, RawParameter.Value);
	}
	return Result;
}

UCogAiSystemMessage::UCogAiSystemMessage(const FApi2CogAiMessage& Message)
	: UCogAiMessage(Message) {
	// Determine the type of the message (it should be either "action" or "system")
	const FString Key = Message.GetKey();
	if (Key.IsEmpty()) {
		return;
	}

	const auto Params = Message.Content->GetObjectField(Key);

	if (FString Query; Params->TryGetStringField(TEXT("query"), Query)) {
		if (Query == TEXT("whoami")) {
			Type = ECogAiSystemMessageType::WhoAmI;
		} else if (Query == TEXT("actions")) {
			Type = ECogAiSystemMessageType::Actions;
		} else if (Query == TEXT("context")) {
			Type = ECogAiSystemMessageType::Context;
		} else if (Query == TEXT("perception")) {
			Type = ECogAiSystemMessageType::Perception;
		} else if (Query == TEXT("inspect")) {
			Type = ECogAiSystemMessageType::Inspect;
		}
	}
}

bool UCogAiSystemMessage::operator==(const UCogAiMessage& Other) const {
	if (auto* OtherCast = Cast<UCogAiSystemMessage>(&Other)) {
		return OtherCast->Type == Type && Super::operator==(Other);
	}

	return false;
}

FApi2CogAiMessage UCogAiSystemMessage::ToApi2Message() const {
	const FApi2CogAiMessage Message = Super::ToApi2Message();
	switch (Type) {
	case ECogAiSystemMessageType::Actions:
		Message.Content->SetStringField(TEXT("query"), TEXT("actions"));
		break;
	case ECogAiSystemMessageType::Context:
		Message.Content->SetStringField(TEXT("query"), TEXT("context"));
		break;
	case ECogAiSystemMessageType::Inspect:
		Message.Content->SetStringField(TEXT("query"), TEXT("inspect"));
		break;
	case ECogAiSystemMessageType::Perception:
		Message.Content->SetStringField(TEXT("query"), TEXT("perception"));
		break;
	case ECogAiSystemMessageType::WhoAmI:
		Message.Content->SetStringField(TEXT("query"), TEXT("whoami"));
		break;
	default:
		VeLogError("Unknown system message type");
		break;
	}
	return Message;
}

FString UCogAiSystemMessage::ToString() const {
	FString Result = "\n---\nMessage: System\nRole: " + RoleToString();

	Result += "\nType: " + FString::FromInt(static_cast<int32>(Type));

	if (Parameters.Num() > 0) {
		Result += "\nParameters:\n";

		for (const auto& Entry : Parameters) {
			Result += Entry.Key + ": " + Entry.Value + "\n";
		}
	}

	return Result;
}

FString UCogAiSystemMessage::SystemTypeToString() const {
	switch (Type) {
	case ECogAiSystemMessageType::Actions:
		return TEXT("Actions");
	case ECogAiSystemMessageType::Context:
		return TEXT("Context");
	case ECogAiSystemMessageType::Inspect:
		return TEXT("Inspect");
	case ECogAiSystemMessageType::Perception:
		return TEXT("Perception");
	case ECogAiSystemMessageType::WhoAmI:
		return TEXT("WhoAmI");
	default:
		return TEXT("None");
	}
}

UCogAiSystemQueryMessage::UCogAiSystemQueryMessage(const FApi2CogAiMessage& Message)
	: UCogAiSystemMessage(Message) {}

bool UCogAiSystemQueryMessage::operator==(const UCogAiMessage& Other) const {
	if (auto* OtherCast = Cast<UCogAiSystemQueryMessage>(&Other)) {
		return OtherCast->Type == Type && Super::operator==(Other);
	}

	return false;
}

FApi2CogAiMessage UCogAiSystemQueryMessage::ToApi2Message() const {
	const FApi2CogAiMessage Message = Super::ToApi2Message();
	for (const auto& Parameter : Parameters) {
		Message.Content->SetStringField(Parameter.Key, Parameter.Value);
	}
	switch (Type) {
	case ECogAiSystemMessageType::Actions:
		Message.Content->SetStringField(TEXT("query"), TEXT("actions"));
		break;
	case ECogAiSystemMessageType::Context:
		Message.Content->SetStringField(TEXT("query"), TEXT("context"));
		break;
	case ECogAiSystemMessageType::Inspect:
		Message.Content->SetStringField(TEXT("query"), TEXT("inspect"));
		if (auto* Target = Parameters.Find(TEXT("target")); Target) {
			Message.Content->SetStringField(TEXT("target"), *Target);
		}
		break;
	case ECogAiSystemMessageType::Perception:
		Message.Content->SetStringField(TEXT("query"), TEXT("perception"));
		break;
	case ECogAiSystemMessageType::WhoAmI:
		Message.Content->SetStringField(TEXT("query"), TEXT("whoami"));
		break;
	default:
		VeLogError("Unknown system message type");
		break;
	}
	return Message;
}

FString UCogAiSystemQueryMessage::ToString() const {
	FString Result = "\n---\nMessage: System Query\nRole: " + RoleToString();

	Result += "\nType: " + SystemTypeToString();

	if (Parameters.Num() > 0) {
		Result += "\nParameters:\n";
		for (const auto& Entry : Parameters) {
			Result += Entry.Key + ": " + Entry.Value + "\n";
		}
	}

	return Result;
}

UCogAiWhoamiSystemMessage::UCogAiWhoamiSystemMessage() {
	Type = ECogAiSystemMessageType::WhoAmI;
}

UCogAiWhoamiSystemMessage::UCogAiWhoamiSystemMessage(const FApi2CogAiMessage& Message)
	: UCogAiSystemMessage(Message) {}

FApi2CogAiMessage UCogAiWhoamiSystemMessage::ToApi2Message() const {
	FApi2CogAiMessage Message = Super::ToApi2Message();
	Message.Content->SetStringField(TEXT("name"), Name);
	Message.Content->SetStringField(TEXT("desc"), Description);
	Message.Content->SetStringField(TEXT("faction"), Faction);

	const TSharedPtr<FJsonObject> PersonalityJson = MakeShareable(new FJsonObject());
	for (const auto& PersonalityProperty : Personality) {
		if (PersonalityProperty.Value < 0) {
			PersonalityJson->SetStringField(PersonalityProperty.Key, TEXT("low"));
		} else if (PersonalityProperty.Value == 0) {
			PersonalityJson->SetStringField(PersonalityProperty.Key, TEXT("medium"));
		} else {
			PersonalityJson->SetStringField(PersonalityProperty.Key, TEXT("high"));
		}
	}

	Message.Content->SetObjectField(TEXT("personality"), PersonalityJson);

	return Message;
}

FString UCogAiWhoamiSystemMessage::ToString() const {
	FString Result = "\n---\nMessage: System Whoami\nRole: " + RoleToString();

	Result += "\nType: " + SystemTypeToString();
	Result += "\nName: " + Name;
	Result += "\nDescription: " + Description;
	Result += "\nFaction: " + Faction;
	Result += "\nPersonality:\n";

	for (const auto& Entry : Personality) {
		Result += Entry.Key + ": " + (Entry.Value > 0 ? "High" : Entry.Value < 0 ? "Low" : "Medium") + "\n";
	}

	if (Parameters.Num() > 0) {
		Result += "\nParameters:\n";
		for (const auto& Entry : Parameters) {
			Result += Entry.Key + ": " + Entry.Value + "\n";
		}
	}

	return Result;
}

bool UCogAiWhoamiSystemMessage::operator==(const UCogAiMessage& Other) const {
	if (auto* OtherCast = Cast<UCogAiWhoamiSystemMessage>(&Other)) {
		// Compare maps
		if (OtherCast->Personality.Num() != Personality.Num()) {
			return false;
		}

		for (const auto& Entry : Personality) {
			if (!OtherCast->Personality.Contains(Entry.Key) || OtherCast->Personality[Entry.Key] != Entry.Value) {
				return false;
			}
		}

		return OtherCast->Name == Name && OtherCast->Description == Description && OtherCast->Faction == Faction && Super::operator==(Other);
	}

	return false;
}

UCogAiActionsSystemMessage::UCogAiActionsSystemMessage() {
	Type = ECogAiSystemMessageType::Actions;
}

UCogAiActionsSystemMessage::UCogAiActionsSystemMessage(const FApi2CogAiMessage& Message)
	: UCogAiSystemMessage(Message) { }

bool UCogAiActionsSystemMessage::operator==(const UCogAiMessage& Other) const {
	if (auto* OtherCast = Cast<UCogAiActionsSystemMessage>(&Other)) {
		// Compare actions
		if (OtherCast->Actions != Actions) {
			return false;
		}
	}

	return false;
}

FApi2CogAiMessage UCogAiActionsSystemMessage::ToApi2Message() const {
	FApi2CogAiMessage Message = Super::ToApi2Message();

	TArray<TSharedPtr<FJsonValue>> ActionJsonArray;

	// Populate the actions array with actions.
	for (const auto& Action : Actions) {
		const TSharedPtr<FJsonObject> ActionJson = MakeShareable(new FJsonObject());
		// Set action type.
		ActionJson->SetStringField(TEXT("type"), Action.Type);
		// Set action parameters and their descriptions.
		for (const auto& Parameter : Action.Parameters) {
			ActionJson->SetStringField(Parameter.Name, Parameter.Description);
		}
		ActionJsonArray.Add(MakeShareable(new FJsonValueObject(ActionJson)));
	}

	Message.Content->SetArrayField(TEXT("actions"), ActionJsonArray);

	return Message;
}

FString UCogAiActionsSystemMessage::ToString() const {
	FString Result = "\n---\nMessage: System Actions\nRole: " + RoleToString();

	Result += "\nType: " + SystemTypeToString();
	Result += "\nActions:\n";

	for (const auto& Action : Actions) {
		Result += Action.Type + "\n";
		for (const auto& Parameter : Action.Parameters) {
			Result += Parameter.Name + ": " + Parameter.Description + "\n";
		}
	}

	if (Parameters.Num() > 0) {
		Result += "\nParameters:\n";
		for (const auto& Parameter : Parameters) {
			Result += Parameter.Key + ": " + Parameter.Value + "\n";
		}
	}

	return Result;
}

UCogAiContextSystemMessage::UCogAiContextSystemMessage() {
	Type = ECogAiSystemMessageType::Context;
}

UCogAiContextSystemMessage::UCogAiContextSystemMessage(const FApi2CogAiMessage& Message) {}

bool UCogAiContextSystemMessage::operator==(const UCogAiMessage& Other) const {
	if (auto* OtherCast = Cast<UCogAiContextSystemMessage>(&Other)) {
		return OtherCast->Description == Description && OtherCast->Location == Location && OtherCast->Time == Time && OtherCast->Weather == Weather && OtherCast->Vibe == Vibe && Super::operator==(Other);
	}

	return false;
}

FApi2CogAiMessage UCogAiContextSystemMessage::ToApi2Message() const {
	auto Message = Super::ToApi2Message();
	Message.Content->SetStringField(TEXT("query"), TEXT("context"));
	Message.Content->SetStringField(TEXT("desc"), Description);
	Message.Content->SetStringField(TEXT("location"), Location);
	Message.Content->SetStringField(TEXT("time"), Time);
	Message.Content->SetStringField(TEXT("weather"), Weather);
	Message.Content->SetStringField(TEXT("vibe"), Vibe);
	return Message;
}

FString UCogAiContextSystemMessage::ToString() const {
	FString Result = "\n---\nMessage: System Context\nRole: " + RoleToString();

	Result += "\nType: " + SystemTypeToString();
	Result += "\nDescription: " + Description;
	Result += "\nLocation: " + Location;
	Result += "\nTime: " + Time;
	Result += "\nWeather: " + Weather;
	Result += "\nVibe: " + Vibe;

	if (Parameters.Num() > 0) {
		Result += "\nParameters:\n";
		for (const auto& Entry : Parameters) {
			Result += Entry.Key + ": " + Entry.Value + "\n";
		}
	}

	return Result;
}

UCogAiInspectSystemMessage::UCogAiInspectSystemMessage() {
	Type = ECogAiSystemMessageType::Inspect;
}

UCogAiInspectSystemMessage::UCogAiInspectSystemMessage(const FApi2CogAiMessage& Message)
	: UCogAiSystemMessage(Message) {}

bool UCogAiInspectSystemMessage::operator==(const UCogAiMessage& Other) const {
	if (auto* OtherCast = Cast<UCogAiInspectSystemMessage>(&Other)) {
		return OtherCast->TargetName == TargetName && OtherCast->TargetDescription == TargetDescription && Super::operator==(Other);
	}

	return false;
}

FApi2CogAiMessage UCogAiInspectSystemMessage::ToApi2Message() const {
	auto Message = Super::ToApi2Message();
	const TSharedPtr<FJsonObject> TargetJson = MakeShareable(new FJsonObject());
	TargetJson->SetStringField(TEXT("name"), TargetName);
	TargetJson->SetStringField(TEXT("desc"), TargetDescription);
	Message.Content->SetObjectField(TEXT("target"), TargetJson);
	return Message;
}

FString UCogAiInspectSystemMessage::ToString() const {
	FString Result = "\n---\nMessage: System Inspect\nRole: " + RoleToString();

	Result += "\nType: " + SystemTypeToString();
	Result += "\nTarget Name: " + TargetName;
	Result += "\nTarget Description: " + TargetDescription;

	if (Parameters.Num() > 0) {
		Result += "\nParameters:\n";

		for (const auto& Entry : Parameters) {
			Result += Entry.Key + ": " + Entry.Value + "\n";
		}
	}

	return Result;
}

bool FCogAiPerception::operator==(const FCogAiPerception& Other) const {
	return Strength == Other.Strength && Type == Other.Type && Name == Other.Name && Description == Other.Description;
}

UCogAiPerceptionSystemMessage::UCogAiPerceptionSystemMessage() {
	Type = ECogAiSystemMessageType::Perception;
}

UCogAiPerceptionSystemMessage::UCogAiPerceptionSystemMessage(const FApi2CogAiMessage& Message)
	: UCogAiSystemMessage(Message) {}

FApi2CogAiMessage UCogAiPerceptionSystemMessage::ToApi2Message() const {
	auto Message = Super::ToApi2Message();
	TArray<TSharedPtr<FJsonValue>> VisualObjectsArray;
	TArray<TSharedPtr<FJsonValue>> AudioObjectsArray;
	TArray<TSharedPtr<FJsonValue>> OtherObjectsArray;

	for (const auto& Perception : Visual) {
		const TSharedPtr<FJsonObject> VisualObject = MakeShareable(new FJsonObject());
		VisualObject->SetStringField(TEXT("name"), Perception.Name);
		VisualObject->SetStringField(TEXT("desc"), Perception.Description);
		VisualObjectsArray.Add(MakeShareable(new FJsonValueObject(VisualObject)));
	}

	for (const auto& Perception : Audio) {
		const TSharedPtr<FJsonObject> AudioObject = MakeShareable(new FJsonObject());
		AudioObject->SetStringField(TEXT("name"), Perception.Name);
		AudioObject->SetStringField(TEXT("desc"), Perception.Description);
		AudioObjectsArray.Add(MakeShareable(new FJsonValueObject(AudioObject)));
	}

	for (const auto& Perception : Other) {
		const TSharedPtr<FJsonObject> OtherObject = MakeShareable(new FJsonObject());
		OtherObject->SetStringField(TEXT("name"), Perception.Name);
		OtherObject->SetStringField(TEXT("desc"), Perception.Description);
		OtherObjectsArray.Add(MakeShareable(new FJsonValueObject(OtherObject)));
	}

	const TSharedPtr<FJsonObject> PerceptionJson = MakeShareable(new FJsonObject());
	PerceptionJson->SetArrayField(TEXT("visual"), VisualObjectsArray);
	PerceptionJson->SetArrayField(TEXT("audio"), AudioObjectsArray);
	PerceptionJson->SetArrayField(TEXT("other"), OtherObjectsArray);

	Message.Content->SetObjectField(TEXT("perception"), PerceptionJson);

	return Message;
}

FString UCogAiPerceptionSystemMessage::ToString() const {
	FString Result = "\n---\nMessage: System Perception\nRole: " + RoleToString();

	Result += "\nType: " + SystemTypeToString();
	Result += "\nVisual:\n";

	for (const auto& Perception : Visual) {
		Result += Perception.Name + "\n";
		Result += Perception.Description + "\n";
	}

	Result += "\nAudio:\n";

	for (const auto& Perception : Audio) {
		Result += Perception.Name + "\n";
		Result += Perception.Description + "\n";
	}

	Result += "\nOther:\n";

	for (const auto& Perception : Other) {
		Result += Perception.Name + "\n";
		Result += Perception.Description + "\n";
	}

	if (Parameters.Num() > 0) {
		Result += "\nParameters:\n";

		for (const auto& Entry : Parameters) {
			Result += Entry.Key + ": " + Entry.Value + "\n";
		}
	}

	return Result;
}

bool UCogAiPerceptionSystemMessage::operator==(const UCogAiMessage& InOther) const {
	if (auto* OtherCast = Cast<UCogAiPerceptionSystemMessage>(&InOther)) {
		return Visual == OtherCast->Visual && Audio == OtherCast->Audio && Other == OtherCast->Other && Super::operator==(InOther);
	}

	return false;
}
