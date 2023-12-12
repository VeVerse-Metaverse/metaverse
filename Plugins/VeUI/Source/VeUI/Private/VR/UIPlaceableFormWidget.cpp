// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.
#include "VR/UIPlaceableFormWidget.h"

#include "VeOpenSea/VeOpenSeaModule.h"
#include "OpenSeaAssetRequest.h"
#include "UIObjectListPageWidget.h"
#include "VeApi.h"
#include "VeDownload.h"
#include "Components/UIButtonWidget.h"
#include "UIOpenSeaListPageWidget.h"
#include "VR/UIPlaceableUrlWidget.h"
#include "UIPageManagerWidget.h"
#include "UIPlayerController.h"
#include "VR/UIPlaceableFilePageWidget.h"
#include "FileHeaderRequest.h"
#include "DownloadChunkSubsystem.h"

#define LOCTEXT_NAMESPACE "VeUI"


void UUIPlaceableFormWidget::NativeOnInitialized() {
	Super::NativeOnInitialized();

	// Create child widgets for tabs.
	ObjectListWidget = CreateWidget<UUIObjectListPageWidget>(ObjectListWidgetClass);
	OpenSeaAssetListWidget = CreateWidget<UUIOpenSeaListPageWidget>(OpenSeaAssetListWidgetClass);
	PlaceableUrlWidget = CreateWidget<UUIPlaceableUrlWidget>(PlaceableUrlWidgetClass);
	PlaceableClassWidget = CreateWidget<UUIPlaceableClassWidget>(PlaceableClassWidgetClass);
	PlaceableFilePage = CreateWidget<UUIPlaceableFilePageWidget>(PlaceableFilePageClass);

	// SwitchToUIMode()
	auto* PC = Cast<AUIPlayerController>(GetOwningPlayer());
	if (IsValid(PC)) {
		PC->SwitchToUIMode();
	}

	// Show the OpenSea list widget by default.
	if (IsValid(PageManagerWidget)) {
		PageManagerWidget->ShowPage(OpenSeaAssetListWidget);

		// Refresh the open sea list widget.
		if (IsValid(OpenSeaAssetListWidget)) {
			OpenSeaAssetListWidget->Refresh();
		}
	}

	if (CloseButtonWidget) {
		// Called when the form is closed.
		CloseButtonWidget->GetOnButtonClicked().AddWeakLambda(this, [this]() {
			Close();
		});
	}

#pragma region Artheon Object Bindings

	ObjectListWidget->OnObjectSelected.AddWeakLambda(this, [=](const TSharedPtr<FApiObjectMetadata> InMetadata) {
		// Broadcast new Object Id
		FPlaceableAssetMetadata RequestMetadata;
		RequestMetadata.Id = InMetadata->Id;
		const auto* File = InMetadata->Files.FindByPredicate([](const FApiFileMetadata& InFile) {
			return InFile.Type == EApiFileType::ImageFull;
		});

		if (File) {
			RequestMetadata.Url = File->Url;
		}

		SelectAssetAndClose(RequestMetadata);
	});

#pragma endregion Artheon Object Bindings

#pragma region OpenSea Bindings

	// if (IsValid(OpenSeaAssetListWidget)) {
	// 	OpenSeaAssetListWidget->OnObjectSelected.AddWeakLambda(this, [this](const FOpenSeaAssetMetadata& InMetadata) {
	// 		GET_MODULE_SUBSYSTEM(ChunkDownloadSubsystem, Download, Chunk);
	// 		if (ChunkDownloadSubsystem) {
	// 			const auto CallbackPtr = MakeShared<FOnFileHeaderRequestComplete>();
	// 			CallbackPtr->BindWeakLambda(this, [=](const FFileHeaderResult& InResult, const bool bInSuccessful, const FString& InError) {
	// 				if (bInSuccessful) {
	// 					FPlaceableAssetMetadata RequestMetadata;
	// 					RequestMetadata.MimeType = InResult.GetContentMimeType();
	// 					RequestMetadata.Url = InResult.Url;
	// 					RequestMetadata.MediaType = GetMediaType(InResult.GetContentMimeType(), InResult.Url);
	//
	// 					SelectAssetAndClose(RequestMetadata);
	// 				} else {
	// 					LogErrorF("failed to load an nft asset: %s", *InError);
	// 				}
	// 			});
	// 			ChunkDownloadSubsystem->GetFileHeaders(InMetadata.GetImageUrl(), CallbackPtr);
	// 		}
	// 	});
	// }

#pragma endregion OpenSea Bindings

#pragma region Url Widget Bindings

	if (IsValid(PlaceableUrlWidget)) {
		PlaceableUrlWidget->OnUrlChanged.AddWeakLambda(this, [this](const FString& Url) {
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

							SelectAssetAndClose(RequestMetadata);
						} else {
							LogErrorF("failed to load nft asset: %s", *InError);
						}
					});
					ChunkDownloadSubsystem->GetFileHeaders(Url, CallbackPtr);
				} else {
					TArray<FString> Tokens;
					Url.ParseIntoArray(Tokens,TEXT("/"), true);
					if (Tokens.Num() > 2) {
						const int32 ContractAddressIndex = Tokens.Num() - 2;
						const int32 TokenIdIndex = Tokens.Num() - 1;
						const auto ContractAddress = Tokens[ContractAddressIndex];
						if (ContractAddress.StartsWith(TEXT("0x"))) {
							const auto TokenId = Tokens[TokenIdIndex];

							GET_MODULE_SUBSYSTEM(OpenSeaAssetSubsystem, OpenSea, Asset);
							if (OpenSeaAssetSubsystem) {
								// 1. Create a callback.
								const auto OpenSeaAssetCallbackPtr = MakeShared<FOnOpenSeaAssetRequestCompleted>();

								// 2. Bind callback.
								OpenSeaAssetCallbackPtr->BindWeakLambda(this, [=](const FOpenSeaAssetMetadata& InMetadata, const bool bInSuccessful, const FString& InError) {
									if (!bInSuccessful) {
										LogErrorF("failed to request: %s", *InError);
									}

									CallbackPtr->BindWeakLambda(this, [=](const FFileHeaderResult& InResult, const bool bInSuccessful2, const FString& InError2) {
										if (bInSuccessful2) {
											FPlaceableAssetMetadata RequestMetadata;
											RequestMetadata.MimeType = InResult.GetContentMimeType();
											RequestMetadata.Url = InResult.Url;
											RequestMetadata.MediaType = GetMediaType(InResult.GetContentMimeType(), InResult.Url);

											SelectAssetAndClose(RequestMetadata);
										} else {
											LogErrorF("failed to load an nft asset: %s", *InError2);
										}
									});
									ChunkDownloadSubsystem->GetFileHeaders(InMetadata.GetImageUrl(), CallbackPtr);
									// FString MediaType = GetMediaType(InMetadata.MimeType);
									//
									// FPlaceableAssetMetadata RequestMetadata;
									// RequestMetadata.MimeType = InMetadata.MimeType;
									// if (InMetadata.ImageOriginalUrl.IsEmpty()) {
									// 	RequestMetadata.Url = InMetadata.ImageUrl;
									// } else {
									// 	RequestMetadata.Url = InMetadata.ImageOriginalUrl;
									// }
									// RequestMetadata.MediaType = MediaType;
									//
									// SelectAssetAndClose(RequestMetadata);
								});

								// 3. Make the request using the callback.
								OpenSeaAssetSubsystem->GetAsset(ContractAddress, TokenId, OpenSeaAssetCallbackPtr);
							}
						}
					}
				}
			}
		});
	} else {
		LogErrorF("placeable browser widget has no valid open sea nft list widget");
	}

