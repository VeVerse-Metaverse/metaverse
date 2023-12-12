// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

 

#pragma once

#include "UObject/Object.h"
#include "ApiPackageMetadata.h"
#include "PortalLib.generated.h"

// enum class EVeMountPakState : uint8 {
// 	Success,
// 	Failure,
// 	NotExist
// };


/**
 * 
 */
UCLASS()
class VEGAMEFRAMEWORK_API UPortalLib : public UObject {
	GENERATED_BODY()

public:
	static bool IsAssetExists(const FString& PackageName);
	
};
