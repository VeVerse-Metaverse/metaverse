// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "VePersonaMetadata.h"
#include "VeFileMetadata.h"
#include "ApiPersonaMetadata.h"

FVePersonaMetadata::FVePersonaMetadata() {
}

FVePersonaMetadata::FVePersonaMetadata(const FApiPersonaMetadata& InPersonaMetadata):
	Id(InPersonaMetadata.Id),
	Name(InPersonaMetadata.Name),
	Description(InPersonaMetadata.Description),
	Configuration(InPersonaMetadata.Configuration),
	Type(InPersonaMetadata.Type) {
	for (const auto& File : InPersonaMetadata.Files) {
		Files.Add(FVeFileMetadata(File));
	}
}
