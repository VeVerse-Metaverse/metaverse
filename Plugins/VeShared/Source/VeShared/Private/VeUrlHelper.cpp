#include "VeUrlHelper.h"

#include "GenericPlatform/GenericPlatformHttp.h"

TMap<FString, FString> FVeUrlHelper::ParseUrlQuery(const FString& Url) {
	const int32 Index = Url.Find("?");
	if (Index == INDEX_NONE) {
		return {};
	}

	const FString Query = Url.RightChop(Index + 1);

	TArray<FString> QueryTokens;
	Query.ParseIntoArray(QueryTokens, TEXT("&"));

	TMap<FString, FString> Result = {};

	for (const auto& Token : QueryTokens) {
		FString Key, Value;
		if (Token.Split("=", &Key, &Value)) {
			Result.Add(Key, FGenericPlatformHttp::UrlDecode(Value)); // Key and value
		} else {
			Result.Add(Token, FString{}); // Key only
		}
	}

	return Result;
}
