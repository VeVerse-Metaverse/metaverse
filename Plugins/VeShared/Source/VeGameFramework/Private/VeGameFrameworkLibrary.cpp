// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "VeGameFrameworkLibrary.h"

#include "AnalyticsMetadata.h"
#include "Api2AnalyticsSubsystem.h"
#include "Api2Request.h"
#include "GameFrameworkWorldItem.h"
#include "VeApi2.h"
#include "VeGameFramework.h"
#include "VePlayerStateBase.h"
#include "VeShared.h"
#include "VeSharedLibrary.h"
#include "GameFramework/PlayerController.h"
#include "VeWorldSettings.h"
#include "VeWorldMetadata.h"


FGuid UVeGameFrameworkLibrary::GetWorldId(const UObject* WorldContextObject) {
	if (const UWorld* World = GEngine->GetWorldFromContextObjectChecked(WorldContextObject)) {
		if (auto* VeWorldSettings = Cast<AVeWorldSettings>(World->GetWorldSettings())) {
			return VeWorldSettings->GetWorldId();
		}
	}
	return FGuid();
}

FGuid UVeGameFrameworkLibrary::GetServerId(const UObject* WorldContextObject) {
	if (const UWorld* World = GEngine->GetWorldFromContextObjectChecked(WorldContextObject)) {
		if (auto* VeWorldSettings = Cast<AVeWorldSettings>(World->GetWorldSettings())) {
			return VeWorldSettings->GetServerId();
		}
	}
	return FGuid();
}

FVeWorldMetadata UVeGameFrameworkLibrary::GetWorldMetadata(const UObject* WorldContextObject) {
	if (const UWorld* World = GEngine->GetWorldFromContextObjectChecked(WorldContextObject)) {
		if (auto* VeWorldSettings = Cast<AVeWorldSettings>(World->GetWorldSettings())) {
			if (auto* WorldItem = VeWorldSettings->GetWorldItem()) {
				return FVeWorldMetadata(WorldItem->GetMetadata());
			}
		}
	}
	return FVeWorldMetadata();
}

bool UVeGameFrameworkLibrary::IsClientInitializationComplete(const UObject* WorldContextObject) {
	if (const UWorld* World = GEngine->GetWorldFromContextObjectChecked(WorldContextObject)) {
		if (auto* VeWorldSettings = Cast<AVeWorldSettings>(World->GetWorldSettings())) {
			return VeWorldSettings->IsClientInitializationComplete();
		}
	}
	return true;
}

bool UVeGameFrameworkLibrary::IsServerInitializationComplete(const UObject* WorldContextObject) {
	if (const UWorld* World = GEngine->GetWorldFromContextObjectChecked(WorldContextObject)) {
		if (auto* VeWorldSettings = Cast<AVeWorldSettings>(World->GetWorldSettings())) {
			return VeWorldSettings->IsServerInitializationComplete();
		}
	}
	return true;
}

#if 0
void UVeGameFrameworkLibrary::OnReady_GameState(const UObject* WorldContextObject, FGameStateDelegate Callback) {
	if (UWorld* World = GEngine->GetWorldFromContextObjectChecked(WorldContextObject)) {
		if (World->GetGameState()) {
			Callback.ExecuteIfBound(World->GetGameState());
			return;
		}
		World->GameStateSetEvent.AddWeakLambda(Callback.GetUObject(), [Callback](AGameStateBase* InGameStateBase) {
			Callback.ExecuteIfBound(InGameStateBase);
		});
		return;
	}
	Callback.ExecuteIfBound(nullptr);
}

void UVeGameFrameworkLibrary::OnReady_WorldId(const UObject* WorldContextObject, const FWorldIdDelegate& Callback) {
	if (const UWorld* World = GEngine->GetWorldFromContextObjectChecked(WorldContextObject)) {
		if (auto* VeWorldSettings = Cast<AVeWorldSettings>(World->GetWorldSettings())) {
			const auto OnWorldId = AVeWorldSettings::FWorldIdDelegate::CreateWeakLambda(Callback.GetUObject(), [Callback](const FGuid& InWorldId) {
				Callback.ExecuteIfBound(InWorldId);
			});
			VeWorldSettings->OnReady_WorldId(OnWorldId);
			return;
		}
	}
	Callback.ExecuteIfBound(FGuid());
}

