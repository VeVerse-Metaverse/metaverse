// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once

namespace ArtheonWebSocketsType {
    const int32 None = 0;
    const int32 PushMessageType = 1 << 0;
    const int32 RequestMessageType = 1 << 1;
    const int32 ResponseMessageType = 1 << 2;
}

namespace ArtheonWebSocketsTopic {
    const int32 None = 0;
    const int32 ConnectTopic = 1 << 0;
    const int32 ChatTopic = 1 << 1;
    const int32 AnalyticsTopic = 1 << 2;
    const int32 VivoxTopic = 1 << 3;
}

namespace ArtheonWebSocketsStatus {
    const auto Ok = TEXT("ok");
    const auto Error = TEXT("error");
}

namespace ArtheonWebSocketsMethods {
    const auto ConnectMethod = TEXT("connect");
    const auto UserActionMethod = TEXT("userAction");
    const auto VivoxGetLoginToken = TEXT("vivoxGetLoginToken");
    const auto VivoxGetJoinToken = TEXT("vivoxGetJoinToken");
    const auto VivoxMute = TEXT("vivoxMute");
    const auto VivoxUnmute = TEXT("vivoxUnmute");
    const auto VivoxKick = TEXT("vivoxKick");
    const auto ChannelSubscribeMethod = TEXT("channelSubscribe");
    const auto ChannelUnsubscribeMethod = TEXT("channelUnsubscribe");
    const auto ChannelSendMethod = TEXT("channelSend");
}

namespace ArtheonWebSocketsJsonFields {
    const auto Id = TEXT("id");
    const auto Type = TEXT("type");
    const auto Topic = TEXT("topic");
    const auto Method = TEXT("method");
    const auto Payload = TEXT("payload");
    const auto Args = TEXT("args");
    const auto Message = TEXT("message");
    const auto Error = TEXT("error");
    const auto Status = TEXT("status");
    const auto Sender = TEXT("sender");
    const auto Name = TEXT("name");
    const auto AvatarUrl = TEXT("avatarUrl");
    const auto Key = TEXT("key");
    const auto ChannelId = TEXT("channelId");
    const auto Category = TEXT("category");
    const auto SenderId = TEXT("senderId");
    const auto UserId = TEXT("userId");
    const auto Details = TEXT("details");
    const auto Action = TEXT("action");
    const auto VxIssuer = TEXT("iss");
    const auto VxAction = TEXT("vxa");
    const auto VxExpiresAt = TEXT("exp");
    const auto VxSerial = TEXT("vxi");
    const auto VxSubject = TEXT("sub");
    const auto VxFrom = TEXT("f");
    const auto VxTo = TEXT("t");
    const auto VxServer = TEXT("server");
    const auto VxToken = TEXT("token");
    const auto VxChannelProperties = TEXT("channelProperties");
    const auto VxAudibleDistance = TEXT("audibleDistance");
    const auto VxConversationalDistance = TEXT("conversationalDistance");
    const auto VxAudioFadeIntensity = TEXT("audioFadeIntensity");
    const auto VxAudioFadeModel = TEXT("audioFadeModel");
    const auto VxChannelType = TEXT("channelType");
    const auto VxPayload = TEXT("vivoxPayload");
}

namespace ArtheonWebSocketsId {
    const auto ChannelId = TEXT("f16fdbe4-813c-4e9c-a1a1-c7c2cbb154d3");
    const auto UserId = TEXT("4e84a83f-e28a-4648-8cda-571d4d4e8147");
}
