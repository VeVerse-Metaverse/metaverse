// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.



#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "VeGameFrameworkLibrary.generated.h"

struct FAnalyticsRequestMetadata;

/**
 * 
 */
UCLASS()
class VEGAMEFRAMEWORK_API UVeGameFrameworkLibrary : public UBlueprintFunctionLibrary {
	GENERATED_BODY()

	DECLARE_DYNAMIC_DELEGATE_OneParam(FGameStateDelegate, AGameStateBase*, InGameState);

	DECLARE_DYNAMIC_DELEGATE_OneParam(FWorldIdDelegate, const FGuid&, InWorldId);

	DECLARE_DYNAMIC_DELEGATE(FGameInitializationDelegate);

public:
	/**
	 * Returns current world ID.
	 */
	UFUNCTION(BlueprintPure, Category="Metaverse", meta=(WorldContext="WorldContextObject"))
	static FGuid GetWorldId(const UObject* WorldContextObject);

	/**
	 * Returns current server ID.
	 */
	UFUNCTION(BlueprintPure, Category="Metaverse", meta=(WorldContext="WorldContextObject"))
	static FGuid GetServerId(const UObject* WorldContextObject);

	/**
	 * Returns current WorldMetadata.
	 */
	UFUNCTION(BlueprintCallable, Category="Metaverse", meta=(WorldContext="WorldContextObject"))
	static FVeWorldMetadata GetWorldMetadata(const UObject* WorldContextObject);

	UFUNCTION(BlueprintPure, Category="Metaverse", meta=(WorldContext="WorldContextObject"))
	static bool IsClientInitializationComplete(const UObject* WorldContextObject);

	UFUNCTION(BlueprintPure, Category="Metaverse", meta=(WorldContext="WorldContextObject"))
	static bool IsServerInitializationComplete(const UObject* WorldContextObject);

#if 0
	/**
	 * Returns current world GameState.
	 */
	UFUNCTION(BlueprintCallable, Category="Metaverse", meta=(WorldContext="WorldContextObject"))
	static void OnReady_GameState(const UObject* WorldContextObject, FGameStateDelegate Callback);

	UFUNCTION(BlueprintCallable, Category="Metaverse", meta=(WorldContext="WorldContextObject"))
	static void OnReady_WorldId(const UObject* WorldContextObject, const FWorldIdDelegate& Callback);

	UFUNCTION(BlueprintCallable, Category="Metaverse", meta=(WorldContext="WorldContextObject"))
	static void OnReady_GameInitializationClient(const UObject* WorldContextObject, const FGameInitializationDelegate& Callback);

	UFUNCTION(BlueprintCallable, Category="Metaverse", meta=(WorldContext="WorldContextObject"))
	static void OnReady_GameInitializationServer(const UObject* WorldContextObject, const FGameInitializationDelegate& Callback);
#endif

	/**
	 * @brief Report a user event to the analytics service without particular context.
	 * @param PlayerController The player controller to use to get the user id.
	 * @param EventType The type of the event, an arbitrary string to identify the event.
	 * @param EventPayload The payload of the event, a structure containing the data to send, must be a structure.
	 */
	UFUNCTION(BlueprintCallable, Category="Metaverse|Analytics", CustomThunk, meta=(WorldContext="WorldContextObject", CustomStructureParam="EventPayload"))
	static void ReportUserEvent(const UObject* WorldContextObject, APlayerController* PlayerController, const FString& EventType, UProperty* EventPayload);

	/**
	 * @brief Report a user event to the analytics service with the current world as a context.
	 * @param PlayerController The player controller to use to get the user id.
	 * @param EventType The type of the event, an arbitrary string to identify the event.
	 * @param EventPayload The payload of the event, a structure containing the data to send, must be a structure.
	 */
	UFUNCTION(BlueprintCallable, Category="Metaverse|Analytics", CustomThunk, meta=(WorldContext="WorldContextObject", CustomStructureParam="EventPayload"))
	static void ReportUserWorldEvent(const UObject* WorldContextObject, APlayerController* PlayerController, const FString& EventType, UProperty* EventPayload);

	/**
	 * @brief Report a custom event to the analytics service.
	 * @param Metadata The metadata of the event, a structure containing the data to send.
	 * @param EventPayload The payload of the event, a structure containing the data to send, must be a structure.
	 * @param PlayerController The player controller to use to get the user id (optional).
	 * @details The user ID can be specified manually, or supplied with PlayerController argument, if both are not provided, the user id  will be empty.
	 */
	UFUNCTION(BlueprintCallable, Category="Metaverse|Analytics", CustomThunk, meta=(WorldContext="WorldContextObject", CustomStructureParam="EventPayload"))
	static void ReportUserCustomEvent(const UObject* WorldContextObject, const FAnalyticsRequestMetadata& Metadata, UProperty* EventPayload, APlayerController* PlayerController = nullptr);

private:
	DECLARE_FUNCTION(execReportUserEvent);
	static void Generic_ReportUserEvent(const UObject* WorldContextObject, APlayerController* PlayerController, const FString& EventType, FProperty* Property, const void* Value);

	DECLARE_FUNCTION(execReportUserWorldEvent);
	static void World_ReportUserEvent(const UObject* WorldContextObject, APlayerController* PlayerController, const FString& EventType, FProperty* Property, const void* Value);

	DECLARE_FUNCTION(execReportUserCustomEvent);
	static void Custom_ReportUserEvent(const UObject* WorldContextObject, const FAnalyticsRequestMetadata& Metadata, FProperty* Property, const void* Value, APlayerController* PlayerController = nullptr);
};
