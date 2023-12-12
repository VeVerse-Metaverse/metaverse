// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once

#include "VeConfigLibrary.h"

namespace Blockchain {

	inline FString GetRootUrl() {
		return FVeConfigLibrary::GetBlockchainApiUrl();
	}

}
