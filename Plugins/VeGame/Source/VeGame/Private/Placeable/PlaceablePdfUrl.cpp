// Authors: Egor A. Pristavka, Nikolay Bulatov. 
// Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "Placeable/PlaceablePdfUrl.h"

#include "ApiFileMetadata.h"
#include "PlaceableComponent.h"
#include "VeApi.h"
#include "VeGameModule.h"
#include "VeDownload.h"
#include "VeShared.h"
#include "Components/InteractionSubjectComponent.h"
#include "Components/UIImageWidget.h"
#include "Components/WidgetComponent.h"
#include "Net/UnrealNetwork.h"
#include "Structs/PlaceableAssetMetadata.h"
#include "UI/UIEditorUrlPropertiesWidget.h"
#include "UI/UIPlaceablePropertiesWidget.h"
#include "DownloadChunkSubsystem.h"

APlaceablePdfUrl::APlaceablePdfUrl() {
	PrimaryActorTick.bCanEverTick = false;

	WidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("Widget"));
	WidgetComponent->SetupAttachment(RootComponent);

	bReplicates = true;
}

void APlaceablePdfUrl::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APlaceablePdfUrl, CurrentPageUrl);
}

void APlaceablePdfUrl::OnRep_CurrentPageUrl() {
	Client_Refresh();
}

void APlaceablePdfUrl::Client_Refresh() {
	if (!IsRunningDedicatedServer()) {
		if (auto* Widget = Cast<UUIImageWidget>(WidgetComponent->GetWidget()); IsValid(Widget)) {
			Widget->ShowImage(CurrentPageUrl);
		}
	}
}

void APlaceablePdfUrl::ReplacePlaceableFile(const FString& InUrl, const FString& InMimeType) {
	GET_MODULE_SUBSYSTEM(FileUploadSubssytem, Api, FileUpload);
	if (FileUploadSubssytem) {
		// Validate mime type.
		const auto MimeType = InMimeType;

		const auto ReplaceFileCallback = MakeShared<FOnFileRequestCompleted>();
		ReplaceFileCallback->BindWeakLambda(this, [this](const FApiFileMetadata InFileMetadata, const bool bSuccessful2, const FString& Error2) {
			if (bSuccessful2) {
				UpdatePlaceableOnServer();
			} else {
				VeLogErrorFunc("failed to replace placeable file: %s", *Error2);
			}
		});

		FApiEntityFileLinkMetadata FileMetadata;
		FileMetadata.File = InUrl;
		FileMetadata.EntityId = GetPlaceableMetadata().Id;
		FileMetadata.SetFileType(MimeType, InUrl);
		FileMetadata.MimeType = MimeType;


		if (!FileUploadSubssytem->Replace(FileMetadata, ReplaceFileCallback)) {
			VeLogErrorFunc("failed to replace placeable file");
		}
	}
}

void APlaceablePdfUrl::Authority_FirstPage() {
	if (!HasAuthority()) return;

	CurrentPage = 0;
	if (PageUrls.Num() > 0) {
		CurrentPageUrl = PageUrls[CurrentPage];
	} else {
		CurrentPageUrl.Empty();
	}

	if (!IsRunningDedicatedServer()) {
		Client_Refresh();
	}
}

void APlaceablePdfUrl::Authority_NextPage() {
	if (!HasAuthority()) return;

	if (const auto PageUrlsNum = PageUrls.Num(); PageUrlsNum > 1) {
		CurrentPage = (CurrentPage + 1) % PageUrlsNum;
		CurrentPageUrl = PageUrls[CurrentPage];

		if (!IsRunningDedicatedServer()) {
			Client_Refresh();
		}
	}
}

void APlaceablePdfUrl::Authority_PreviousPage() {
	if (!HasAuthority()) return;

	if (const auto PageUrlsNum = PageUrls.Num(); PageUrlsNum > 1) {
		CurrentPage = (CurrentPage - 1 + PageUrlsNum) % PageUrlsNum;
		CurrentPageUrl = PageUrls[CurrentPage];

		if (!IsRunningDedicatedServer()) {
			Client_Refresh();
		}
	}
}

void APlaceablePdfUrl::ClientInputBegin_Implementation(UInteractionSubjectComponent* Subject, UInteractionObjectComponent* Object, const FName& Name, const FVector& Origin, const FVector& Location, const FVector& Normal) {
	if (Name == FName(EKeys::LeftMouseButton.GetDisplayName().ToString())) {
		Subject->RemoteServer_Event_Binary_BP(Object, FName(TEXT("PdfPage-")), {});
		return;
	}

	if (Name == FName(EKeys::RightMouseButton.GetDisplayName().ToString())) {
		Subject->RemoteServer_Event_Binary_BP(Object, FName(TEXT("PdfPage+")), {});
		return;
	}

	if (Name == FName(EKeys::R.GetDisplayName().ToString())) {
		Subject->RemoteServer_Event_Binary_BP(Object, FName(TEXT("PdfPage0")), {});
		return;
	}

	// Super::ClientInputBegin_Implementation(Subject, Object, Name, Origin, Location, Normal);
}

void APlaceablePdfUrl::ServerEvent_Implementation(UInteractionSubjectComponent* Subject, UInteractionObjectComponent* Object, const FName& Name, const TArray<uint8>& Args) {
	if (Name == FName(TEXT("PdfPage-"))) {
		Authority_PreviousPage();
		return;
	}

	if (Name == FName(TEXT("PdfPage+"))) {
		Authority_NextPage();
		return;
	}

	if (Name == FName(TEXT("PdfPage0"))) {
		Authority_FirstPage();
		return;
	}

	// Super::ServerEvent_Implementation(Subject, Object, Name, Args);
}

void APlaceablePdfUrl::OnSave(const FString& Url) {
	GET_MODULE_SUBSYSTEM(ChunkDownloadSubsystem, Download, Chunk);
	if (ChunkDownloadSubsystem) {
		const auto CallbackPtr = MakeShared<FOnFileHeaderRequestComplete>();
		CallbackPtr->BindWeakLambda(this, [=](const FFileHeaderResult& Result, const bool bSuccessful, const FString& Error) {
			if (bSuccessful) {
				FPlaceableAssetMetadata RequestMetadata;
				RequestMetadata.Url = Url;
				RequestMetadata.MimeType = Result.GetContentMimeType();
				RequestMetadata.MediaType = GetMediaType(Result.GetContentMimeType(), Url);

				ReplacePlaceableFile(RequestMetadata.Url, RequestMetadata.MimeType);
			}
		});
		ChunkDownloadSubsystem->GetFileHeaders(Url, CallbackPtr);
	}
}

UUIPlaceablePropertiesWidget* APlaceablePdfUrl::GetPlaceablePropertiesWidget_Implementation(APlayerController* PlayerController) {
	if (!IsValid(PlaceablePropertiesClass)) {
		return nullptr;
	}

	auto* PropertiesWidget = CreateWidget<UUIEditorUrlPropertiesWidget>(PlayerController, PlaceablePropertiesClass);
	if (!PropertiesWidget) {
		VeLogErrorFunc("Failed to create a properties widget instance");
		return nullptr;
	}

	const auto& Metadata = PlaceableComponent->GetMetadata();

	if (const auto* File = Metadata.Files.FindByPredicate([](const FVeFileMetadata& Item) {
		return Item.Type == EApiFileType::Pdf;
	})) {
		PropertiesWidget->SetUrl(File->Url);
	}

	PropertiesWidget->GetOnSave().BindWeakLambda(this, [=] {
		OnSave(PropertiesWidget->GetUrl());
	});

	return PropertiesWidget;
}
