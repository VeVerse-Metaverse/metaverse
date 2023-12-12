// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.


#pragma once

#include "CoreMinimal.h"
#include "Download1Metadata.h"
#include "GameFramework/Actor.h"
#include "TestDownload.generated.h"

class FVeDownload1;

UCLASS(Blueprintable, BlueprintType)
class VEDOWNLOAD_API ATestDownload final : public AActor {
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ATestDownload();

	TSharedPtr<FVeDownload1> Download;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Url = TEXT("http://ipv4.download.thinkbroadband.com/100MB.zip");

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void StartDownload();

public:
	UFUNCTION(BlueprintImplementableEvent)
	void OnDownloadProgress(const FDownload1ProgressMetadata& Metadata);

	UFUNCTION(BlueprintImplementableEvent)
	void OnDownloadComplete(const FDownload1ResultMetadata& Metadata);

	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
