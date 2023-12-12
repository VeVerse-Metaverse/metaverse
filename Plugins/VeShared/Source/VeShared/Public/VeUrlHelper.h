#pragma once

class VESHARED_API FVeUrlHelper {
public:
	static TMap<FString, FString> ParseUrlQuery(const FString& Url);
};
