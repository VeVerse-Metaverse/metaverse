// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.



#pragma once

#include "VeShared.h"
#include "ApiSpaceMetadata.h"
#include "ApiRequest.h"


class FGameFrameworkWorldItem;

typedef TDelegate<void(TSharedRef<FGameFrameworkWorldItem> InWorldItem, const bool bIsValid, const FString& InError)> FOnGameFrameworkWorldItemCreated;

/**
 * 
 */
class VEGAMEFRAMEWORK_API FGameFrameworkWorldSubsystem : public IModuleSubsystem {

public:
	const static FName Name;
	virtual void Initialize() override;
	virtual void Shutdown() override;
	FGameFrameworkWorldSubsystem() = default;

public:
	TSharedRef<FGameFrameworkWorldItem> GetItem(const FGuid& ItemId, const class FApiSpaceMetadata& InMetadata);
	void GetItem(const FGuid& ItemId, TSharedRef<FOnGameFrameworkWorldItemCreated> InCallback);

protected:
	TMap<FGuid, TSharedRef<FGameFrameworkWorldItem>> ItemList;

};
