#pragma once

#include "VeAiSimpleFsmLocationLink.generated.h"

USTRUCT(BlueprintType)
struct VEAI_API FVeAiSimpleFsmLocationLink {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	FString Name = {};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	FString Description = {};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	FString Target = {};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	FString Object = {};
};
