// Authors: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022-2023 VeVerse AS. All rights reserved.
// Copyright (c) 2023 LE7EL AS. All rights reserved.


#include "CogAiFloatingThoughtWidget.h"

#include "Components/TextBlock.h"

void UCogAiFloatingThoughtWidget::SetThoughtAndEmotion(const FString& InThought, const FString& InEmotionName) const {
	// todo: Set the emotion material or image.

	if (InEmotionName.IsEmpty()) {
		EmotionContainerWidget->SetVisibility(ESlateVisibility::Collapsed);
	} else {
		EmotionContainerWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}

	if (EmotionWidget) {
		EmotionWidget->SetText(FText::FromString(InEmotionName));
	}

	if (ThoughtWidget) {
		ThoughtWidget->SetText(FText::FromString(InThought));
	}
}
