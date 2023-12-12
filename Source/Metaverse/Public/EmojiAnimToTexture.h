// Author: Egor A. Pristavka. 
// Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "EmojiAnimToTexture.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class METAVERSE_API UEmojiAnimToTexture : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UEmojiAnimToTexture();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
  //  UPROPERTY(EditAnywhere)
  //  UTextureRenderTarget2D* RenderTarget;

 //   UPROPERTY(EditAnywhere)
 //   UTexture2D* Picture;


	UFUNCTION(BlueprintCallable) //so visible in BluePrints
    void CreateTexture(FString TextureName, FString Path, UTextureRenderTarget2D *Target, int32 TextureSize);
	UPARAM(DisplayName="Name" FString TextureName);
	UPARAM(DisplayName="Path" FString Path);
	UPARAM(DisplayName="Target" UTextureRenderTarget2D Target);
	UPARAM(DisplayName="Size" int32 TextureSize);
};
