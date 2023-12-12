// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "ArtheonWebSocketsPayload.h"

#include "VoiceDefaults.h"
#include "ArtheonWebSocketsTypes.h"

TSharedPtr<FJsonObject> FArtheonWebSocketsUserAction::ToJson() const {
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());

	if (!Id.IsEmpty()) {
		JsonObject->SetStringField(ArtheonWebSocketsJsonFields::Id, Id);
	}

	if (!SenderId.IsEmpty()) {
		JsonObject->SetStringField(ArtheonWebSocketsJsonFields::SenderId, SenderId);
	}

	if (!UserId.IsEmpty()) {
		JsonObject->SetStringField(ArtheonWebSocketsJsonFields::UserId, UserId);
	}

	if (!Action.IsEmpty()) {
		JsonObject->SetStringField(ArtheonWebSocketsJsonFields::Action, Action);
	}

	if (!Details.IsEmpty()) {
		JsonObject->SetStringField(ArtheonWebSocketsJsonFields::Details, Details);
	}

	return JsonObject;
}

void FArtheonWebSocketsUserAction::ParseJson(const TSharedPtr<FJsonObject> JsonObject) {
	if (JsonObject->HasTypedField<EJson::String>(ArtheonWebSocketsJsonFields::Id)) {
		Id = JsonObject->GetStringField(ArtheonWebSocketsJsonFields::Id);
	}

	if (JsonObject->HasTypedField<EJson::String>(ArtheonWebSocketsJsonFields::SenderId)) {
		SenderId = JsonObject->GetStringField(ArtheonWebSocketsJsonFields::SenderId);
	}

	if (JsonObject->HasTypedField<EJson::String>(ArtheonWebSocketsJsonFields::UserId)) {
		UserId = JsonObject->GetStringField(ArtheonWebSocketsJsonFields::UserId);
	}

	if (JsonObject->HasTypedField<EJson::String>(ArtheonWebSocketsJsonFields::Action)) {
		Action = JsonObject->GetStringField(ArtheonWebSocketsJsonFields::Action);
	}

	if (JsonObject->HasTypedField<EJson::String>(ArtheonWebSocketsJsonFields::Details)) {
		Details = JsonObject->GetStringField(ArtheonWebSocketsJsonFields::Details);
	}
}

bool FArtheonWebSocketsUserAction::IsValid() const {
	return !SenderId.IsEmpty() && !UserId.IsEmpty();
}

TSharedPtr<FJsonObject> FArtheonVivoxChannelProperties::ToJson() const {
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());

	if (!Id.IsEmpty()) {
		JsonObject->SetStringField(ArtheonWebSocketsJsonFields::Id, Id);
	}

	if (ChannelType <= EArtheonVivoxChannelType::Positional) {
		JsonObject->SetNumberField(ArtheonWebSocketsJsonFields::VxChannelType, static_cast<uint8>(ChannelType));
	} else {
		JsonObject->SetNumberField(ArtheonWebSocketsJsonFields::VxChannelType, ArtheonVivoxDefaults::ChannelType);
	}
	
	if (IsPositional()) {
		if (AudibleDistance > 0) {
			JsonObject->SetNumberField(ArtheonWebSocketsJsonFields::VxAudibleDistance, AudibleDistance);
		} else {
			JsonObject->SetNumberField(ArtheonWebSocketsJsonFields::VxAudibleDistance, ArtheonVivoxDefaults::AudibleDistance);
		}
	
		if (ConversationalDistance > 0) {
			JsonObject->SetNumberField(ArtheonWebSocketsJsonFields::VxConversationalDistance, ConversationalDistance);
		} else {
			JsonObject->SetNumberField(ArtheonWebSocketsJsonFields::VxConversationalDistance, ArtheonVivoxDefaults::ConversationalDistance);
		}
	
		if (AudioFadeIntensity > 0) {
			JsonObject->SetNumberField(ArtheonWebSocketsJsonFields::VxAudioFadeIntensity, AudioFadeIntensity);
		} else {
			JsonObject->SetNumberField(ArtheonWebSocketsJsonFields::VxAudioFadeIntensity, ArtheonVivoxDefaults::AudioFadeIntensity);
		}
	
		if (AudioFadeModel >= 1 && AudioFadeModel <= 3) {
			JsonObject->SetNumberField(ArtheonWebSocketsJsonFields::VxAudioFadeModel, AudioFadeModel);
		} else {
			JsonObject->SetNumberField(ArtheonWebSocketsJsonFields::VxAudioFadeModel, ArtheonVivoxDefaults::AudioFadeModel);
		}
	}

	return JsonObject;
}