void UVeGameFrameworkLibrary::OnReady_GameInitializationClient(const UObject* WorldContextObject, const FGameInitializationDelegate& Callback) {
	if (const UWorld* World = GEngine->GetWorldFromContextObjectChecked(WorldContextObject)) {
		if (auto* VeWorldSettings = Cast<AVeWorldSettings>(World->GetWorldSettings())) {
			const auto OnGameInitialization = AVeWorldSettings::FInitializationDelegate::CreateWeakLambda(Callback.GetUObject(), [Callback]() {
				Callback.ExecuteIfBound();
			});
			VeWorldSettings->OnReady_ClientInitializationComplete(OnGameInitialization);
			return;
		}
	}
	Callback.ExecuteIfBound();
}

void UVeGameFrameworkLibrary::OnReady_GameInitializationServer(const UObject* WorldContextObject, const FGameInitializationDelegate& Callback) {
	if (const UWorld* World = GEngine->GetWorldFromContextObjectChecked(WorldContextObject)) {
		if (auto* VeWorldSettings = Cast<AVeWorldSettings>(World->GetWorldSettings())) {
			const auto OnGameInitialization = AVeWorldSettings::FInitializationDelegate::CreateWeakLambda(Callback.GetUObject(), [Callback]() {
				Callback.ExecuteIfBound();
			});
			VeWorldSettings->OnReady_ServerInitializationComplete(OnGameInitialization);
			return;
		}
	}
	Callback.ExecuteIfBound();
}
#endif

void UVeGameFrameworkLibrary::Generic_ReportUserEvent(const UObject* WorldContextObject, APlayerController* PlayerController, const FString& EventType, FProperty* Property, const void* Value) {
	GET_MODULE_SUBSYSTEM(AnalyticsSubsystem, Api2, Analytics);
	if (!AnalyticsSubsystem) {
		return;
	}

	FString PayloadStr = UVeSharedLibrary::ValueToJson(Property, Value);

	FGuid PlayerId;
	if (PlayerController) {
		if (const auto* PlayerState = PlayerController->GetPlayerState<AVePlayerStateBase>()) {
			PlayerId = PlayerState->GetUserId();
		}
	}

	const auto Callback = MakeShared<FOnAnalyticsRequestCompleted2>();
	Callback->BindLambda([=](const FApi2AnalyticsRequestMetadata& InMetadata, const EApi2ResponseCode InResponseCode, const FString& InError) { });

	FApi2AnalyticsRequestMetadata Metadata;
	Metadata.Payload = PayloadStr;
	Metadata.UserId = PlayerId;
	Metadata.AppId = FVeConfigLibrary::GetAppId();
	Metadata.ContextEntityType = "";
	Metadata.ContextEntityId = FGuid{};
	Metadata.Event = EventType;
	Metadata.Configuration = FVeConfigLibrary::GetConfiguration();

	AnalyticsSubsystem->ReportEvent(Metadata, Callback);
}

void UVeGameFrameworkLibrary::World_ReportUserEvent(const UObject* WorldContextObject, APlayerController* PlayerController, const FString& EventType, FProperty* Property, const void* Value) {
	GET_MODULE_SUBSYSTEM(AnalyticsSubsystem, Api2, Analytics);
	if (!AnalyticsSubsystem) {
		return;
	}

	FString PayloadStr = UVeSharedLibrary::ValueToJson(Property, Value);

	FGuid PlayerId;
	if (PlayerController) {
		if (const auto* PlayerState = PlayerController->GetPlayerState<AVePlayerStateBase>()) {
			PlayerId = PlayerState->GetUserMetadata().Id;
		}
	}

	FGuid WorldId;
	if (const auto* World = GEngine->GetWorldFromContextObjectChecked(WorldContextObject)) {
		if (const auto* VeWorldSettings = Cast<AVeWorldSettings>(World->GetWorldSettings())) {
			WorldId = VeWorldSettings->GetWorldId();
		}
	}

	const auto Callback = MakeShared<FOnAnalyticsRequestCompleted2>();
	Callback->BindLambda([=](const FApi2AnalyticsRequestMetadata& InMetadata, const EApi2ResponseCode InResponseCode, const FString& InError) { });

	FApi2AnalyticsRequestMetadata Metadata;
	Metadata.Payload = PayloadStr;
	Metadata.UserId = PlayerId;
	Metadata.AppId = FVeConfigLibrary::GetAppId();
	Metadata.ContextEntityType = "world";
	Metadata.ContextEntityId = WorldId;
	Metadata.Event = EventType;
	Metadata.Configuration = FVeConfigLibrary::GetConfiguration();

	AnalyticsSubsystem->ReportEvent(Metadata, Callback);
}

