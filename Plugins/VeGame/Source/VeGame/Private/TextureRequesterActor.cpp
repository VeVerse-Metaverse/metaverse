// Author: Egor A. Pristavka. Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.


#include "TextureRequesterActor.h"


// Sets default values
ATextureRequesterActor::ATextureRequesterActor() {
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void ATextureRequesterActor::NativeOnTextureLoaded(const FTextureLoadResult& InResult) {
	if (IsRunningDedicatedServer()) {
		return;
	}

	OnTextureLoadComplete.Broadcast(InResult.bSuccessful, InResult.Texture.Get());
}

// Called when the game starts or when spawned
void ATextureRequesterActor::BeginPlay() {
	Super::BeginPlay();

}

void ATextureRequesterActor::RequestTexture(const FString& InUrl) {
	IManagedTextureRequester::RequestTexture(this, InUrl);
}

// Called every frame
void ATextureRequesterActor::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

}
