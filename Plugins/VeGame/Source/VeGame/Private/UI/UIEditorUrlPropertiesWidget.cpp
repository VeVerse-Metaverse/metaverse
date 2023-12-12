// Authors: Egor A. Pristavka, Nikolay Bulatov. 
// Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "UI/UIEditorUrlPropertiesWidget.h"

#include "VeShared.h"
#include "VeGameModule.h"
#include "Components/Image.h"
#include "AnimatedTexture2D.h"
#if PLATFORM_DESKTOP
#include "VePlatform.h"
#include "IVePlatform.h"
#endif

void UUIEditorUrlPropertiesWidget::NativeOnTextureLoaded(const FTextureLoadResult& InResult) {
	if (!InResult.bSuccessful || !InResult.Texture.IsValid()) {
		return;
	}

	// SetTexture(InResult.Texture);
	ASSIGN_WIDGET_TEXTURE(PreviewImageWidget, InResult);
}

void UUIEditorUrlPropertiesWidget::NativeOnInitialized() {
	// Listen for URL commit events.
	if (IsValid(UrlInputWidget)) {
		if (!UrlInputWidget->OnTextCommitted.IsBound()) {
			UrlInputWidget->OnTextCommitted.AddWeakLambda(this, [this](const FText& InText, const ETextCommit::Type InCommitMethod) {
				OnUrlTextCommittedCallback(InText, InCommitMethod);
			});
		}
	}

	// Listen for file browse button clicks.
	if (IsValid(FileBrowserButtonWidget)) {
		FileBrowserButtonWidget->GetOnButtonClicked().AddUObject(this, &UUIEditorUrlPropertiesWidget::OnFileBrowserButtonClicked);
	}

	Super::NativeOnInitialized();
}

void UUIEditorUrlPropertiesWidget::OnUrlTextCommittedCallback(const FText& InText, ETextCommit::Type /*InCommitMethod*/) {
	if (!InText.IsEmpty()) {
		Url = InText.ToString();
		RequestTexture(this, Url, TEXTUREGROUP_UI);
		SetSaveEnabled(!InText.IsEmpty());
	}
}

void UUIEditorUrlPropertiesWidget::OnFileBrowserButtonClicked() {
#if PLATFORM_DESKTOP
	const FString DialogTitle(TEXT("Browse..."));
	const FString DefaultPath(TEXT(""));

	const void* ParentWindowPtr = FSlateApplication::Get().GetActiveTopLevelWindow()->GetNativeWindow()->GetOSWindowHandle();
	if (IVePlatform* VePlatform = FVePlatformModule::Get()->GetPlatform()) {
		TArray<FString> OutFileNames;
		constexpr uint32 SelectionFlags = EVeFileDialogFlags::None;
		VePlatform->OpenFileDialog(ParentWindowPtr, DialogTitle, DefaultPath, FString(""), FileTypes, SelectionFlags, OutFileNames);
		if (OutFileNames.Num()) {
			FString FilePath = FPaths::ConvertRelativePathToFull(OutFileNames[0]);

			SetSaveEnabled(!FilePath.IsEmpty());
			if (FilePath.IsEmpty()) return;

			FilePath = FString::Printf(TEXT("file:///%s"), *FilePath);

			SetUrl(FilePath);
		}
	} else {
		VeLogErrorFunc("failed to access desktop platform");
	}
#endif
}

void UUIEditorUrlPropertiesWidget::SetUrl(const FString& InUrl) {
	Url = InUrl;
	UrlInputWidget->SetText(FText::FromString(Url));

	SetSaveEnabled(!Url.IsEmpty());
	if (!Url.IsEmpty()) {
		RequestTexture(this, Url, TEXTUREGROUP_UI);
	}
}

FString UUIEditorUrlPropertiesWidget::GetUrl() {
	return Url;
}
