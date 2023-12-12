// Author: Egor A. Pristavka. 
// Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.


#pragma once

#include "UserScoreFloat.generated.h"

/**
 * @brief A struct that holds a name and a score.
 */
USTRUCT(BlueprintType)
struct VEGAME_API FUserScoreFloat {
	GENERATED_BODY()

	/**
	 * @brief The ID of the user (optional).
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "UserScoreFloat")
	FGuid ID;

	/**
	 * @brief The name of the user (optional).
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "UserScoreFloat")
	FString Name;

	/**
	 * @brief The score of the user.
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "UserScoreFloat")
	float Score = 0.0f;
};

UCLASS()
class VEGAME_API UUserScoreFloatFunctionLibrary : public UBlueprintFunctionLibrary {
	GENERATED_BODY()

public:
	/**
	 * @brief Sorts an array of UserScoreFloat structs by their score.
	 * @param UserScoreFloatArray The array to sort.
	 * @param bDescending Sort in descending order (default: true).
	 */
	UFUNCTION(BlueprintCallable, Category = "UserScoreFloat")
	static TArray<FUserScoreFloat> SortUserScoreFloatArray(TArray<FUserScoreFloat> UserScoreFloatArray, bool bDescending = true);
};
