// Authors: Egor A. Pristavka, Nikolay Bulatov. 
// Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "PlaceableUrlActor.h"

#include "ApiFileUploadRequest.h"
#include "ApiPlaceableRequest.h"
#include "ApiFileUploadSubsystem.h"
#include "FileHeaderRequest.h"
#include "MediaPlayer.h"
#include "MediaSoundComponent.h"
#include "MediaTexture.h"
#include "PlaceableComponent.h"
#include "StreamMediaSource.h"
#include "VeApi.h"
#include "VeDownload.h"
#include "VeShared.h"
#include "VeGameModule.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Structs/PlaceableAssetMetadata.h"
#include "UI/UIUrlPropertiesWidget.h"
#include "DownloadChunkSubsystem.h"

APlaceableUrlActor::APlaceableUrlActor() {
	PrimaryActorTick.bCanEverTick = true;
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

void APlaceableUrlActor::BeginPlay() {
	if (FrameMeshComponent) {
		FrameMeshComponent->SetStaticMesh(FrameMesh);
	}

	if (CanvasMeshComponent) {
		CanvasMeshComponent->SetStaticMesh(CanvasMesh);
	}

	if (IsRunningDedicatedServer()) {
		Super::BeginPlay();
	}

	FrameMaterialInstance = UMaterialInstanceDynamic::Create(FrameMaterial, this);
	if (FrameMeshComponent && FrameMaterialInstance) {
		FrameMeshComponent->SetMaterial(0, FrameMaterialInstance);
		FrameMaterialInstance->SetVectorParameterValue(FrameMaterialColorParameterName, FrameColor);
	}

	if (PlaceableComponent) {
		PlaceableComponent->GetOnPlaceableMetadataChanged().AddWeakLambda(this, [=](const FVePlaceableMetadata& InMetadata, EPlaceableChangeType /*InChange*/) {
			OnPlaceableMetadataChanged(InMetadata);
		});
	}

	Super::BeginPlay();
}

void APlaceableUrlActor::Tick(const float DeltaTime) {
	Super::Tick(DeltaTime);
}

void APlaceableUrlActor::SetStreamMediaSourceUrl(const FString& InUrl) {
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

		// StreamAudioComponent->UpdatePlayer();
	}
}

void APlaceableUrlActor::NativeOnTextureLoaded(const FTextureLoadResult& InResult) {
	if (IsRunningDedicatedServer()) {
		return;
	}

	CanvasMaterialInstance = UMaterialInstanceDynamic::Create(CanvasMaterial, this);
	if (CanvasMeshComponent && CanvasMaterialInstance) {
		CanvasMeshComponent->SetMaterial(0, CanvasMaterialInstance);
	}

	if (CanvasMaterialInstance) {
		CanvasMaterialInstance->SetTextureParameterValue(CanvasMaterialTextureParameterName, InResult.Texture.Get());
		CanvasMaterialInstance->SetScalarParameterValue(CanvasMaterialEmissivePowerParameterName, EmissivePower);
	}
}

