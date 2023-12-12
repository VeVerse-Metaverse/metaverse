// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.

#include "VR/UIPlaceableFilePageWidget.h"

#include "VeUI.h"
#include "Components/UIButtonWidget.h"
#include "Components/UIFormInputWidget.h"
#include "ApiFileMetadata.h"


void UUIPlaceableFilePageWidget::NativeOnInitialized() {
	Super::NativeOnInitialized();

	if (CloseButtonWidget) {
		CloseButtonWidget->GetOnButtonClicked().AddWeakLambda(this, [this]() {
			OnClosedDelegate.Broadcast();
		});
	}

	if (SubmitButtonWidget) {
		SubmitButtonWidget->GetOnButtonClicked().AddUObject(this, &UUIPlaceableFilePageWidget::SubmitButton_OnClicked);
		SubmitButtonWidget->SetIsEnabled(false);
	}

	if (OpenFileButtonWidget) {
		OpenFileButtonWidget->GetOnButtonClicked().AddUObject(this, &UUIPlaceableFilePageWidget::OpenFileButton_OnClicked);
	}
}

void UUIPlaceableFilePageWidget::SubmitButton_OnClicked() {
	OnClosedDelegate.Broadcast();
	OnFileChanged.Broadcast(FilePath, FileType);
}

#if !UE_SERVER
#if PLATFORM_DESKTOP
#include "VePlatform.h"
#include "IVePlatform.h"
#endif
#endif

void UUIPlaceableFilePageWidget::OpenFileButton_OnClicked() {
#if !UE_SERVER
#if PLATFORM_DESKTOP
	TArray<FString> ImageExtensions = {TEXT("jpg"), TEXT("jpeg"), TEXT("png")};
	TArray<FString> PdfExtensions = {TEXT("pdf")};
	TArray<FString> GlbExtensions = {TEXT("glb")};

	TArray<FString> AllExtensions;
	AllExtensions.Reserve(ImageExtensions.Num() + PdfExtensions.Num());
	AllExtensions.Append(ImageExtensions);
	AllExtensions.Append(PdfExtensions);
	AllExtensions.Append(GlbExtensions);

	FilePath = TEXT("");
	FileType = TEXT("");

	const FString DialogTitle(TEXT("Choose File to Upload"));
	const FString DefaultPath(TEXT(""));
	const FString FileTypes(
		TEXT("All Files|*.") + FString::Join(AllExtensions, TEXT(";*.")) +
		TEXT("|Images|*.") + FString::Join(ImageExtensions, TEXT(";*.")) +
		TEXT("|PDF Files|*.") + FString::Join(PdfExtensions, TEXT(";*.")) +
		TEXT("|GLB Meshes|*.") + FString::Join(GlbExtensions, TEXT(";*."))
		);
	TArray<FString> OutFileNames;

	void* ParentWindowPtr = FSlateApplication::Get().GetActiveTopLevelWindow()->GetNativeWindow()->GetOSWindowHandle();
	IVePlatform* VePlatform = FVePlatformModule::Get()->GetPlatform();
	if (VePlatform) {
		uint32 SelectionFlag = EVeFileDialogFlags::None; //A value of 0 represents single file selection while a value of 1 represents multiple file selection
		VePlatform->OpenFileDialog(ParentWindowPtr, DialogTitle, DefaultPath, FString(""), FileTypes, SelectionFlag, OutFileNames);

		if (OutFileNames.Num()) {
			FString FileExtension = FPaths::GetExtension(OutFileNames[0]);
			if (!AllExtensions.Contains(FileExtension)) {
				VeLogError("failed file extension");
				SubmitButtonWidget->SetIsEnabled(false);
				return;
			}

			FilePath = FPaths::ConvertRelativePathToFull(OutFileNames[0]);
			if (FileInputWidget) {
				FileInputWidget->SetText(FText::FromString(FilePath));
			}

			if (ImageExtensions.Contains(FileExtension)) {
				FileType = Api::FileTypes::ImageFullInitial;
			} else if (PdfExtensions.Contains(FileExtension)) {
				FileType = Api::FileTypes::Pdf;
			} else if (GlbExtensions.Contains(FileExtension)) {
				FileType = Api::FileTypes::Model;
			}

			SubmitButtonWidget->SetIsEnabled(true);
		}
	}
#endif
#endif
}
