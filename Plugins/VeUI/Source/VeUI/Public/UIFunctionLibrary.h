// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.
#pragma once

#include "UIFriendInfo.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "UIFunctionLibrary.generated.h"

UCLASS()
class VEUI_API UUIFunctionLibrary final : public UBlueprintFunctionLibrary {
	GENERATED_BODY()

	UFUNCTION(BlueprintCallable)
	static TArray<FUIFriendInfo> SortUIFriendList(const TArray<FUIFriendInfo> ObjectArray, const bool Reversed);

	UFUNCTION(BlueprintCallable)
	static TArray<FUIFriendInfo> FilterUIFriendList(const TArray<FUIFriendInfo> ObjectArray, const FText& Text, EUIUserStatus Status = EUIUserStatus::Any);
};
