// Authors: Egor A. Pristavka, Nikolay Bulatov. 
// Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "Placeable/PlaceableOpenSeaCanvas.h"

#include "MediaPlayer.h"
#include "MediaSoundComponent.h"
#include "MediaTexture.h"
#include "PlaceableComponent.h"
#include "StreamMediaSource.h"
#include "VeShared.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "ApiFileMetadata.h"
#include "VeApi.h"
#include "VeDownload.h"
#include "VeGameModule.h"
#include "Structs/PlaceableAssetMetadata.h"
#include "UI/UIEditorMediaUrlPropertiesWidget.h"

APlaceableOpenSeaCanvas::APlaceableOpenSeaCanvas() {
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	FrameMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("FrameMesh");
	if (FrameMeshComponent) {
		RootComponent = FrameMeshComponent;
		FrameMeshComponent->SetRelativeLocation(FVector::ZeroVector);
		FrameMeshComponent->SetCanEverAffectNavigation(false);
		FrameMeshComponent->SetIsReplicated(true);
	}

	CanvasMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("CanvasMesh");
	if (CanvasMeshComponent) {
		CanvasMeshComponent->SetupAttachment(RootComponent);
		CanvasMeshComponent->SetRelativeLocation(FVector::ZeroVector);
		CanvasMeshComponent->SetCanEverAffectNavigation(false);
		CanvasMeshComponent->SetIsReplicated(true);
	}

#pragma region Stream

	StreamMediaSource = CreateDefaultSubobject<UStreamMediaSource>("StreamMediaSource");

	StreamMediaPlayer = CreateDefaultSubobject<UMediaPlayer>("StreamMediaPlayer");
	if (StreamMediaPlayer) {
		StreamMediaPlayer->SetLooping(true);
	}

	StreamAudioComponent = CreateDefaultSubobject<UMediaSoundComponent>("StreamMediaSound");
	if (StreamAudioComponent) {
		StreamAudioComponent->SetupAttachment(RootComponent);
		StreamAudioComponent->SetRelativeLocation(FVector{25.0f, 0.0f, 0.0f});
		StreamAudioComponent->SetRelativeRotation(FRotator::ZeroRotator);
		StreamAudioComponent->SetWorldScale3D(FVector::OneVector);
	}

	StreamMediaTexture = CreateDefaultSubobject<UMediaTexture>("StreamMediaTexture");
	if (StreamMediaTexture) {
		StreamMediaTexture->SetMediaPlayer(StreamMediaPlayer);
		StreamMediaTexture->EnableGenMips = false;
		StreamMediaTexture->Filter = TF_Trilinear;
	}

#pragma endregion
}

void APlaceableOpenSeaCanvas::OnPlaceableMetadataChanged(const FVePlaceableMetadata& InMetadata, EPlaceableChangeType PlaceableChange) {
	if (IsRunningDedicatedServer()) return;

	if (InMetadata.Type == Api::PlaceableTypes::Video2D) {
		if (const FVeFileMetadata* FileMetadata = InMetadata.Files.FindByPredicate([](const FVeFileMetadata& InFileMetadata) {
			return InFileMetadata.Type == EApiFileType::Video;
		})) {
			SetStreamMediaSourceUrl(FileMetadata->Url);
		}
	} else {
		// Assume Image2D
		if (const FVeFileMetadata* FileMetadata = InMetadata.Files.FindByPredicate([](const FVeFileMetadata& InFileMetadata) {
			return InFileMetadata.Type != EApiFileType::TextureDiffuse;
		})) {
			RequestTexture(this, FileMetadata->Url, TEXTUREGROUP_World);
		} else {
			FileMetadata = InMetadata.Files.FindByPredicate([](const FVeFileMetadata& InFileMetadata) {
				return InFileMetadata.Type == EApiFileType::ImageFull;
			});

			if (FileMetadata) {
				RequestTexture(this, FileMetadata->Url, TEXTUREGROUP_World);
			} else {
				FileMetadata = InMetadata.Files.FindByPredicate([](const FVeFileMetadata& InFileMetadata) {
					return InFileMetadata.Type == EApiFileType::ImagePreview;
				});

				if (FileMetadata) {
					RequestTexture(this, FileMetadata->Url, TEXTUREGROUP_World);
				} else {
					LogWarningF("failed to get a file");
				}
			}
		}
	}
}

