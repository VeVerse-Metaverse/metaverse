// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "ArtheonWebSocketsRequest.h"

bool FArtheonWebSocketsRequest::TriggerDelegates(const bool bSuccessful) const {
    return OnCppRequestComplete.ExecuteIfBound(ResponseMessage, bSuccessful);
}
