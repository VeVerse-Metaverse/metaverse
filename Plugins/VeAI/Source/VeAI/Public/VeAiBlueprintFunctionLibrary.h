// Authors: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022-2023 VeVerse AS. All rights reserved.
// Copyright (c) 2023 LE7EL AS. All rights reserved.


#pragma once

#include "CoreMinimal.h"
#include "VeAiEnums.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "VeAiBlueprintFunctionLibrary.generated.h"

class USoundWaveProcedural;

DECLARE_DELEGATE_TwoParams(FOnGenerateTtsUrlCompleteNative, const FString& /*Url*/, const FString& /*ErrorMessage*/);

DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnGenerateTtsUrlComplete, const FString&, Url, const FString&, ErrorMessage);

DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnGenerateTtsWaveComplete, USoundWaveProcedural*, Wave, const FString&, ErrorMessage);

/**
 * 
 */
UCLASS()
class VEAI_API UVeAiBlueprintFunctionLibrary : public UBlueprintFunctionLibrary {
	GENERATED_BODY()

public:
	/**
	 * @brief Generates a TTS URL.
	 */
	static void GenerateTtsUrl(UObject* WorldContextObject, const FString& OutputFormat, const FString& SampleRate, const FString& InText, const ECogAiVoice& SubjectVoice, const FOnGenerateTtsUrlCompleteNative& OnComplete);
	static void StreamTts(UObject* WorldContextObject, const FString& OutputFormat, const FString& SampleRate, const FString& InText, const ECogAiVoice& SubjectVoice, const FOnGenerateTtsUrlCompleteNative& OnComplete);

	/**
	 * @brief Generates a TTS URL.
	 */
	UFUNCTION(BlueprintCallable, Category = "VeAi", meta=(AutoCreateRefTerm="OnComplete", WorldContext="WorldContextObject"))
	static void GenerateTtsUrl(UObject* WorldContextObject, const FString& OutputFormat, const FString& SampleRate, const FString& InText, const ECogAiVoice& SubjectVoice, const FOnGenerateTtsUrlComplete& OnComplete);

	/**
	 * @brief Generates a TTS URL.
	 */
	UFUNCTION(BlueprintCallable, Category = "VeAi", meta=(AutoCreateRefTerm="OnComplete", WorldContext="WorldContextObject"))
	static void StreamTts(UObject* WorldContextObject, const FString& OutputFormat, const FString& SampleRate, const FString& InText, const ECogAiVoice& SubjectVoice, const FOnGenerateTtsUrlComplete& OnComplete);

	/**
	 * @brief Removes XML tags from the input string.
	 */
	UFUNCTION(BlueprintCallable, Category = "VeAi", meta=(AutoCreateRefTerm="OnComplete", WorldContext="WorldContextObject"))
	static FString SanitizeTtsText(const FString& InText);
};
