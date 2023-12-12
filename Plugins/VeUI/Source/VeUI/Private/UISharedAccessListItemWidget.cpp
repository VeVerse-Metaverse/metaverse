// Authors: Khusan T.Yadgarov, Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022-2023 VeVerse AS. All rights reserved.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "UISharedAccessListItemWidget.h"

#include "Api2SharedAccessMetadataObject.h"
#include "ApiSharedAccessMetadataObject.h"
#include "VeShared.h"
#include "Components/CheckBox.h"
#include "Components/TextBlock.h"


#define LOCTEXT_NAMESharedAccess "VeUI"


void UUISharedAccessListItemWidget::NativeOnListItemObjectSet(UObject* ListItemObject) {
	if (const UApi2SharedAccessMetadataObject* SharedAccessObject = Cast<UApi2SharedAccessMetadataObject>(ListItemObject)) {
		Metadata = FApi2SharedAccessMetadata(*SharedAccessObject->Metadata);
		
		if (CanViewCheckBoxWidget) {
			CanViewCheckBoxWidget->SetIsChecked(Metadata.CanView);
		}

		if (CanEditCheckBoxWidget) {
			CanEditCheckBoxWidget->SetIsChecked(Metadata.CanEdit);
		}

		if (CanDeleteCheckBoxWidget) {
			CanDeleteCheckBoxWidget->SetIsChecked(Metadata.CanDelete);
		}

		if (NameTextWidget) {
			if (Metadata.User.Name.IsEmpty()) {
				NameTextWidget->SetText(NSLOCTEXT("VeUI", "WorldOwnerUnknown", "Unknown"));
			} else {
				NameTextWidget->SetText(FText::FromString(TruncateString(Metadata.User.Name, 16)));
			}
		}

	}
}


#undef LOCTEXT_NAMESharedAccess