void APlaceableUrlActor::OnPlaceableMetadataChanged(const FVePlaceableMetadata& InMetadata) {
	if (IsRunningDedicatedServer()) {
		return;
	}

	if (InMetadata.Type == Api::PlaceableTypes::Image2D) {
		if (const FVeFileMetadata* FileMetadata = InMetadata.Files.FindByPredicate([](const FVeFileMetadata& InFileMetadata) {
			return InFileMetadata.Type == EApiFileType::TextureDiffuse;
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
	} else if (InMetadata.Type == Api::PlaceableTypes::Video2D) {
		if (const FVeFileMetadata* FileMetadata = InMetadata.Files.FindByPredicate([](const FVeFileMetadata& InFileMetadata) {
			return InFileMetadata.Type == EApiFileType::Video;
		})) {
			SetStreamMediaSourceUrl(FileMetadata->Url);
		}
	}
}

void APlaceableUrlActor::OnSaved(const FString& Url) {

	GET_MODULE_SUBSYSTEM(ChunkDownloadSubsystem, Download, Chunk);
	if (ChunkDownloadSubsystem) {
		const auto CallbackPtr = MakeShared<FOnFileHeaderRequestComplete>();
		if (!Url.Contains(TEXT("opensea.io"))) {
			CallbackPtr->BindWeakLambda(this, [=](const FFileHeaderResult& InResult, const bool bInSuccessful, const FString& InError) {
				if (bInSuccessful) {
					const auto MimeType = InResult.GetContentMimeType();
					FString MediaType = GetMediaType(MimeType, InResult.Url);

					FPlaceableAssetMetadata RequestMetadata;
					RequestMetadata.MimeType = InResult.GetContentMimeType();
					RequestMetadata.MediaType = MediaType;
					RequestMetadata.Url = Url;

					UpdatePlaceableUrl(RequestMetadata);
				} else {
					LogErrorF("failed to load nft asset: %s", *InError);
				}
			});
			ChunkDownloadSubsystem->GetFileHeaders(Url, CallbackPtr);
		}
	}
}

void APlaceableUrlActor::UpdatePlaceableUrl(const FPlaceableAssetMetadata& InMetadata) {
	GET_MODULE_SUBSYSTEM(FileUploadSubsystem, Api, FileUpload);
	if (FileUploadSubsystem) {
		// Replace file
		const auto ReplaceFileCallback = MakeShared<FOnFileRequestCompleted>();
		ReplaceFileCallback->BindLambda([=](const FApiFileMetadata InFileMetadata, const bool bInFileRequestSuccessful, const FString& InFileRequestError) {
			if (!bInFileRequestSuccessful) {
				VeLogErrorFunc("failed to update placeable url: %s", *InFileRequestError);
			} else {
				UpdatePlaceableOnServer();
			}
		});

		FApiEntityFileLinkMetadata LinkFileRequestMetadata;
		LinkFileRequestMetadata.File = InMetadata.Url;
		LinkFileRequestMetadata.EntityId = InMetadata.Id;
		if (InMetadata.MediaType == Api::PlaceableTypes::Mesh3D) {
			LinkFileRequestMetadata.FileType = Api::FileTypes::Model;
		} else if (InMetadata.MediaType == Api::PlaceableTypes::Video2D) {
			LinkFileRequestMetadata.FileType = Api::FileTypes::Video;
		} else if (InMetadata.MediaType == Api::PlaceableTypes::Audio) {
			LinkFileRequestMetadata.FileType = Api::FileTypes::Audio;
		} else if (InMetadata.MediaType == Api::PlaceableTypes::Pdf) {
			LinkFileRequestMetadata.FileType = Api::FileTypes::Pdf;
		} else {
			LinkFileRequestMetadata.FileType = Api::FileTypes::ImageFullInitial;
		}

		FileUploadSubsystem->Replace(LinkFileRequestMetadata, ReplaceFileCallback);
	}
}

UUIPlaceablePropertiesWidget* APlaceableUrlActor::GetPlaceablePropertiesWidget_Implementation(APlayerController* PlayerController) {
	if (!IsValid(PlaceablePropertiesClass)) {
		return nullptr;
	}

	auto* PropertiesWidget = CreateWidget<UUIUrlPropertiesWidget>(PlayerController, PlaceablePropertiesClass);
	if (!PropertiesWidget) {
		VeLogErrorFunc("Failed to create a properties widget instance");
		return nullptr;
	}

	if (!IsValid(PlaceableComponent)) {
		return nullptr;
	}

	FString Url;

	if (PlaceableComponent->GetMetadata().Type == Api::PlaceableTypes::Image2D) {
		if (const FVeFileMetadata* FileMetadata = PlaceableComponent->GetMetadata().Files.FindByPredicate([](const FVeFileMetadata& InFileMetadata) {
			return InFileMetadata.Type == EApiFileType::TextureDiffuse;
		})) {
			Url = FileMetadata->Url;
		} else {
			FileMetadata = PlaceableComponent->GetMetadata().Files.FindByPredicate([](const FVeFileMetadata& InFileMetadata) {
				return InFileMetadata.Type == EApiFileType::ImageFull;
			});

			if (FileMetadata) {
				Url = FileMetadata->Url;
			} else {
				FileMetadata = PlaceableComponent->GetMetadata().Files.FindByPredicate([](const FVeFileMetadata& InFileMetadata) {
					return InFileMetadata.Type == EApiFileType::ImagePreview;
				});

				if (FileMetadata) {
					Url = FileMetadata->Url;
				} else {
					LogWarningF("failed to get url");
				}
			}
		}
	} else if (PlaceableComponent->GetMetadata().Type == Api::PlaceableTypes::Video2D) {
		if (const FVeFileMetadata* FileMetadata = PlaceableComponent->GetMetadata().Files.FindByPredicate([](const FVeFileMetadata& InFileMetadata) {
			return InFileMetadata.Type == EApiFileType::Video;
		})) {
			Url = FileMetadata->Url;
		}
	}

	PropertiesWidget->SetUrl(Url);

	PropertiesWidget->GetOnSave().BindWeakLambda(this, [=]() {
		OnSaved(PropertiesWidget->GetUrl());
	});

	return PropertiesWidget;
}
