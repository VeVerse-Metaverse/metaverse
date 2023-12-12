// Authors: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022-2023 VeVerse AS. All rights reserved.
// Copyright (c) 2023 LE7EL AS. All rights reserved.
#pragma once

#include "Components/WidgetComponent.h"
#include "AiInfoWidgetComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class VEAI_API UAiInfoWidgetComponent final : public UWidgetComponent {
	GENERATED_BODY()

public:
	UAiInfoWidgetComponent();

	/**
	 * @brief Called when the person starts saying something to display subtitles and the voice icon.
	 */
	UFUNCTION(BlueprintCallable)
	void OnSay(const FText& InText);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Widget|Content")
	float MinVisibilityDistance = 500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Widget|Content")
	float MaxVisibilityDistance = 1000.0f;

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	FTimerHandle PlayerStateTimerHandle;
};