#pragma endregion Url Widget Bindings

#pragma region Class Widget Bindings

	if (IsValid(PlaceableClassWidget)) {
		PlaceableClassWidget->OnClassSelected.AddWeakLambda(this, [=](UClass* InClass) {
			FPlaceableAssetMetadata Metadata;
			Metadata.MimeType = Api::ContentTypes::UnrealBlueprint;
			Metadata.MediaType = InClass->GetPathName();
			SelectAssetAndClose(Metadata);
		});
	}

#pragma endregion

#pragma region PlaceableFile

	if (IsValid(PlaceableFilePage)) {
		PlaceableFilePage->OnFileChanged.AddWeakLambda(this, [=](const FString& FilePath, const FString& FileType) {
			FPlaceableFileMetadata Metadata;
			Metadata.FilePath = FilePath;
			Metadata.FileType = FileType;

			if (FileType == Api::FileTypes::ImageFullInitial) {
				Metadata.MediaType = TEXT("image_2d");
			} else if (FileType == Api::FileTypes::Pdf) {
				Metadata.MediaType = TEXT("pdf");
			} else if (FileType == Api::FileTypes::Model) {
				Metadata.MediaType = TEXT("mesh_3d");
			}

			if (!Metadata.MediaType.IsEmpty()) {
				SelectFileAndClose(Metadata);
			}
		});
	}

