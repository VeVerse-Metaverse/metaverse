// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.

#include "Components/UILoadingWidget.h"

#include "AnimatedTexture2D.h"
#include "GameFrameworkAppSubsystem.h"
#include "VeGameFramework.h"
#include "Components/UIImageWidget.h"


void UUILoadingWidget::NativeOnTextureLoaded(const FTextureLoadResult& InResult) {
	if (!InResult.bSuccessful || !InResult.Texture.IsValid()) {
		return;
	}

	ASSIGN_WIDGET_TEXTURE(PreviewImageWidget, InResult);
}

void UUILoadingWidget::SetTexture(UTexture2D* Texture) {
	PreviewImageWidget->SetBrushFromTexture(Texture);

	const FVector2D Size = {
		static_cast<float>(Texture->GetSizeX()),
		static_cast<float>(Texture->GetSizeY())
	};
	
	PreviewImageWidget->SetDesiredSizeOverride(Size);
}

void UUILoadingWidget::NativeConstruct() {
	Super::NativeConstruct();
	if (LoadingAnimation) {
		PlayAnimation(LoadingAnimation,  0.0f ,0);
	}

	GET_MODULE_SUBSYSTEM(AppSubsystem, GameFramework, App);
	if (!AppSubsystem) {
		LogErrorF("failed to get app subsystem");
		return;
	}
	
	const auto Callback = MakeShared<FOnGetAppImageFileUrlCompleted>();
	if (!AppSubsystem->GetAppLogoImageFileUrl(Callback)) {
		LogErrorF("failed to get app logo file id");
		return;
	}

	Callback->BindWeakLambda(this, [=](const FString& Url) {
		if (IsValid(PreviewImageWidget) && !Url.IsEmpty()) {
			ImageUrl = Url;
		}
	});
	
	if (!AppSubsystem->GetAppPlaceholderLogo().IsEmpty()) {
		ImageUrl = AppSubsystem->GetAppPlaceholderLogo();
	}
	
	{
		if (PreviewImageWidget && !ImageUrl.IsEmpty()) {
			RequestTexture(this, ImageUrl, TEXTUREGROUP_UI);
		} 
	}
}
