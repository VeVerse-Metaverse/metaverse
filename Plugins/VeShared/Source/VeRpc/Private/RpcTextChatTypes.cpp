// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "RpcWebSocketsTextChatTypes.h"


FLinearColor FRpcWebSocketsTextChatMessage::GetFontColor() const {
	switch (Category) {
	case ERpcWebSocketsTextChatCategory::System:
		return {1.000000f, 0.982251f, 0.401978f, 1.0f}; // #fffdaa (SpecifiedColor=(R=1.000000,G=0.982251,B=0.401978,A=1.000000),ColorUseRule=UseColor_Specified)
	case ERpcWebSocketsTextChatCategory::General:
		return {0.603828f, 0.603828f, 0.603828f, 1.0f}; // #cccccc (SpecifiedColor=(R=0.603828,G=0.603828,B=0.603828,A=1.000000),ColorUseRule=UseColor_Specified)
	case ERpcWebSocketsTextChatCategory::Space:
		return {0.341915f, 0.491021f, 1.000000f, 1.0f}; // #9ebaff (SpecifiedColor=(R=0.341915,G=0.491021,B=1.000000,A=1.000000),ColorUseRule=UseColor_Specified)
	case ERpcWebSocketsTextChatCategory::Server:
		return {0.205079f, 1.000000f, 0.630757f, 1.0f}; // #7dffd0 (SpecifiedColor=(R=0.205079,G=1.000000,B=0.630757,A=1.000000),ColorUseRule=UseColor_Specified)
	case ERpcWebSocketsTextChatCategory::Private:
		return {0.863157f, 0.401978f, 1.000000f, 1.0f}; // #efaaff (SpecifiedColor=(R=0.863157,G=0.401978,B=1.000000,A=1.000000),ColorUseRule=UseColor_Specified)
	case ERpcWebSocketsTextChatCategory::AI:
		return {0.670588f, 1.000000f, 0.858824f, 1.0f}; // #abffdb (SpecifiedColor=(R=0.670588,G=1.000000,B=0.858824,A=1.000000),ColorUseRule=UseColor_Specified)
	default:
		return {1.f, 1.f, 1.f, 1.f};
	}
}

FString FRpcWebSocketsTextChatMessage::ToString() const {
	FString CategoryStr{TEXT("None")};
	switch (Category) {
	case ERpcWebSocketsTextChatCategory::System:
		CategoryStr = RpcTextChatMessageCategory::System;
		break;
	case ERpcWebSocketsTextChatCategory::General:
		CategoryStr = RpcTextChatMessageCategory::General;
		break;
	case ERpcWebSocketsTextChatCategory::Space:
		CategoryStr = RpcTextChatMessageCategory::Space;
		break;
	case ERpcWebSocketsTextChatCategory::Server:
		CategoryStr = RpcTextChatMessageCategory::Server;
		break;
	case ERpcWebSocketsTextChatCategory::Private:
		CategoryStr = RpcTextChatMessageCategory::Private;
		break;
	default:
		CategoryStr = RpcTextChatMessageCategory::None;
	}
	CategoryStr = CategoryStr.Mid(0, 1).ToUpper() + CategoryStr.Mid(1);

	return FString::Printf(TEXT("[%s] @%s: %s"), *CategoryStr, *Sender.Name, *Text);
}

void FRpcWebSocketsTextChatMessage::ParseJson(const TSharedPtr<FJsonObject> JsonObject) {
	if (!JsonObject) {
		return;
	}
	
	if (JsonObject->HasTypedField<EJson::String>(RpcTextChatJsonFields::ChannelId)) {
		ChannelId = FGuid(JsonObject->GetStringField(RpcTextChatJsonFields::ChannelId));
	}
	
	if (JsonObject->HasTypedField<EJson::String>(RpcTextChatJsonFields::Message)) {
		Text = JsonObject->GetStringField(RpcTextChatJsonFields::Message);
	}

	if (JsonObject->HasTypedField<EJson::Object>(RpcTextChatJsonFields::Sender)) {
		Sender.ParseJson(JsonObject->GetObjectField(RpcTextChatJsonFields::Sender));
	}

	if (JsonObject->HasTypedField<EJson::String>(RpcTextChatJsonFields::Category)) {
		const FString InCategory = JsonObject->GetStringField(RpcTextChatJsonFields::Category);
		if (InCategory.Equals(RpcTextChatMessageCategory::System)) {
			Category = ERpcWebSocketsTextChatCategory::System;
		} else if (InCategory.Equals(RpcTextChatMessageCategory::General)) {
			Category = ERpcWebSocketsTextChatCategory::General;
		} else if (InCategory.Equals(RpcTextChatMessageCategory::Space)) {
			Category = ERpcWebSocketsTextChatCategory::Space;
		} else if (InCategory.Equals(RpcTextChatMessageCategory::Server)) {
			Category = ERpcWebSocketsTextChatCategory::Server;
		} else if (InCategory.Equals(RpcTextChatMessageCategory::Private)) {
			Category = ERpcWebSocketsTextChatCategory::Private;
		} else {
			Category = ERpcWebSocketsTextChatCategory::None;
		}
	}
}