void FArtheonVivoxChannelProperties::ParseJson(const TSharedPtr<FJsonObject> JsonObject) {
	if (JsonObject->HasTypedField<EJson::String>(ArtheonWebSocketsJsonFields::Id)) {
		Id = JsonObject->GetStringField(ArtheonWebSocketsJsonFields::Id);
	}

	if (JsonObject->HasTypedField<EJson::Number>(ArtheonWebSocketsJsonFields::VxAudibleDistance)) {
		AudibleDistance = JsonObject->GetNumberField(ArtheonWebSocketsJsonFields::VxAudibleDistance);
	}

	if (JsonObject->HasTypedField<EJson::Number>(ArtheonWebSocketsJsonFields::VxConversationalDistance)) {
		ConversationalDistance = JsonObject->GetNumberField(ArtheonWebSocketsJsonFields::VxConversationalDistance);
	}

	if (JsonObject->HasTypedField<EJson::Number>(ArtheonWebSocketsJsonFields::VxAudioFadeIntensity)) {
		AudioFadeIntensity = JsonObject->GetNumberField(ArtheonWebSocketsJsonFields::VxAudioFadeIntensity);
	}

	if (JsonObject->HasTypedField<EJson::Number>(ArtheonWebSocketsJsonFields::VxAudioFadeModel)) {
		AudioFadeModel = JsonObject->GetNumberField(ArtheonWebSocketsJsonFields::VxAudioFadeModel);
	}

	if (JsonObject->HasTypedField<EJson::Number>(ArtheonWebSocketsJsonFields::VxChannelType)) {
		ChannelType = static_cast<EArtheonVivoxChannelType>(static_cast<int>(JsonObject->GetNumberField(ArtheonWebSocketsJsonFields::VxChannelType)));
	}
}

bool FArtheonVivoxChannelProperties::IsValid() const {
	return !Id.IsEmpty();
}

TSharedPtr<FJsonObject> FArtheonWebSocketsVivoxPayload::ToJson() const {
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());

	if (!Issuer.IsEmpty()) {
		JsonObject->SetStringField(ArtheonWebSocketsJsonFields::VxIssuer, Issuer);
	}

	if (ExpiresAt > 0) {
		JsonObject->SetNumberField(ArtheonWebSocketsJsonFields::VxExpiresAt, ExpiresAt);
	}

	if (!Action.IsEmpty()) {
		JsonObject->SetStringField(ArtheonWebSocketsJsonFields::VxAction, Action);
	}

	if (Serial > 0) {
		JsonObject->SetNumberField(ArtheonWebSocketsJsonFields::VxSerial, Serial);
	}

	if (!Subject.IsEmpty()) {
		JsonObject->SetStringField(ArtheonWebSocketsJsonFields::VxSubject, Subject);
	}

	if (!From.IsEmpty()) {
		JsonObject->SetStringField(ArtheonWebSocketsJsonFields::VxFrom, From);
	}

	if (!To.IsEmpty()) {
		JsonObject->SetStringField(ArtheonWebSocketsJsonFields::VxTo, To);
	}

	if (!Server.IsEmpty()) {
		JsonObject->SetStringField(ArtheonWebSocketsJsonFields::VxServer, Server);
	}

	if (ChannelProperties.IsValid()) {
		JsonObject->SetObjectField(ArtheonWebSocketsJsonFields::VxChannelProperties, ChannelProperties.ToJson());
	}

	return JsonObject;
}

void FArtheonWebSocketsVivoxPayload::ParseJson(const TSharedPtr<FJsonObject> JsonObject) {
	if (JsonObject->HasTypedField<EJson::String>(ArtheonWebSocketsJsonFields::VxIssuer)) {
		Issuer = JsonObject->GetStringField(ArtheonWebSocketsJsonFields::VxIssuer);
	}

	if (JsonObject->HasTypedField<EJson::Number>(ArtheonWebSocketsJsonFields::VxExpiresAt)) {
		ExpiresAt = JsonObject->GetNumberField(ArtheonWebSocketsJsonFields::VxExpiresAt);
	}

	if (JsonObject->HasTypedField<EJson::String>(ArtheonWebSocketsJsonFields::VxAction)) {
		Action = JsonObject->GetStringField(ArtheonWebSocketsJsonFields::VxAction);
	}

	if (JsonObject->HasTypedField<EJson::Number>(ArtheonWebSocketsJsonFields::VxSerial)) {
		Serial = JsonObject->GetNumberField(ArtheonWebSocketsJsonFields::VxSerial);
	}

	if (JsonObject->HasTypedField<EJson::String>(ArtheonWebSocketsJsonFields::VxSubject)) {
		Subject = JsonObject->GetStringField(ArtheonWebSocketsJsonFields::VxSubject);
	}

	if (JsonObject->HasTypedField<EJson::String>(ArtheonWebSocketsJsonFields::VxFrom)) {
		From = JsonObject->GetStringField(ArtheonWebSocketsJsonFields::VxFrom);
	}

	if (JsonObject->HasTypedField<EJson::String>(ArtheonWebSocketsJsonFields::VxTo)) {
		To = JsonObject->GetStringField(ArtheonWebSocketsJsonFields::VxTo);
	}

	if (JsonObject->HasTypedField<EJson::String>(ArtheonWebSocketsJsonFields::VxServer)) {
		Server = JsonObject->GetStringField(ArtheonWebSocketsJsonFields::VxServer);
	}

	if (JsonObject->HasTypedField<EJson::String>(ArtheonWebSocketsJsonFields::VxToken)) {
		Token = JsonObject->GetStringField(ArtheonWebSocketsJsonFields::VxToken);
	}

	if (JsonObject->HasTypedField<EJson::Object>(ArtheonWebSocketsJsonFields::VxChannelProperties)) {
		const auto Properties = JsonObject->GetObjectField(ArtheonWebSocketsJsonFields::VxChannelProperties);
		if (Properties.IsValid()) {
			ChannelProperties.ParseJson(Properties);
		}
	}
}

