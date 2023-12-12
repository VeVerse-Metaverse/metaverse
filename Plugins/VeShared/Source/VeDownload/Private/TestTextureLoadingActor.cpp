// Author: Egor A. Pristavka. Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#include "TestTextureLoadingActor.h"

#include "ManagedTexture.h"
#include "DownloadTextureSubsystem.h"
#include "VeDownload.h"
#include "VeShared.h"

// Sets default values
ATestTextureLoadingActor::ATestTextureLoadingActor() {
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 15.f;
}

// Called when the game starts or when spawned
void ATestTextureLoadingActor::BeginPlay() {
	Super::BeginPlay();
}

void ATestTextureLoadingActor::EndPlay(const EEndPlayReason::Type EndPlayReason) {
	Super::EndPlay(EndPlayReason);

	ReleaseAllTextures();
}

// Called every frame
void ATestTextureLoadingActor::Tick(const float DeltaTime) {
	Super::Tick(DeltaTime);

	if (TextureUrls.Num() <= 0) {
		return;
	}

	if (TextureIndex <= INDEX_NONE) {
		return;
	}

	// It should be called here, but we need to test that even pending requests are canceled and textures are unloaded upon requester deletion. 
	// if (Lifetime < 0) {
	// 	Destroy();
	// 	return;
	// }

	LastTextureIndex = TextureIndex;
	TextureIndex++;
	if (TextureIndex >= TextureUrls.Num()) {
		TextureIndex = 0;
	}

	{
		// Asynchronous texture loading example:
		RequestTexture(this, TextureUrls[TextureIndex]);
	}

	Lifetime -= DeltaTime;

	if (Lifetime < 0) {
		Destroy();
	}
}

void ATestTextureLoadingActor::NativeOnTextureLoaded(const FTextureLoadResult& Result) {
	if (FVeDownloadModule* DownloadModule = FVeDownloadModule::Get()) {
		if (const TSharedPtr<FDownloadTextureSubsystem> TextureSubsystem = DownloadModule->GetTextureSubsystem()) {
			{
				TextureSubsystem->RemoveObjectReference(TextureUrls[LastTextureIndex], this);
			}
		} else { LogErrorF("failed to get subsystem F%s%sSubsystem", STRINGIFY(Download), STRINGIFY(Texture)); }
	} else { LogErrorF("failed to get module FVe%sModule", STRINGIFY(Download)); };

	if (Result.Texture.IsValid()) {
		OnTextureLoadedBP(Result.Texture.Get());
	}
}