// Called when the game starts or when spawned
void APlaceableOpenSeaCanvas::BeginPlay() {
	Super::BeginPlay();

	if (FrameMeshComponent) {
		FrameMeshComponent->SetStaticMesh(FrameMesh);
	}

	if (CanvasMeshComponent) {
		CanvasMeshComponent->SetStaticMesh(CanvasMesh);
	}

	FrameMaterialInstance = UMaterialInstanceDynamic::Create(FrameMaterial, this);
	if (FrameMeshComponent && FrameMaterialInstance) {
		FrameMeshComponent->SetMaterial(0, FrameMaterialInstance);
		FrameMaterialInstance->SetVectorParameterValue(FrameMaterialColorParameterName, FrameColor);
	}

	if (PlaceableComponent) {
		PlaceableComponent->GetOnPlaceableMetadataChanged().AddUObject(this, &APlaceableOpenSeaCanvas::OnPlaceableMetadataChanged);
	}

	UpdateCanvasDimensions();
}

void APlaceableOpenSeaCanvas::SetStreamMediaSourceUrl(const FString& InUrl) {
	CanvasMaterialInstance = UMaterialInstanceDynamic::Create(CanvasVideoMaterial, this);
	if (CanvasMeshComponent && CanvasMaterialInstance) {
		CanvasMeshComponent->SetMaterial(0, CanvasMaterialInstance);
	}

	if (StreamMediaPlayer) {
		StreamMediaPlayer->OpenUrl(InUrl);
		StreamMediaPlayer->Play();
		if (CanvasMaterialInstance) {
			CanvasMaterialInstance->SetTextureParameterValue(CanvasMaterialTextureParameterName, StreamMediaTexture);
		}
		StreamMediaTexture->UpdateResource();
	}

	// Update sound attenuation settings on playback start
	if (StreamAudioComponent) {
		if (StreamAudioAttenuationSettings) {
			StreamAudioComponent->AttenuationSettings = StreamAudioAttenuationSettings;
		}

		StreamAudioComponent->SetMediaPlayer(StreamMediaPlayer);
#if WITH_EDITOR
		StreamAudioComponent->SetDefaultMediaPlayer(StreamMediaPlayer);
#endif

		// It seems that this call causes exception
		// StreamAudioComponent->UpdatePlayer();
	}
}

void APlaceableOpenSeaCanvas::NativeOnTextureLoaded(const FTextureLoadResult& InResult) {
	if (IsRunningDedicatedServer()) return;

	CanvasMaterialInstance = UMaterialInstanceDynamic::Create(CanvasMaterial, this);
	if (CanvasMeshComponent && CanvasMaterialInstance) {
		CanvasMeshComponent->SetMaterial(0, CanvasMaterialInstance);
	}

	if (CanvasMaterialInstance) {
		CanvasMaterialInstance->SetTextureParameterValue(CanvasMaterialTextureParameterName, InResult.Texture.Get());
		CanvasMaterialInstance->SetScalarParameterValue(CanvasMaterialEmissivePowerParameterName, EmissivePower);
	}
}

void APlaceableOpenSeaCanvas::UpdateCanvasDimensions() const {
	const auto RootComp = GetRootComponent();
	if (!RootComp) {
		return;
	}

	float LocalHeight = RootComp->GetComponentScale().Z;
	if (LocalHeight == 1) {
		LocalHeight = 100.f;
	}

	if (LocalHeight <= 0) {
		LocalHeight = DefaultHeight;
	}

	float LocalWidth = RootComp->GetComponentScale().Y;
	if (LocalWidth == 1) {
		LocalWidth = 100.f;
	}

	if (LocalWidth <= 0) {
		LocalWidth = DefaultWidth;
	}

	if (LocalHeight > 0 && LocalWidth > 0) {
		if (FrameMeshComponent) {
			FrameMeshComponent->SetWorldScale3D(FVector(1.0f, LocalWidth + 0.1f, LocalHeight + 0.1f));
		} else {
			VeLogErrorFunc("invalid frame mesh component");
		}

		if (CanvasMeshComponent) {
			CanvasMeshComponent->SetWorldScale3D(FVector(1.0f, LocalWidth - 1.1f, LocalHeight - 1.1f));
			CanvasMeshComponent->SetRelativeLocation(FVector(1.001f, 0.0f, 0.0f));
		} else {
			VeLogErrorFunc("invalid canvas mesh component");
		}
	} else {
		VeLogErrorFunc("invalid canvas dimensions");
	}
}
