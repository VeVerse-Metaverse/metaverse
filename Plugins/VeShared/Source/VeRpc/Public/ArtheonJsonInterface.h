// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once
#include "Dom/JsonObject.h"

struct FArtheonJsonInterface {
    virtual ~FArtheonJsonInterface() = default;
    virtual TSharedPtr<FJsonObject> ToJson() = 0;
    virtual void ParseJson(const TSharedPtr<FJsonObject> JsonObject) = 0;
};
