// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.
#pragma once
#include "UINotificationData.generated.h"

UENUM(BlueprintType)
enum class EUINotificationType : uint8 {
	Info,
	Success,
	Warning,
	Failure
};

USTRUCT(BlueprintType)
struct FUINotificationData {
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Widget")
	EUINotificationType Type = EUINotificationType::Info;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Widget")
	float Lifetime = 3.f;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Widget")
	FText Header;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Widget")
	FText Message;
};
