// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "PlaceableLib.h"

#include "VeShared.h"
#include "VeGameFramework.h"
#include "PlaceableComponent.h"


AActor* UPlaceableLib::SpawnPlaceable(const UObject* WorldContextObject, const FVePlaceableMetadata& InVeMetadata, UClass* DefaultClass) {
	if (IsValid(WorldContextObject)) {
		if (auto* World = WorldContextObject->GetWorld()) {
			UClass* UsedPlaceableClass = IsValid(InVeMetadata.Class) ? InVeMetadata.Class.Get() : DefaultClass;
			if (IsValid(UsedPlaceableClass)) {
				FTransform Transform(InVeMetadata.Position);
				Transform.SetRotation(FQuat::MakeFromRotator(InVeMetadata.Rotation));
				Transform.SetScale3D(InVeMetadata.Scale);

				FActorSpawnParameters Parameters;
				Parameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

				auto* PlaceableActor = World->SpawnActor<AActor>(UsedPlaceableClass, Transform, Parameters);
				if (PlaceableActor) {
					// UpdatePlaceableMetadata(PlaceableActor, InVeMetadata);
					return PlaceableActor;
				} else {
					VeLogError("[GameMode] failed to spawn placeable of class {%s}", *DefaultClass->GetName());
				}
			}
		}
	}
	return nullptr;
}

FString UPlaceableLib::GetFileMimeType(const FString& FilePath) {
	const FString FileExtension = FPaths::GetExtension(FilePath, true);

	static TMap<FString, FString> ExtensionMimeTypeMap;
	if (ExtensionMimeTypeMap.Num() == 0) {
		// Web
		ExtensionMimeTypeMap.Add(TEXT(".html"), TEXT("text/html"));
		ExtensionMimeTypeMap.Add(TEXT(".css"), TEXT("text/css"));
		ExtensionMimeTypeMap.Add(TEXT(".js"), TEXT("application/x-javascript"));

		// Video
		ExtensionMimeTypeMap.Add(TEXT(".avi"), TEXT("video/msvideo, video/avi, video/x-msvideo"));
		ExtensionMimeTypeMap.Add(TEXT(".mpeg"), TEXT("video/mpeg"));
		ExtensionMimeTypeMap.Add(TEXT(".mp4"), TEXT("video/mp4"));

		// Image
		ExtensionMimeTypeMap.Add(TEXT(".bmp"), TEXT("image/bmp"));
		ExtensionMimeTypeMap.Add(TEXT(".gif"), TEXT("image/gif"));
		ExtensionMimeTypeMap.Add(TEXT(".jpg"), TEXT("image/jpeg"));
		ExtensionMimeTypeMap.Add(TEXT(".jpeg"), TEXT("image/jpeg"));
		ExtensionMimeTypeMap.Add(TEXT(".png"), TEXT("image/png"));
		ExtensionMimeTypeMap.Add(TEXT(".svg"), TEXT("image/svg+xml"));
		ExtensionMimeTypeMap.Add(TEXT(".tiff"), TEXT("image/tiff"));

		// Audio
		ExtensionMimeTypeMap.Add(TEXT(".midi"), TEXT("audio/x-midi"));
		ExtensionMimeTypeMap.Add(TEXT(".mp3"), TEXT("audio/mpeg"));
		ExtensionMimeTypeMap.Add(TEXT(".ogg"), TEXT("audio/vorbis, application/ogg"));
		ExtensionMimeTypeMap.Add(TEXT(".wav"), TEXT("audio/wav, audio/x-wav"));

		// Documents
		ExtensionMimeTypeMap.Add(TEXT(".xml"), TEXT("application/xml"));
		ExtensionMimeTypeMap.Add(TEXT(".txt"), TEXT("text/plain"));
		ExtensionMimeTypeMap.Add(TEXT(".tsv"), TEXT("text/tab-separated-values"));
		ExtensionMimeTypeMap.Add(TEXT(".csv"), TEXT("text/csv"));
		ExtensionMimeTypeMap.Add(TEXT(".json"), TEXT("application/json"));

		// Compressed
		ExtensionMimeTypeMap.Add(TEXT(".zip"), TEXT("application/zip, application/x-compressed-zip"));

		//Model
		ExtensionMimeTypeMap.Add(TEXT(".glb"), TEXT("model/gltf-binary"));

	}

	if (FString* FoundMimeType = ExtensionMimeTypeMap.Find(FileExtension)) {
		return *FoundMimeType;
	}

	return TEXT("application/unknown");
}

UPlaceableComponent* UPlaceableLib::GetPlaceableComponent(const AActor* PlaceableActor) {
	if (IsValid(PlaceableActor)) {
		auto* PlaceableComponent = PlaceableActor->FindComponentByClass<UPlaceableComponent>();
		if (IsValid(PlaceableComponent)) {
			return PlaceableComponent;
		}
	}
	return nullptr;
}

USceneComponent* UPlaceableLib::CreateRuntimeComponent(USceneComponent* Outer, const TSubclassOf<USceneComponent> Class, const FName Name, const EComponentCreationMethod& CreationMethod) {
	auto* NewComponent = NewObject<USceneComponent>(Outer, Class, Name, RF_Transient | RF_Transactional);
	NewComponent->CreationMethod = CreationMethod;

	if (!NewComponent->GetAttachParent() && !NewComponent->IsAttachedTo(Outer)) {
		if (const auto* Owner = Outer->GetOwner(); !Owner || !Owner->GetWorld()) {
			if (auto* World = Outer->GetWorld()) {
				NewComponent->RegisterComponentWithWorld(World);
				NewComponent->AttachToComponent(Outer, FAttachmentTransformRules::KeepRelativeTransform);
			} else {
				NewComponent->SetupAttachment(Outer);
			}
		} else {
			NewComponent->AttachToComponent(Outer, FAttachmentTransformRules::KeepRelativeTransform);
			NewComponent->RegisterComponent();
		}
	}

	return NewComponent;
}

void UPlaceableLib::DestroyRuntimeComponent(USceneComponent* Outer, USceneComponent* RuntimeComponent) {
	if (RuntimeComponent && !RuntimeComponent->IsBeingDestroyed()) {
		if (Outer->GetOwner() && Outer->GetOwner()->GetWorld()) {
			RuntimeComponent->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
			RuntimeComponent->UnregisterComponent();
		}
		RuntimeComponent->DestroyComponent();
		RuntimeComponent = nullptr;
	}
}
