// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.
#pragma once
#include "UIUserStatus.h"
#include "UIFriendInfo.generated.h"

USTRUCT(BlueprintType)
struct FUIFriendInfo {
	GENERATED_BODY()

	FUIFriendInfo(): Status(EUIUserStatus::Offline), AvatarTexture(nullptr) {
	}

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	EUIUserStatus Status;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FText Name;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UTexture2D* AvatarTexture;
};
