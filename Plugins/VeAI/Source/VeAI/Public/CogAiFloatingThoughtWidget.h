// Authors: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022-2023 VeVerse AS. All rights reserved.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CogAiFloatingThoughtWidget.generated.h"

class UTextBlock;

/** Floating thought widget. */
UCLASS(BlueprintType, Blueprintable)
class VEAI_API UCogAiFloatingThoughtWidget : public UUserWidget {
	GENERATED_BODY()

public:
	/** Image to draw as the emotion. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="UI")
	FSlateBrush EmotionBrush;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	TObjectPtr<UWidget> EmotionContainerWidget = nullptr;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	TObjectPtr<UTextBlock> EmotionWidget = nullptr;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	TObjectPtr<UTextBlock> ThoughtWidget = nullptr;

	void SetThoughtAndEmotion(const FString& InThought, const FString& InEmotionName) const;
};
