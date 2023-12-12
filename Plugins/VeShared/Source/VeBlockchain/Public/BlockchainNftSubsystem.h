// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once
#include "BlockchainRequest.h"
#include "VeShared.h"

class VEBLOCKCHAIN_API FBlockchainNftSubsystem final : public IModuleSubsystem {
public:
	const static FName Name;

	virtual void Initialize() override;
	virtual void Shutdown() override;

	/** Gets owner address. */
	bool GetOwnerOf(uint32 InId, TSharedPtr<FOnOwnerOfRequestCompleted> InCallback) const;

	/** Requests token metadata. */
	bool GetTokenUri(uint32 InId, TSharedPtr<FOnTokenUriRequestCompleted> InCallback) const;
};
