// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.


#pragma once

#include "VeShared.h"

/**
 * 
 */
class VEGAMEFRAMEWORK_API FGameFrameworkPlaceableSubsystem : public IModuleSubsystem {

	DECLARE_EVENT_OneParam(FGameFrameworkPlaceableSubsystem, GameFrameworkPlaceableEvent, class UPlaceableComponent* PlaceableComponent);

public:
	const static FName Name;

	virtual void Initialize() override;
	virtual void Shutdown() override;

	void SetSelected(AActor* Object);
	void SetSelected(class UPlaceableComponent* PlaceableComponent);

	GameFrameworkPlaceableEvent& GetOnSelected() { return OnSelected; }

private:
	GameFrameworkPlaceableEvent OnSelected;
};
