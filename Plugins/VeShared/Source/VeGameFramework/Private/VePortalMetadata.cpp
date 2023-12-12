// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "VePortalMetadata.h"


FVePortalDestinationMetadata::FVePortalDestinationMetadata(const FApiPortalDestinationMetadata& InApiMetadata)
	: Id(InApiMetadata.Id),
	  Name(InApiMetadata.Name),
	  World(FVeWorldMetadata{InApiMetadata.Space}) {

	for (auto File : InApiMetadata.Files) {
		Files.Add(FVeFileMetadata{File});
	}
}

FVePortalMetadata::FVePortalMetadata(const FApiPortalMetadata& InApiMetadata)
	: Id(InApiMetadata.Id),
	  Name(InApiMetadata.Name),
	  Destination(FVePortalDestinationMetadata(InApiMetadata.Destination)) {}
