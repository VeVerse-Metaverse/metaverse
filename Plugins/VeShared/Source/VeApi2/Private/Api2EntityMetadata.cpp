// Authors: Khusan Yadgarov, Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "Api2EntityMetadata.h"

TSharedPtr<FJsonObject> IApi2BatchRequestMetadata::ToJson() {
	TSharedPtr<FJsonObject> JsonObject = IApiMetadata::ToJson();
	JsonObject->SetNumberField(Api::Fields::Offset, Offset);
	JsonObject->SetNumberField(Api::Fields::Limit, Limit);
	return JsonObject;
}

FString IApi2BatchRequestMetadata::ToUrlParams() {
	return FString::Printf(TEXT("%s=%d&%s=%d"), Api::Fields::Offset, Offset, Api::Fields::Limit, Limit);
}
