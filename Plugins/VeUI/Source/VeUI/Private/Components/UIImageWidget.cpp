#include "Components/UIImageWidget.h"

#include "Components/Image.h"
#include "Components/Button.h"
#include "GameFrameworkAppSubsystem.h"
#include "VeGameFramework.h"
#include "Components/UILoadingWidget.h"

void UUIImageWidget::NativeOnInitializedShared() {
	Super::NativeOnInitializedShared();

	FallbackTextureRequester = MakeShared<FManagedTextureRequester>();

	if (LoadingWidget) {
		LoadingWidget->Hide();
	}
}

void UUIImageWidget::NativeConstruct() {
	Super::NativeConstruct();
	if (ImageWidget) {
		ImageWidget->SetDesiredSizeOverride(DesiredSize);
	}
}

void UUIImageWidget::NativeDestruct() {
	FallbackTextureRequester.Reset();

	Super::NativeDestruct();
}

void UUIImageWidget::SynchronizeProperties() {
	Super::SynchronizeProperties();

	if (CurrentURL.IsEmpty()) {
		LoadFallbackImage();
	}
}

bool UUIImageWidget::ShowImage(const FString& InURL) {
	LogF("show image: %s", *InURL);
	LoadFallbackImage();

	if (FallbackTextureRequester && !FallbackTextureRequester->IsBusy() && FallbackTextureRequester->GetResult().bResolved) {
		LogF("fallback texture is ready, pre-setting it");
		SetTexture(FallbackTextureRequester->GetResult().AsTexture2D());
	} else {
		LogF("fallback texture is not ready, not pre-setting it");
	}

	CurrentURL = InURL;

	if (!CurrentURL.IsEmpty()) {
		LogF("url is not empty, requesting texture");
		SetIsLoading(true);
		RequestTexture(this, CurrentURL, TEXTUREGROUP_UI);
		return true;
	}

	LogF("url is empty, not requesting texture");
	return false;
}

bool UUIImageWidget::ShowImage(const TArray<FApiFileMetadata>& Files, EApiFileType Type) {
	LoadFallbackImage();

	const auto* FileMetadata = Files.FindByPredicate([Type](const FApiFileMetadata& File) {
		return File.Type == Type;
	});

	if (FileMetadata) {
		if (!FileMetadata->Url.IsEmpty()) {
			CurrentURL = FileMetadata->Url;
			SetIsLoading(true);
			RequestTexture(this, CurrentURL, TEXTUREGROUP_UI);
		}
		return true;
	}

	CurrentURL.Empty();
	return false;
}

bool UUIImageWidget::ShowImage(const TArray<FVeFileMetadata>& Files, EApiFileType Type) {
	LoadFallbackImage();

	const auto* FileMetadata = Files.FindByPredicate([Type](const FVeFileMetadata& File) {
		return File.Type == Type;
	});

	if (FileMetadata) {
		if (!FileMetadata->Url.IsEmpty()) {
			CurrentURL = FileMetadata->Url;
			SetIsLoading(true);
			RequestTexture(this, CurrentURL, TEXTUREGROUP_UI);
			return true;
		}
	}

	CurrentURL.Empty();
	return false;
}

void UUIImageWidget::ShowDefaultImage() {
	SetIsLoading(false);
	ClearRequest();

	LogF("show default image");
	LoadFallbackImage();
}