void UVeGameFrameworkLibrary::Custom_ReportUserEvent(const UObject* WorldContextObject, const FAnalyticsRequestMetadata& InMetadata, FProperty* Property, const void* Value, APlayerController* PlayerController) {
	GET_MODULE_SUBSYSTEM(AnalyticsSubsystem, Api2, Analytics);
	if (!AnalyticsSubsystem) {
		return;
	}

	FString PayloadStr = UVeSharedLibrary::ValueToJson(Property, Value);

	FGuid PlayerId;
	if (InMetadata.UserId.IsValid()) {
		PlayerId = InMetadata.UserId;
	} else if (PlayerController) {
		if (const auto* PlayerState = PlayerController->GetPlayerState<AVePlayerStateBase>()) {
			PlayerId = PlayerState->GetUserMetadata().Id;
		}
	}

	FGuid WorldId;
	if (const auto* World = GEngine->GetWorldFromContextObjectChecked(WorldContextObject)) {
		if (const auto* VeWorldSettings = Cast<AVeWorldSettings>(World->GetWorldSettings())) {
			WorldId = VeWorldSettings->GetWorldId();
		}
	}

	FGuid ContextEntityId;
	if (InMetadata.ContextEntityId.IsValid()) {
		ContextEntityId = InMetadata.ContextEntityId;
	} else if (InMetadata.ContextEntityType == "world" && WorldId.IsValid()) {
		ContextEntityId = WorldId;
	}

	const auto Callback = MakeShared<FOnAnalyticsRequestCompleted2>();
	Callback->BindLambda([=](const FApi2AnalyticsRequestMetadata&, const EApi2ResponseCode, const FString&) { });

	FApi2AnalyticsRequestMetadata Metadata;
	Metadata.Payload = PayloadStr;
	Metadata.UserId = PlayerId;
	Metadata.AppId = FVeConfigLibrary::GetAppId();
	Metadata.ContextEntityType = InMetadata.ContextEntityType;
	Metadata.ContextEntityId = ContextEntityId;
	Metadata.Event = InMetadata.Event;
	Metadata.Configuration = FVeConfigLibrary::GetConfiguration();

	AnalyticsSubsystem->ReportEvent(Metadata, Callback);
}

DEFINE_FUNCTION(UVeGameFrameworkLibrary::execReportUserEvent) {
	P_GET_OBJECT(UObject, WorldContextObject);
	P_GET_OBJECT(APlayerController, PlayerController);
	P_GET_PROPERTY(FStrProperty, EventType);

	Stack.Step(Stack.Object, NULL);

	FProperty* Property = Stack.MostRecentProperty;

	void* Value = Stack.MostRecentPropertyAddress;

	P_FINISH;

	P_NATIVE_BEGIN;
		Generic_ReportUserEvent(WorldContextObject, PlayerController, EventType, Property, Value);
	P_NATIVE_END;
}

DEFINE_FUNCTION(UVeGameFrameworkLibrary::execReportUserWorldEvent) {
	P_GET_OBJECT(UObject, WorldContextObject);
	P_GET_OBJECT(APlayerController, PlayerController);
	P_GET_PROPERTY(FStrProperty, EventType);

	Stack.Step(Stack.Object, NULL);

	FProperty* Property = Stack.MostRecentProperty;

	void* Value = Stack.MostRecentPropertyAddress;

	P_FINISH;

	P_NATIVE_BEGIN;
		World_ReportUserEvent(WorldContextObject, PlayerController, EventType, Property, Value);
	P_NATIVE_END;
}

DEFINE_FUNCTION(UVeGameFrameworkLibrary::execReportUserCustomEvent) {
	P_GET_OBJECT(UObject, WorldContextObject);
	P_GET_STRUCT(FAnalyticsRequestMetadata, InMetadata);
	P_GET_PROPERTY(FStrProperty, EventType);
	P_GET_OBJECT(APlayerController, PlayerController);

	Stack.Step(Stack.Object, NULL);

	FProperty* Property = Stack.MostRecentProperty;

	void* Value = Stack.MostRecentPropertyAddress;

	P_FINISH;

	P_NATIVE_BEGIN;
		Custom_ReportUserEvent(WorldContextObject, InMetadata, Property, Value, PlayerController);
	P_NATIVE_END;
}
