// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.

#include "UIAboutPageWidget.h"

#include "Components/TextBlock.h"
#include "Components/UIButtonWidget.h"
#include "VeConfigLibrary.h"

#define LOCTEXT_NAMESPACE "VeUI"


void UUIAboutPageWidget::NativeOnInitialized() {
	Super::NativeOnInitialized();

	if (IsValid(PtojectVersionTextWidget)) {
		PtojectVersionTextWidget->SetText(FText::FromString(GetProjectVersion()));
	}
}

FString UUIAboutPageWidget::GetCopyright() const {
	return FVeConfigLibrary::GetProjectCopyright();
}


FString UUIAboutPageWidget::GetProjectVersion() const {
	return FVeConfigLibrary::GetProjectVersion();
}


FString UUIAboutPageWidget::GetProjectHomepage() const {
	return FVeConfigLibrary::GetProjectHomepage();
}


FString UUIAboutPageWidget::GetProjectSupportContact() const {
	return FVeConfigLibrary::GetProjectSupportContact();
}


#undef LOCTEXT_NAMESPACE
