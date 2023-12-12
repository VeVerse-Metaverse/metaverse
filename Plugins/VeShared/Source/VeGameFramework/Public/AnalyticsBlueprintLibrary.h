// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.



#pragma once

#include "CoreMinimal.h"
#include "Api2Request.h"
#include "AnalyticsMetadata.h"
#include "AnalyticsBlueprintLibrary.generated.h"

DECLARE_DYNAMIC_DELEGATE_ThreeParams(FOnAnalyticsBatchRequestCompleted, const TArray<FAnalyticsResponseMetadata>&, ResponseMetadata, bool, bSuccessful, const FString&, InError);

/**
 * 
 */
UCLASS()
class VEGAMEFRAMEWORK_API UAnalyticsBlueprintLibrary : public UBlueprintFunctionLibrary {
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Analytics", meta = (WorldContext = "WorldContextObject"))
	static void RequestAnalytics(UObject* WorldContextObject, const FAnalyticsIndexRequestMetadata &InRequestMetadata, const FOnAnalyticsBatchRequestCompleted& InOnCompleted);
};
