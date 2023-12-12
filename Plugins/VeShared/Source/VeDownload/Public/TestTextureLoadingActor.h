// Author: Egor A. Pristavka. Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#pragma once

#include "ManagedTextureRequester.h"
#include "GameFramework/Actor.h"
#include "TestTextureLoadingActor.generated.h"

UCLASS()
class VEDOWNLOAD_API ATestTextureLoadingActor final : public AActor, public IManagedTextureRequester {
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ATestTextureLoadingActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	/** Current texture index. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 TextureIndex;

	/** Last texture index used to be able to detach from the last used texture. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 LastTextureIndex;

	/** Texture URLs to be used for testing. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FString> TextureUrls;

	/** After this time the object will be destroyed and all its texture references will be removed. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Lifetime = 60.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* MeshComponent;

	UFUNCTION(BlueprintImplementableEvent)
	void OnTextureLoadedBP(UTexture* InTexture);

private:
	virtual void Tick(float DeltaTime) override;

	virtual void NativeOnTextureLoaded(const FTextureLoadResult& Result) override;
};
