// Author: Egor A. Pristavka. 
// Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#pragma once
namespace OpenSea {
	const static auto Key = TEXT("bf90adf0105b44ba88580811d337a7c8");

#if UE_BUILD_SHIPPING
	const static FString RootUrl = TEXT("https://api.opensea.io/api/v1/");
#else
	const static FString RootUrl = TEXT("https://api.opensea.io/api/v1/");
	// const static FString RootUrl = TEXT("https://rinkeby-api.opensea.io/api/v1/");
#endif

	const static FString AssetUrl = RootUrl / TEXT("asset");
	const static FString AssetsUrl = RootUrl / TEXT("assets");
}
