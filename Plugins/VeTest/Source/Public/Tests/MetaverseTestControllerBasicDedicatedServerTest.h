// Copyright Epic Games, Inc.All Rights Reserved.
#pragma once

#include "MetaverseTestControllerBase.h"
#include "MetaverseTestControllerBasicDedicatedServerTest.generated.h"

UCLASS()
class UMetaverseTestControllerBasicDedicatedServerTest : public UMetaverseTestControllerBase
{
	GENERATED_BODY()

protected:
	virtual void OnTick(float TimeDelta) override;

public:
	virtual void OnPostMapChange(UWorld* World) override;
};
