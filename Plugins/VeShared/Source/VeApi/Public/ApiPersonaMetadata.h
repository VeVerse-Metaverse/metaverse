// Author: Egor A. Pristavka. Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#pragma once

#include "ApiEntityMetadata.h"
#include "ApiFileMetadata.h"

/** User entity public reference. */
class VEAPI_API FApiPersonaMetadata final : public FApiEntityMetadata {
public:
	FString Name;
	FString Description;
	FString Configuration;
	FString Type;

	TArray<FApiFileMetadata> Files;

	FString GetDefaultAvatarUrl() const;

	virtual bool FromJson(const TSharedPtr<FJsonObject> JsonObject) override;
};

/** Used to create and update object requests. */
class VEAPI_API FApiCreatePersonaMetadata final : public IApiMetadata {
public:
	FString Name;
	FString Configuration;
	FString Type;

	virtual TSharedPtr<FJsonObject> ToJson() override;
};

/** Used to default persona requests. */
class VEAPI_API FApiDefaultPersonaMetadata final : public IApiMetadata {
public:
	FGuid PersonaId = FGuid();

	virtual TSharedPtr<FJsonObject> ToJson() override;
};

class VEAPI_API FApiUpdatePersonaMetadata final : public IApiMetadata {
public:
	FGuid Id = FGuid();
	FGuid PersonaId = FGuid();
	FString Name;
	FString Configuration;
	FString Type;


	virtual TSharedPtr<FJsonObject> ToJson() override;
};

typedef TApiBatchMetadata<FApiPersonaMetadata> FApiPersonaBatchMetadata;
