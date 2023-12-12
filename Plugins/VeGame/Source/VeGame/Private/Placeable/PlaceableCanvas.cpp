// Authors: Egor A. Pristavka, Nikolay Bulatov. 
// Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "Placeable/PlaceableCanvas.h"

#include "MediaPlayer.h"
#include "MediaSoundComponent.h"
#include "MediaTexture.h"
#include "PlaceableComponent.h"
#include "StreamMediaSource.h"
#include "VeShared.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "ApiFileMetadata.h"
#include "VeApi.h"
#include "IMediaEventSink.h"
#include "VeDownload.h"
#include "VeGameModule.h"
#include "Structs/PlaceableAssetMetadata.h"
#include "UI/UIEditorMediaUrlPropertiesWidget.h"

APlaceableCanvas::APlaceableCanvas() {
	PrimaryActorTick.bCanEverTick = false;

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

	// StreamMediaSource = CreateDefaultSubobject<UStreamMediaSource>("StreamMediaSource");

	// StreamMediaPlayer = CreateDefaultSubobject<UMediaPlayer>("StreamMediaPlayer");
	// if (StreamMediaPlayer) {
	// 	StreamMediaPlayer->SetLooping(true);
	// }

	// StreamAudioComponent = CreateDefaultSubobject<UMediaSoundComponent>("StreamMediaSound");
	// if (StreamAudioComponent) {
	// 	StreamAudioComponent->SetupAttachment(RootComponent);
	// 	StreamAudioComponent->SetRelativeLocation(FVector{25.0f, 0.0f, 0.0f});
	// 	StreamAudioComponent->SetRelativeRotation(FRotator::ZeroRotator);
	// 	StreamAudioComponent->SetWorldScale3D(FVector::OneVector);
	// }
	//
	// StreamMediaTexture = CreateDefaultSubobject<UMediaTexture>("StreamMediaTexture");
	// if (StreamMediaTexture) {
	// 	StreamMediaTexture->SetMediaPlayer(StreamMediaPlayer);
	// 	StreamMediaTexture->EnableGenMips = false;
	// 	StreamMediaTexture->Filter = TF_Trilinear;
	// }

#pragma endregion
}

void APlaceableCanvas::OnPlaceableMetadataChanged(const FVePlaceableMetadata& InMetadata, EPlaceableChangeType PlaceableChange) {
	if (IsRunningDedicatedServer()) {
		UpdateCanvasDimensions();
		return;
	}

	const auto* FileMetadata = InMetadata.Files.FindByPredicate([](const FVeFileMetadata& InFileMetadata) {
		return InFileMetadata.Type == EApiFileType::Video;
	});
	if (FileMetadata) {
		// SetPlaceableState(EPlaceableState::Downloading);
		// SetStreamMediaSourceUrl(FileMetadata->Url);
		// BeginStreamVideoSourceUrl(FileMetadata->Url);
		StreamMediaSource = NewObject<UStreamMediaSource>(this);
		StreamMediaSource->StreamUrl = FileMetadata->Url;
		StreamMediaPlayer->OpenSource(StreamMediaSource);
		return;
	}

	if (!FileMetadata) {
		FileMetadata = InMetadata.Files.FindByPredicate([](const FVeFileMetadata& InFileMetadata) {
			return InFileMetadata.Type != EApiFileType::TextureDiffuse;
		});
	}
	
	if (!FileMetadata) {
		FileMetadata = InMetadata.Files.FindByPredicate([](const FVeFileMetadata& InFileMetadata) {
			return InFileMetadata.Type == EApiFileType::ImageFull;
		});
	}
	if (!FileMetadata) {
		FileMetadata = InMetadata.Files.FindByPredicate([](const FVeFileMetadata& InFileMetadata) {
			return InFileMetadata.Type == EApiFileType::ImagePreview;
		});
	}

	if (FileMetadata) {
		SetPlaceableState(EPlaceableState::Downloading);
		RequestTexture(this, FileMetadata->Url, TEXTUREGROUP_World);
		return;
	}


	VeLogWarningFunc("Failed to get a file");
}

void APlaceableCanvas::PostInitProperties() {
	Super::PostInitProperties();

	if (!IsTemplate()) {
		StreamMediaPlayer = NewObject<UMediaPlayer>(this);
		StreamMediaPlayer->PlayOnOpen = true;
		StreamMediaPlayer->SetLooping(true);

		StreamMediaTexture = NewObject<UMediaTexture>(this);
		StreamMediaTexture->AutoClear = true;
		StreamMediaTexture->SetMediaPlayer(StreamMediaPlayer);
		StreamMediaTexture->UpdateResource();

		if (IsValid(CanvasVideoMaterial)) {
			CanvasMaterialInstance = UMaterialInstanceDynamic::Create(CanvasVideoMaterial, this);
			CanvasMaterialInstance->SetTextureParameterValue(TEXT("T"), StreamMediaTexture);
			CanvasMeshComponent->SetMaterial(0, CanvasMaterialInstance);
		}

		StreamAudioComponent = NewObject<UMediaSoundComponent>(this);
		StreamAudioComponent->RegisterComponent();
		StreamAudioComponent->Channels = EMediaSoundChannels::Stereo;
		StreamAudioComponent->bIsUISound = true;

		StreamAudioComponent->SetMediaPlayer(StreamMediaPlayer);
		StreamAudioComponent->Initialize();
		StreamAudioComponent->UpdatePlayer();
	}
}