bool FArtheonWebSocketsVivoxPayload::IsValid() const {
	return !Action.IsEmpty() && !From.IsEmpty();
}

TSharedPtr<FJsonObject> FArtheonWebSocketsUser::ToJson() const {
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());

	if (!Id.IsEmpty()) {
		JsonObject->SetStringField(ArtheonWebSocketsJsonFields::Id, Id);
	}

	if (!Name.IsEmpty()) {
		JsonObject->SetStringField(ArtheonWebSocketsJsonFields::Name, Name);
	}

	if (!AvatarUrl.IsEmpty()) {
		JsonObject->SetStringField(ArtheonWebSocketsJsonFields::AvatarUrl, AvatarUrl);
	}

	return JsonObject;
}

void FArtheonWebSocketsUser::ParseJson(const TSharedPtr<FJsonObject> JsonObject) {
	if (JsonObject->HasTypedField<EJson::String>(ArtheonWebSocketsJsonFields::Id)) {
		Id = JsonObject->GetStringField(ArtheonWebSocketsJsonFields::Id);
	}

	if (JsonObject->HasTypedField<EJson::String>(ArtheonWebSocketsJsonFields::Name)) {
		Name = JsonObject->GetStringField(ArtheonWebSocketsJsonFields::Name);
	}

	if (JsonObject->HasTypedField<EJson::String>(ArtheonWebSocketsJsonFields::AvatarUrl)) {
		AvatarUrl = JsonObject->GetStringField(ArtheonWebSocketsJsonFields::AvatarUrl);
	}
}

bool FArtheonWebSocketsUser::IsValid() const {
	return !Id.IsEmpty() && !Name.IsEmpty();
}

TSharedPtr<FJsonObject> FArtheonWebSocketsPayload::ToJson() {
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());

	if (!Status.IsEmpty()) {
		JsonObject->SetStringField(ArtheonWebSocketsJsonFields::Status, Status);
	}

	if (!Message.IsEmpty()) {
		JsonObject->SetStringField(ArtheonWebSocketsJsonFields::Message, Message);
	}

	if (!ChannelId.IsEmpty()) {
		JsonObject->SetStringField(ArtheonWebSocketsJsonFields::ChannelId, ChannelId);
	}

	if (!Key.IsEmpty()) {
		JsonObject->SetStringField(ArtheonWebSocketsJsonFields::Key, Key);
	}

	if (User.IsValid()) {
		JsonObject->SetObjectField(ArtheonWebSocketsJsonFields::Sender, User.ToJson());
	}

	if (Action.IsValid()) {
		JsonObject->SetObjectField(ArtheonWebSocketsJsonFields::Message, Action.ToJson());
	}

	if (!Key.IsEmpty()) {
		JsonObject->SetStringField(ArtheonWebSocketsJsonFields::Category, Category);
	}

	if (VivoxPayload.IsValid()) {
		JsonObject->SetObjectField(ArtheonWebSocketsJsonFields::VxPayload, VivoxPayload.ToJson());
	}

	return JsonObject;
}

void FArtheonWebSocketsPayload::ParseJson(const TSharedPtr<FJsonObject> JsonObject) {
	if (JsonObject->HasTypedField<EJson::String>(ArtheonWebSocketsJsonFields::Status)) {
		Status = JsonObject->GetStringField(ArtheonWebSocketsJsonFields::Status);
	}

	if (JsonObject->HasTypedField<EJson::String>(ArtheonWebSocketsJsonFields::Message)) {
		Message = JsonObject->GetStringField(ArtheonWebSocketsJsonFields::Message);
	}

	if (JsonObject->HasTypedField<EJson::String>(ArtheonWebSocketsJsonFields::ChannelId)) {
		ChannelId = JsonObject->GetStringField(ArtheonWebSocketsJsonFields::ChannelId);
	}

	if (JsonObject->HasTypedField<EJson::String>(ArtheonWebSocketsJsonFields::Key)) {
		Key = JsonObject->GetStringField(ArtheonWebSocketsJsonFields::Key);
	}

	if (JsonObject->HasTypedField<EJson::Object>(ArtheonWebSocketsJsonFields::Sender)) {
		const auto UserField = JsonObject->GetObjectField(ArtheonWebSocketsJsonFields::Sender);
		if (UserField.IsValid()) {
			User.ParseJson(UserField);
		}
	}

	if (JsonObject->HasTypedField<EJson::String>(ArtheonWebSocketsJsonFields::Category)) {
		Category = JsonObject->GetStringField(ArtheonWebSocketsJsonFields::Category);
	}
}
