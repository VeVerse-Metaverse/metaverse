// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.


#include "UIFileBrowserListItemWidget.h"

#include "PlatformFileInfo.h"
#include "VeShared.h"


void UUIFileBrowserListItemWidget::NativeConstruct() {
	Super::NativeConstruct();
}

void UUIFileBrowserListItemWidget::NativeOnListItemObjectSet(UObject* ListItemObject) {
	if (const UPlatformFileInfo* FileObject = Cast<UPlatformFileInfo>(ListItemObject)) {
		File = FileObject->Metadata;

		LogF("Name: %s", *File.Name);

		if (NameTextWidget) {
			if (File.Name.IsEmpty()) {
				NameTextWidget->SetText(NSLOCTEXT("VeUI", "FileNameUnknown", "Unnamed"));
			} else {
				NameTextWidget->SetText(FText::FromString(File.Name));
			}
		}
		// if (PreviewImageWidget) {
		// }
	}
}