#pragma endregion PlaceableFile

	if (ObjectButtonWidget) {
		ObjectButtonWidget->GetOnButtonClicked().AddUObject(this, &UUIPlaceableFormWidget::SwitchToObjectForm);
	}

	if (OpenSeaAssetButtonWidget) {
		OpenSeaAssetButtonWidget->GetOnButtonClicked().AddUObject(this, &UUIPlaceableFormWidget::SwitchToOpenSeaForm);
	}

	if (PlaceableUrlButtonWidget) {
		PlaceableUrlButtonWidget->GetOnButtonClicked().AddUObject(this, &UUIPlaceableFormWidget::SwitchToUrlForm);
	}

	if (PlaceableClassButtonWidget) {
		PlaceableClassButtonWidget->GetOnButtonClicked().AddUObject(this, &UUIPlaceableFormWidget::SwitchToClassForm);
	}

	if (PlaceableFileButtonWidget) {
		PlaceableFileButtonWidget->GetOnButtonClicked().AddUObject(this, &UUIPlaceableFormWidget::SwitchToPlaceableFileForm);
	}
}

void UUIPlaceableFormWidget::Close() {
	OnClosedDelegate.Broadcast();

	//SwitchToGameMode();
	auto* PC = Cast<AUIPlayerController>(GetOwningPlayer());
	if (IsValid(PC)) {
		PC->SwitchToGameMode();
	}

	RemoveFromParent();
}

void UUIPlaceableFormWidget::SelectAssetAndClose(const FPlaceableAssetMetadata& InMetadata) {
	OnPlaceableFormAssetSelected.Broadcast(InMetadata);
	Close();
}

void UUIPlaceableFormWidget::SelectFileAndClose(const FPlaceableFileMetadata& InMetadata) {
	OnPlaceableFormFileSelected.Broadcast(InMetadata);
	Close();
}

FString UUIPlaceableFormWidget::GetMediaType(const FString& MimeType, const FString& Url) {
	FString MediaType;
	if (MimeType.StartsWith(TEXT("image/"))) {
		MediaType = TEXT("image_2d");
	} else if (MimeType.EndsWith(TEXT("mp4"))) {
		MediaType = TEXT("video_2d");
	} else if (MimeType.StartsWith(TEXT("model/")) && MimeType.Contains(TEXT("gltf"))) {
		MediaType = TEXT("mesh_3d");
	} else if (MimeType.StartsWith(TEXT("audio/"))) {
		MediaType = TEXT("audio");
	} else if (MimeType == TEXT("application/x-pdf") || MimeType == TEXT("application/pdf")) {
		MediaType = TEXT("pdf");
	} else if (Url.StartsWith(TEXT("http")) && (Url.Contains(TEXT("youtube.com")) || Url.Contains(TEXT("youtu.be")))) {
		MediaType = TEXT("youtube");
	} else {
		MediaType = TEXT("unknown");
	}
	return MediaType;
}

void UUIPlaceableFormWidget::SwitchToObjectForm() {
	if (IsValid(ObjectListWidget)) {
		//OpenPage(ObjectListWidgetClass);
		if (PageManagerWidget->GetCurrentPage() != ObjectListWidget) {
			ObjectListWidget->Refresh();
			PageManagerWidget->ShowPage(ObjectListWidget);
		}
	}
}

void UUIPlaceableFormWidget::SwitchToOpenSeaForm() {
	if (IsValid(OpenSeaAssetListWidget)) {
		if (PageManagerWidget->GetCurrentPage() != OpenSeaAssetListWidget) {
			PageManagerWidget->ShowPage(OpenSeaAssetListWidget);
		}
	}
}

void UUIPlaceableFormWidget::SwitchToUrlForm() {
	if (IsValid(PlaceableUrlWidget)) {
		if (PageManagerWidget->GetCurrentPage() != PlaceableUrlWidget) {
			PageManagerWidget->ShowPage(PlaceableUrlWidget);
		}
	}
}

void UUIPlaceableFormWidget::SwitchToClassForm() {
	if (IsValid(PlaceableClassWidget)) {
		if (PageManagerWidget->GetCurrentPage() != PlaceableClassWidget) {
			PageManagerWidget->ShowPage(PlaceableClassWidget);
		}
	}
}

void UUIPlaceableFormWidget::SwitchToPlaceableFileForm() {
	if (IsValid(PlaceableFilePage)) {
		PageManagerWidget->ShowPage(PlaceableFilePage);
	}
}


#undef LOCTEXT_NAMESPACE
