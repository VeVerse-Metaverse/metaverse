// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once

#include "ApiFileUploadRequest.h"
#include "ApiPersonaRequest.h"
#include "VeShared.h"

class VEAPI_API FApiPersonaSubsystem final : public IModuleSubsystem {
public:
	const static FName Name;

	virtual void Initialize() override;
	virtual void Shutdown() override;

	/**
	 * Requests the user metadata by the id.
	 * @param InUserId User ID.
	 * @param InMetadata Metadata.
	 * @param InCallback Complete callback delegate. 
	 */
	bool GetPersonas(const FGuid& InUserId, const IApiBatchQueryRequestMetadata InMetadata, TSharedPtr<FOnPersonaBatchRequestCompleted> InCallback);

	/**
	 * Requests the persona metadata by its id.
	 * @param InId Persona ID.
	 * @param InCallback Complete callback delegate.
	 */
	void GetPersona(const FGuid& InId, TSharedRef<FOnPersonaRequestCompleted> InCallback);

	/**
	 * Requests the user metadata by the id.
	 * @param InMetadata Metadata.
	 * @param InCallback Complete callback delegate. 
	 */
	bool PostPersonas(const FApiCreatePersonaMetadata InMetadata, TSharedPtr<FOnPersonaRequestCompleted> InCallback);

	/**
	 * Requests the user metadata by the id.
	 * @param InPersonaId PersonaId.
	 * @param InCallback Complete callback delegate. 
	 */
	bool PostDefaultPersonas(const FGuid& InPersonaId, TSharedPtr<FOnPersonaRequestCompleted> InCallback);

	/**
	 * Requests the user metadata by the id.
	 * @param InMetadata Metadata.
	 * @param InCallback Complete callback delegate. 
	 */
	bool PatchPersonas(const FApiUpdatePersonaMetadata InMetadata, TSharedPtr<FOnPersonaRequestCompleted> InCallback); /**

	* Requests the user metadata by the id.
	 * @param InMetadata Metadata.
	 * @param InCallback Complete callback delegate. 
	 */
	bool DeletePersonas(const FApiUpdatePersonaMetadata InMetadata, TSharedPtr<FOnPersonaRequestCompleted> InCallback);

};

typedef TSharedPtr<FApiPersonaSubsystem> FApiPersonaSubsystemPtr;
