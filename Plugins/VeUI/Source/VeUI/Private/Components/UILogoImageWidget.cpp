#include "Components/UILogoImageWidget.h"

#include "GameFrameworkAppSubsystem.h"
#include "VeGameFramework.h"
#include "Components/UIImageWidget.h"


void UUILogoImageWidget::NativeConstruct() {
	UUIWidgetBase::NativeConstruct();

	ShowImage();
}

void UUILogoImageWidget::ShowImage() {

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
		if (IsValid(ImageWidget) && !Url.IsEmpty()) {
			ImageWidget->ShowImage(Url);
		}
	});
	
	if (!AppSubsystem->GetAppLogo().IsEmpty()) {
		ImageWidget->ShowImage(AppSubsystem->GetAppLogo());
	}
	
}