// Authors: Khusan Yadgarov, Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "Api2AiActionRequest.h"

bool FApi2AiMessage::FromJson(const TSharedPtr<FJsonObject> JsonObject) {
	ensure(false);
	return IApiMetadata::FromJson(JsonObject);
}

bool FApi2AiActionRequest::Process() {
	ensure(false);
	return FApi2Request::Process();
}

bool FApi2AiActionRequest::OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) {
	ensure(false);
	return FApi2Request::OnComplete(Request, Response, bSuccessful);
}
