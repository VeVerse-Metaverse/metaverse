// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.

#pragma once

#include "UIDialogData.generated.h"

UENUM(BlueprintType)
enum class EUIDialogType : uint8 {
	Ok,
	OkCancel,
	YesNoCancel,
};

USTRUCT(BlueprintType)
struct FUIDialogData {
	GENERATED_BODY()

	FUIDialogData(): Type(EUIDialogType::Ok), bCloseOnButtonClick(false) {
	}

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Widget")
	EUIDialogType Type;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Widget")
	FText HeaderText;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Widget")
	FText MessageText;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Widget")
	FText FirstButtonText;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Widget")
	FText SecondButtonText;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Widget")
	FText ThirdButtonText;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Widget")
	bool bCloseOnButtonClick;
};
