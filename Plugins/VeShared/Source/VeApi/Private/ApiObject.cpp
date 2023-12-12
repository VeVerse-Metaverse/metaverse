// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "ApiObject.h"

#include "ApiCommon.h"

// bool FApiObjectRef::FromJson(const TSharedPtr<FJsonObject> JsonObject) {
// 	bool bOk = FApiEntityRef::FromJson(JsonObject);
//
// 	if (JsonObject->HasTypedField<EJson::String>(Api::Fields::Type)) {
// 		bPublic = JsonObject->GetBoolField(Api::Fields::Type);
// 	} else {
// 		bPublic = false;
// 	}
//
// 	
// 	return bOk;
// }
//
// TSharedPtr<FJsonObject> FApiObjectRef::ToJson() {
// 	return FApiEntityRef::ToJson();
// }
