// Author: Nikolay Bulatov, Egor A. Pristavka. 
// Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#pragma once

#include "CoreMinimal.h"
#include "Tickable.h"
#include "VeLiveLinkSource.h"

class FVeLiveLinkSource;

class UVeLiveLinkLauncher : public FTickableGameObject {

public:
	UVeLiveLinkLauncher();
	virtual ~UVeLiveLinkLauncher();

	virtual void Tick(float DeltaTime) override;
	virtual bool IsTickable() const override { return !Source.IsValid(); }
	virtual bool IsTickableInEditor() const override { return true; }

	void Destroy();

	virtual TStatId GetStatId() const override {
		RETURN_QUICK_DECLARE_CYCLE_STAT(UVeLiveLinkStarter, STATGROUP_Tickables);
	}

private:
	TWeakPtr<FVeLiveLinkSource> Source;

};
