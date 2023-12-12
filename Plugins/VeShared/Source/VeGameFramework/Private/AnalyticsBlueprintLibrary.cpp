// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "AnalyticsBlueprintLibrary.h"

#include "Api2AnalyticsSubsystem.h"
#include "VeApi2.h"
#include "VeGameFramework.h"
#include "VeShared.h"
#include "VeWorldSettings.h"


void UAnalyticsBlueprintLibrary::RequestAnalytics(UObject* WorldContextObject, const FAnalyticsIndexRequestMetadata& InRequestMetadata, const FOnAnalyticsBatchRequestCompleted& InOnCompleted) {
	GET_MODULE_SUBSYSTEM(AnalyticsSubsystem, Api2, Analytics);
	if (!AnalyticsSubsystem) {
		return;
	}

	FGuid WorldId;
	if (const auto* World = GEngine->GetWorldFromContextObjectChecked(WorldContextObject)) {
		if (const auto* VeWorldSettings = Cast<AVeWorldSettings>(World->GetWorldSettings())) {
			WorldId = VeWorldSettings->GetWorldId();
		}
	}

	const auto Callback = MakeShared<FOnAnalyticsBatchRequestCompleted2>();
	Callback->BindWeakLambda(WorldContextObject, [=](const FApi2AnalyticsBatchResponseMetadata& InMetadata, const EApi2ResponseCode InResponseCode, const FString& InError) {
		TArray<FAnalyticsResponseMetadata> ResponseMetadata;
		for (const auto& ResponseEntity : InMetadata.Entities) {
			if (ResponseEntity) {
				FAnalyticsResponseMetadata ResponseMetadataEntity;
				ResponseMetadataEntity.ContextEntityType = ResponseEntity->ContextEntityType;
				ResponseMetadataEntity.ContextEntityId = ResponseEntity->ContextEntityId;
				ResponseMetadataEntity.UserId = ResponseEntity->UserId;
				ResponseMetadataEntity.Platform = ResponseEntity->Platform;
				ResponseMetadataEntity.Deployment = ResponseEntity->Deployment;
				ResponseMetadataEntity.Configuration = ResponseEntity->Configuration;
				ResponseMetadataEntity.Event = ResponseEntity->Event;
				ResponseMetadataEntity.Payload = ResponseEntity->Payload;
				ResponseMetadata.Add(ResponseMetadataEntity);
			}
		}
		InOnCompleted.ExecuteIfBound(ResponseMetadata, (InResponseCode == EApi2ResponseCode::Ok), InError);
	});

	FApi2AnalyticsBatchRequestMetadata Metadata;
	Metadata.Offset = InRequestMetadata.Offset;
	Metadata.Limit = InRequestMetadata.Limit;
	Metadata.ContextEntityType = InRequestMetadata.ContextEntityType;
	Metadata.ContextEntityId = InRequestMetadata.ContextEntityId;
	if (!Metadata.ContextEntityId.IsValid() && Metadata.ContextEntityType == "world") {
		Metadata.ContextEntityId = WorldId;
	}
	Metadata.UserId = InRequestMetadata.UserId;
	Metadata.Platform = InRequestMetadata.Platform;
	Metadata.Deployment = InRequestMetadata.Deployment;
	Metadata.Event = InRequestMetadata.Event;
	Metadata.Configuration = FVeConfigLibrary::GetConfiguration();

	AnalyticsSubsystem->IndexEvents(Metadata, Callback);
}