void UUIImageWidget::LoadFallbackImage() {
	if (DefaultTexture) {
		SetTexture(DefaultTexture);
	}

	LogF("checking if fallback image is available");
	// Check if fallback texture requester is available.
	if (FallbackTextureRequester) {
		LogF("checking if fallback image is busy loading");
		// Check if fallback texture requester is busy loading the texture.
		if (FallbackTextureRequester->IsBusy()) {
			LogF("fallback image is busy loading, waiting for it to finish");
			// Await for the texture to be loaded.
			FallbackTextureRequester->GetOnTextureLoaded().AddWeakLambda(this, [=](const FTextureLoadResult& InResult) {
				LogF("fallback image has finished loading");
				if (InResult.bSuccessful) {
					SetTexture(InResult.AsTexture2D());
				}
			});
		} else {
			LogF("fallback image is not busy loading, checking if it has been loaded");
			// Check if fallback texture requester has loaded the texture successfully.
			if (FallbackTextureRequester->GetResult().bResolved && FallbackTextureRequester->GetResult().bSuccessful) {
				LogF("fallback image has been loaded, setting it");
				SetTexture(FallbackTextureRequester->GetResult().AsTexture2D());
			} else {
				LogF("fallback image has not been loaded, requesting it");
				// If fallback texture requester has not loaded the texture, request it.
				RequestFallbackTexture();
			}
		}
	} else {
		LogErrorF("fallback texture requester is not available");
	}
}

void UUIImageWidget::SetIsLoading(bool bInIsLoading) const {
	if (LoadingVisible && LoadingWidget) {
		if (bInIsLoading) {
			LoadingWidget->Show();
		} else {
			LoadingWidget->Hide();
		}
	}
}

void UUIImageWidget::NativeOnTextureLoaded(const FTextureLoadResult& InResult) {
	SetIsLoading(false);

	LogF("native on texture loaded");
	if (!InResult.bSuccessful || !InResult.Texture.IsValid()) {
		LoadFallbackImage();
		return;
	}

	SetTexture(InResult.Texture.Get());
}

void UUIImageWidget::SetTexture(UTexture* Texture) {
	if (!ImageWidget) {
		return;
	}

	LogF("native set texture");

	ImageWidget->SetBrushResourceObject(Texture);

	if (IsValid(Texture)) {
		DesiredSize = {
			Texture->GetSurfaceWidth(),
			Texture->GetSurfaceHeight()
		};
	}

	ImageWidget->SetDesiredSizeOverride(DesiredSize);
}

void UUIImageWidget::RequestFallbackTexture() {
	GET_MODULE_SUBSYSTEM(AppSubsystem, GameFramework, App);
	if (!AppSubsystem) {
		LogErrorF("failed to get app subsystem");
		return;
	}

	LogF("requesting fallback texture");
	const auto Callback = MakeShared<FOnGetAppImageFileUrlCompleted>();

	// Wait for app placeholder file url to be received from the APIv2 
	Callback->BindWeakLambda(this, [=](const FString& Url) {
		LogF("fallback texture url received");
		if (FallbackTextureRequester) {
			LogF("fallback texture requester is available");
			if (Url.IsEmpty()) {
				LogErrorF("failed to get app placeholder file url");
			} else {
				LogF("fallback texture url is not empty, requesting texture");
				// Bind to the texture load complete event
				FallbackTextureRequester->GetOnTextureLoaded().AddWeakLambda(this, [=](const FTextureLoadResult& Result) {
					LogF("fallback texture has finished loading");
					// If the texture was loaded successfully, set it on the image widget
					if (Result.bSuccessful && Result.Texture.IsValid()) {
						LogF("fallback texture has been loaded successfully, setting it");
						// Do not update the texture if the URL has changed (actual image has been loaded meanwhile).
						if (CurrentURL != Url && !CurrentURL.IsEmpty()) {
							LogF("fallback texture url has changed, not setting it");
							SetTexture(Result.Texture.Get());
						} else {
							LogF("fallback texture url has not changed, setting it");
						}
					} else {
						LogF("fallback texture has not been loaded successfully, not setting it");
					}
				});

				LogF("requesting fallback texture");
				FallbackTextureRequester->RequestTexture(this, Url, TEXTUREGROUP_UI);
			}
		}
	});

	LogF("getting app placeholder file url");
	if (!AppSubsystem->GetAppPlaceholderImageFileUrl(Callback)) {
		LogErrorF("failed to get app logo file id");
	}
}
