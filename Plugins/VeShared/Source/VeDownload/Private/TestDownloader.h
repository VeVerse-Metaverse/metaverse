// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.


#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TestDownloader.generated.h"

UCLASS()
class VEDOWNLOAD_API ATestDownloader : public AActor {
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ATestDownloader();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void TestDownload(const FString& URL, const FString& Filename);

	UFUNCTION(BlueprintCallable)
	void TestDownloadCancel();

	UFUNCTION(BlueprintCallable)
	void TestDateTime(const FString& Key);
	
	// UFUNCTION(Exec)
	// static void VeTest();

private:
	TSharedPtr<class FHttpDownloadTask> HttpDownloadTask;
	TSharedPtr<class FHttpDownloadItem> HttpDownloadItem;
	static TSharedPtr<class FHttpDownloadTask> DownloadTask2;
};