void APlaceableCanvas::PostInitializeComponents() {
	Super::PostInitializeComponents();

	if (FrameMeshComponent) {
		FrameMeshComponent->SetStaticMesh(FrameMesh);
	}

	if (CanvasMeshComponent) {
		CanvasMeshComponent->SetStaticMesh(CanvasMesh);
	}

	if (PlaceableComponent) {
		PlaceableComponent->GetOnPlaceableMetadataChanged().AddUObject(this, &APlaceableCanvas::OnPlaceableMetadataChanged);
	}

	UpdateCanvasDimensions();

	if (IsRunningDedicatedServer()) {
		return;
	}

	FrameMaterialInstance = UMaterialInstanceDynamic::Create(FrameMaterial, this);
	if (FrameMeshComponent && FrameMaterialInstance) {
		FrameMeshComponent->SetMaterial(0, FrameMaterialInstance);
		FrameMaterialInstance->SetVectorParameterValue(FrameMaterialColorParameterName, FrameColor);
	}
}

// Called when the game starts or when spawned
void APlaceableCanvas::BeginPlay() {
	Super::BeginPlay();
}

void APlaceableCanvas::EndPlay(const EEndPlayReason::Type EndPlayReason) {
	Super::EndPlay(EndPlayReason);
	if (IsValid(StreamMediaPlayer)) {
		StreamMediaPlayer->Close();
	}

	if (IsValid(StreamAudioComponent)) {
		StreamAudioComponent->Stop();
	}
}

void APlaceableCanvas::HandleMediaPlayerEvent(EMediaEvent EventType) {
	switch (EventType) {
	case EMediaEvent::MediaClosed:
		if (IsValid(StreamMediaPlayer)) {
			StreamMediaPlayer->Close();
		}

		if (IsValid(StreamAudioComponent)) {
			StreamAudioComponent->Stop();
		}
		break;
	case EMediaEvent::PlaybackEndReached:
		if (!StreamMediaPlayer->IsLooping()) {
			if (IsValid(StreamAudioComponent)) {
				StreamAudioComponent->Stop();
			}
		}
		break;
	case EMediaEvent::PlaybackResumed:
		if (!bIsMuted) {
			if (IsValid(StreamAudioComponent)) {
				StreamAudioComponent->Start();
				StreamAudioComponent->UpdatePlayer();
			}
		}
		break;
	case EMediaEvent::PlaybackSuspended:
		if (IsValid(StreamAudioComponent)) {
			StreamAudioComponent->Stop();
		}
		break;
	default:
		break;
	}
}

void APlaceableCanvas::Play() {
	StreamMediaPlayer->SetRate(1.0f);
	if (IsValid(StreamMediaPlayer)) {
		if (StreamMediaPlayer->IsReady() && !StreamMediaPlayer->IsPlaying()) {
			StreamMediaPlayer->Play();
		}
	}
}

void APlaceableCanvas::PlayFromStart() {
	if (IsValid(StreamMediaPlayer)) {
		StreamMediaPlayer->Rewind();
	}
	Play();
}

void APlaceableCanvas::Pause() {
	if (IsValid(StreamMediaPlayer)) {
		StreamMediaPlayer->Pause();
	}
}

bool APlaceableCanvas::IsPaused() const {
	return StreamMediaPlayer->IsPaused();
}

bool APlaceableCanvas::IsPlaying() const {
	return StreamMediaPlayer->IsPlaying();
}

void APlaceableCanvas::SetIsMuted(bool bInIsMuted) {
	bIsMuted = bInIsMuted;
	if (bIsMuted) {
		StreamAudioComponent->Stop();
	} else if (StreamMediaPlayer->IsPlaying()) {
		StreamAudioComponent->Start();
	}
}

void APlaceableCanvas::SetStreamMediaSourceUrl(const FString& InUrl) {
	SetPlaceableState(EPlaceableState::None);

	CanvasMaterialInstance = UMaterialInstanceDynamic::Create(CanvasVideoMaterial, this);
	if (CanvasMeshComponent && CanvasMaterialInstance) {
		CanvasMeshComponent->SetMaterial(0, CanvasMaterialInstance);
	}

	if (IsValid(StreamMediaPlayer)) {
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

void APlaceableCanvas::NativeOnTextureLoaded(const FTextureLoadResult& InResult) {
	SetPlaceableState(EPlaceableState::None);

	CanvasMaterialInstance = UMaterialInstanceDynamic::Create(CanvasMaterial, this);
	if (CanvasMeshComponent && CanvasMaterialInstance) {
		CanvasMeshComponent->SetMaterial(0, CanvasMaterialInstance);
	}

	if (CanvasMaterialInstance) {
		CanvasMaterialInstance->SetTextureParameterValue(CanvasMaterialTextureParameterName, InResult.Texture.Get());
		CanvasMaterialInstance->SetScalarParameterValue(CanvasMaterialEmissivePowerParameterName, EmissivePower);
	}
}

void APlaceableCanvas::UpdateCanvasDimensions() const {
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
			VeLogErrorFunc("Invalid frame mesh component");
		}

		if (CanvasMeshComponent) {
			CanvasMeshComponent->SetWorldScale3D(FVector(1.0f, LocalWidth - 1.1f, LocalHeight - 1.1f));
			CanvasMeshComponent->SetRelativeLocation(FVector(1.001f, 0.0f, 0.0f));
		} else {
			VeLogErrorFunc("Invalid canvas mesh component");
		}
	} else {
		VeLogErrorFunc("Invalid canvas dimensions");
	}
}
