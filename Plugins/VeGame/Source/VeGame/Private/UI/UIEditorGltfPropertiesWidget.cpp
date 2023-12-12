// Authors: Egor A. Pristavka, Nikolay Bulatov. 
// Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "UI/UIEditorGltfPropertiesWidget.h"

#if PLATFORM_DESKTOP
#include "VePlatform.h"
#include "IVePlatform.h"
#endif
#include "VeShared.h"
#include "VeGameModule.h"
#include "Components/Image.h"
#include "AnimatedTexture2D.h"

void UUIEditorGltfPropertiesWidget::NativeOnTextureLoaded(const FTextureLoadResult& InResult) {
	if (!InResult.bSuccessful || !InResult.Texture.IsValid()) {
		return;
	}

	// SetTexture(InResult.Texture);
	ASSIGN_WIDGET_TEXTURE(PreviewImageWidget, InResult);
}

void UUIEditorGltfPropertiesWidget::NativeOnInitialized() {
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
		FileBrowserButtonWidget->GetOnButtonClicked().AddUObject(this, &UUIEditorGltfPropertiesWidget::OnFileBrowserButtonClicked);
	}

	Super::NativeOnInitialized();
}

void UUIEditorGltfPropertiesWidget::OnUrlTextCommittedCallback(const FText& InText, ETextCommit::Type /*InCommitMethod*/) {
	if (!InText.IsEmpty()) {
		Url = InText.ToString();
		RequestTexture(this, Url, TEXTUREGROUP_UI);
		SetSaveEnabled(!InText.IsEmpty());
	}
}

void UUIEditorGltfPropertiesWidget::OnFileBrowserButtonClicked() {
#if PLATFORM_DESKTOP
	const FString DialogTitle(TEXT("Open Model File"));
	const FString DefaultPath(TEXT(""));
	const FString FileTypes(TEXT("Models (*.gltf;*.glb)|*.gltf;*.glb|Archive files (*.zip;*.gz)|*.zip;*.gz|All files (*.*)|*.*"));

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

void UUIEditorGltfPropertiesWidget::SetUrl(const FString& InUrl) {
	Url = InUrl;
	if (IsValid(UrlInputWidget)) {
		UrlInputWidget->SetText(FText::FromString(Url));
	}

	SetSaveEnabled(!Url.IsEmpty());
	if (!Url.IsEmpty()) {
		// RequestTexture(this, Url, TEXTUREGROUP_UI);
	}
}

FString UUIEditorGltfPropertiesWidget::GetUrl() {
	return Url;
}
