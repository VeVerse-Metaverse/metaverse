#include "HttpRequest.h"

#include "VeShared.h"
#include "VeConfigLibrary.h"


FHttpRequest::FHttpRequest() {
	bLogHttpContent = FVeConfigLibrary::GetLogHttpContent();
}

FHttpRequest::~FHttpRequest() {}

bool FHttpRequest::Process() {
	if (!HttpRequest->OnProcessRequestComplete().IsBoundToObject(this)) {
		HttpRequest->OnProcessRequestComplete().BindSP(AsShared(), &FHttpRequest::OnCompleteCallback);
	}

	SelfToLive = AsShared();

	if (!HttpRequest->ProcessRequest()) {
		OnRequestComplete.ExecuteIfBound(AsShared(), false);
	}

	VeLogVerbose("[%p] HTTP Request: %s %s", this, *HttpRequest->GetVerb(), *HttpRequest->GetURL());

	if (bLogHttpContent) {
		// auto Headers = HttpRequest->GetAllHeaders();
		// if (Headers.Num()) {
		// 	VeLogVerbose("[%p] HTTP Request Headers: %s", this, *FString::Join(Headers, TEXT("; ")));
		// }

		if (HttpRequest->GetContentLength()) {
			FUTF8ToTCHAR TCHARData(reinterpret_cast<const ANSICHAR*>(HttpRequest->GetContent().GetData()), HttpRequest->GetContentLength());
			auto Content = FString(TCHARData.Length(), TCHARData.Get());
			Content.RemoveSpacesInline();
			VeLogVerbose("[%p] HTTP Request Content: %s", this, *Content);
		}
	}

	return true;
}

bool FHttpRequest::OnComplete(const FHttpRequestPtr& InRequest, const FHttpResponsePtr& InResponse, bool bSuccessful) {
	return bSuccessful;
}

void FHttpRequest::OnCompleteCallback(FHttpRequestPtr InRequest, FHttpResponsePtr InResponse, bool bSuccessful) {
	MyResponse = InResponse;

	if (!bSuccessful) {
		VeLogError("[%p] HTTP Response: Connection Error", this);
	} else if (InResponse->GetResponseCode() >= 500) {
		VeLogError("[%p] HTTP Response: %d Server Error", this, InResponse->GetResponseCode());
	} else {
		if (bLogHttpContent) {
			if (InResponse->GetContentLength()) {
				auto Content = InResponse->GetContentAsString();
				Content.ReplaceInline(TEXT("\r"), TEXT(""));
				Content.ReplaceInline(TEXT("\n"), TEXT(""));
				Content.ReplaceInline(TEXT("\t"), TEXT(""));
				VeLogVerbose("[%p] HTTP Response: %d %s", this, InResponse->GetResponseCode(), *Content);
			}
		}
	}

	const bool bResult = OnComplete(InRequest, InResponse, bSuccessful);
	OnRequestComplete.ExecuteIfBound(AsShared(), bResult);

	HttpRequest->OnProcessRequestComplete().Unbind();
	OnRequestComplete.Unbind();
	SelfToLive.Reset();
}

int32 FHttpRequest::GetResponseCode() const { return MyResponse->GetResponseCode(); }

bool FHttpRequest::CheckResponse() {
	if (!MyResponse.IsValid()) {
		return false;
	}

	return true;
}

bool FHttpRequest::CheckResponse(const FHttpResponsePtr Response) {
	if (!Response.IsValid()) {
		return false;
	}

	return true;
}
