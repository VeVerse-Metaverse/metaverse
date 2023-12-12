// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once
#include "VeShared.h"

class VEAPI_API FApiUserActionSubsystem : public IModuleSubsystem {

	bool SendUserAction(const FGuid& InUserId, const FString& Category, const FString& Action, const FString& Details) const;

};
