// Author: Egor A. Pristavka

#pragma once

#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"


typedef TDelegate<void(const TSharedPtr<class FHttpRequest>& InRequest, bool bInSuccessful)> FOnRequestComplete;


class VESHARED_API FHttpRequest : public TSharedFromThis<FHttpRequest> {
public:
	FHttpRequest();
	virtual ~FHttpRequest();

	/**
	 * Initiates request processing.
	 * @returns true if request initiated successfully or false otherwise
	 */
	virtual bool Process();

	/**
	 * Internal HTTP request complete callback.
	 * @param Request Request sent to the HTTP server
	 * @param Response Response received from the server
	 * @param bSuccessful True if request was successful
	 */
	virtual bool OnComplete(const FHttpRequestPtr& InRequest, const FHttpResponsePtr& InResponse, bool bSuccessful);

	/**
	 * Returns HTTP error string 
	 */
	const FString& GetErrorString() const { return ErrorString; }

	/**
	 * Returns standard HTTP response code
	 */
	int32 GetResponseCode() const;

	/**
	 * Getter for the request complete callback.
	 */
	FOnRequestComplete& GetOnRequestComplete() { return OnRequestComplete; }

protected:
	/**
	 * Helper method to check response is valid.
	 */
	virtual bool CheckResponse();

	/**
	 * Helper method to check response is valid (API version 1).
	 */
	virtual bool CheckResponse(const FHttpResponsePtr Response);

	/**
	 * Internal HTTP request.
	 */
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();

	/**
	 * Request complete delegate.
	 */
	FOnRequestComplete OnRequestComplete;

	/**
	 * Cached response error string.
	 */
	FString ErrorString;

	/**
	 * Cached response code.
	 */
	// int32 ResponseCode = EHttpResponseCodes::Unknown;

	/**
	 * Log HTTP content if true.
	 **/
	bool bLogHttpContent = false;

	const FHttpResponsePtr& GetResponse() const { return MyResponse; }

private:
	/**
	 * Save object between Process() and OnCompleteCallback().
	 */
	TSharedPtr<FHttpRequest> SelfToLive;

	void OnCompleteCallback(FHttpRequestPtr InRequest, FHttpResponsePtr InResponse, bool bSuccessful);

	FHttpResponsePtr MyResponse;

};
