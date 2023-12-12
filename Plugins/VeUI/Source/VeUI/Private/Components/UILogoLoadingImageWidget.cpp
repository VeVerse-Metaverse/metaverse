#include "Components/UILogoLoadingImageWidget.h"

#include "GameFrameworkAppSubsystem.h"
#include "VeGameFramework.h"
#include "Components/UIImageWidget.h"

void UUILogoLoadingImageWidget::NativeConstruct() {
	UUIWidgetBase::NativeConstruct();

	GET_MODULE_SUBSYSTEM(AppSubsystem, GameFramework, App);
	if (!AppSubsystem) {
		LogErrorF("failed to get app subsystem");
		return;
	}

	const auto Callback = MakeShared<FOnGetAppImageFileUrlCompleted>();
	if (!AppSubsystem->GetAppPlaceholderImageFileUrl(Callback)) {
		LogErrorF("failed to get app logo file id");
		return;
	}

	Callback->BindWeakLambda(this, [=](const FString& Url) {
		ImageWidget->ShowImage(Url);
	});
}
