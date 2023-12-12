// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once
#include "ArtheonWebSocketsMessage.h"

/** Called when an API request completes. */
DECLARE_DELEGATE_TwoParams(FOnCppWebSocketsRequestComplete, FArtheonWebSocketsMessage, bool);

class FArtheonWebSocketsRequest final {
public:
    /** Internal helper to call both blueprint and code delegates. */
    bool TriggerDelegates(const bool bSuccessful) const;

    FArtheonWebSocketsMessage Message;
    FArtheonWebSocketsMessage ResponseMessage = FArtheonWebSocketsMessage();

    /** Delegate called when the request completes. */
    FOnCppWebSocketsRequestComplete OnCppRequestComplete;
};
