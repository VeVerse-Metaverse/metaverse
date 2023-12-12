// Author: Egor A. Pristavka. Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#pragma once

#include "VeDownload/Public/ManagedTextureRequester.h"
#include "TextureRequesterActor.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTextureLoadComplete, bool, bSuccessful, UTexture*, Texture);

UCLASS(Blueprintable, BlueprintType)
class VEGAME_API ATextureRequesterActor : public AActor, public IManagedTextureRequester {
	GENERATED_BODY()

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintCallable, BlueprintCosmetic)
	void RequestTexture(const FString& InUrl);

	/** Callback to set up the canvas texture once it was loaded. */
	virtual void NativeOnTextureLoaded(const FTextureLoadResult& InResult) override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FOnTextureLoadComplete OnTextureLoadComplete;

	// Sets default values for this actor's properties
	ATextureRequesterActor();

	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
