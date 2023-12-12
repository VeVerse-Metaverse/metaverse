// Author: Nikolay Bulatov, Egor A. Pristavka. 
// Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#include "VeLiveLinkRole.h"
#include "VeLiveLinkTypes.h"

#define LOCTEXT_NAMESPACE "LiveLinkRole"

UScriptStruct* UVeLiveLinkRole::GetStaticDataStruct() const {
	return FVeLiveLinkStaticData::StaticStruct();
}

UScriptStruct* UVeLiveLinkRole::GetFrameDataStruct() const
{
	return FVeLiveLinkFrameData::StaticStruct();
}

UScriptStruct* UVeLiveLinkRole::GetBlueprintDataStruct() const
{
	return FVeLiveLinkBlueprintData::StaticStruct();
}

bool UVeLiveLinkRole::InitializeBlueprintData(const FLiveLinkSubjectFrameData& InSourceData, FLiveLinkBlueprintDataStruct& OutBlueprintData) const
{
	bool bSuccess = false;

	FVeLiveLinkBlueprintData* BlueprintData = OutBlueprintData.Cast<FVeLiveLinkBlueprintData>();
	const FVeLiveLinkFrameData* FrameData = InSourceData.FrameData.Cast<FVeLiveLinkFrameData>();
	if (BlueprintData && FrameData)
	{
		GetFrameDataStruct()->CopyScriptStruct(&BlueprintData->FrameData, FrameData);
		bSuccess = true;
	}

	return bSuccess;
}

FText UVeLiveLinkRole::GetDisplayName() const
{
	return LOCTEXT("VeVerse LiveLink Role", "VeVerse LiveLink Role");
}

bool UVeLiveLinkRole::IsStaticDataValid(const FLiveLinkStaticDataStruct& InStaticData, bool& bOutShouldLogWarning) const {
	return Super::IsStaticDataValid(InStaticData, bOutShouldLogWarning);
}

bool UVeLiveLinkRole::IsFrameDataValid(const FLiveLinkStaticDataStruct& InStaticData, const FLiveLinkFrameDataStruct& InFrameData, bool& bOutShouldLogWarning) const {
	return Super::IsFrameDataValid(InStaticData, InFrameData, bOutShouldLogWarning);
}

#undef LOCTEXT_NAMESPACE
