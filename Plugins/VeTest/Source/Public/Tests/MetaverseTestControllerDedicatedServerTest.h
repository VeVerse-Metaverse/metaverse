// Copyright Epic Games, Inc.All Rights Reserved.
#pragma once

#include "MetaverseTestControllerBase.h"
#include "MetaverseTestControllerDedicatedServerTest.generated.h"

UCLASS()
class UMetaverseTestControllerDedicatedServerTest : public UMetaverseTestControllerBase {
	GENERATED_BODY()

protected:
	virtual void OnTick(float TimeDelta) override;
};
