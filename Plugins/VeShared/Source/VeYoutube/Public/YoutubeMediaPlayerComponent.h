// Author: Egor A. Pristavka. 
// Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved. 

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "YoutubeMediaPlayerComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class VEYOUTUBE_API UYoutubeMediaPlayerComponent : public UActorComponent {
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UYoutubeMediaPlayerComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category="Metaverse|Youtube")
	void OpenUrl(const FString& Url);
	void RequestYoutubeVideoMetadata(const FString& Url);
	static FString GetVideoIdFromUrl(const FString& Url);
};
