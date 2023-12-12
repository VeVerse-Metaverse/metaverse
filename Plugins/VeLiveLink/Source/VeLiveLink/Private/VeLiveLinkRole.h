// Author: Nikolay Bulatov, Egor A. Pristavka. 
// Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#pragma once

#include "Roles/LiveLinkAnimationRole.h"

#include "VeLiveLinkRole.generated.h"

/**
 * Role associated with VeVerse Live Link data
 */
UCLASS(BlueprintType, meta = (DisplayName = "VeVerse LiveLink Role"))
class VELIVELINK_API UVeLiveLinkRole : public ULiveLinkBasicRole
{
	GENERATED_BODY()

public:
	//~ Begin ULiveLinkRole interface
	virtual UScriptStruct* GetStaticDataStruct() const override;
	virtual UScriptStruct* GetFrameDataStruct() const override;
	virtual UScriptStruct* GetBlueprintDataStruct() const override;

	bool InitializeBlueprintData(const FLiveLinkSubjectFrameData& InSourceData, FLiveLinkBlueprintDataStruct& OutBlueprintData) const override;

	virtual FText GetDisplayName() const override;
	virtual bool IsStaticDataValid(const FLiveLinkStaticDataStruct& InStaticData, bool& bOutShouldLogWarning) const override;
	virtual bool IsFrameDataValid(const FLiveLinkStaticDataStruct& InStaticData, const FLiveLinkFrameDataStruct& InFrameData, bool& bOutShouldLogWarning) const override;
	//~ End ULiveLinkRole interface
};
