// Authors: Khusan T.Yadgarov, Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022-2023 VeVerse AS. All rights reserved.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "UISharedAccessWidget.h"

#include "Api2SharedAccessSubsystem.h"
#include "VeApi.h"
#include "VeUI.h"
#include "ApiUserSubsystem.h"
#include "UINotificationData.h"
#include "UIPageManagerWidget.h"
#include "UISharedAccessListPageWidget.h"
#include "VeApi2.h"
#include "VeUserMetadata.h"
#include "Components/CheckBox.h"
#include "Components/UIComboBoxUser.h"
#include "Components/UICommonFilterWidget.h"
#include "Subsystems/UINotificationSubsystem.h"

#define LOCTEXT_NAMESPACE "VeUI"


void UUISharedAccessWidget::NativeOnInitialized() {
	// if (IsValid(FilterWidget)) {
	// 	FilterWidget->OnFilterChanged.AddWeakLambda(this, [this](FString InQuery) {
	// 	   RequestMetadata.Query = InQuery;
	// 	   RequestMetadata.Offset = 0;
	// 		UserMetadataRequest(RequestMetadata);
	//    });
	// }

	// if (UserComboBoxWidget) {
	// 	UserComboBoxWidget->OnSelectionChanged.AddDynamic(this, &UUISharedAccessWidget::UserComboBox_OnSelectionChanged);
	// }

	if (SaveButtonWidget) {
		SaveButtonWidget->GetOnButtonClicked().AddWeakLambda(this, [=] {
			UserMetadata = UserComboBoxWidget->GetSelectedOption();
			UpdateSharedAccessMetadataRequest();
		});
	}

	if (UserListButtonWidget) {
		UserListButtonWidget->GetOnButtonClicked().AddWeakLambda(this, [=] {
			OnUserListButtonClicked.Broadcast();
		});
	}

	if (UserInputWidget) {
		UserInputWidget->OnTextCommitted.AddWeakLambda(this, [=](const FText& Text, ETextCommit::Type) {
			if (Text.ToString() != UserFilter) {
				UserFilter = Text.ToString();
				const FGuid UserId = UserComboBoxWidget->GetSelectedOption().Id;
				ShowUsers(UserId, UserFilter);
			}
		});
	}
}

void UUISharedAccessWidget::UserComboBox_OnSelectionChanged(FVeUserMetadata SelectedItem, ESelectInfo::Type SelectionType) {
	UserMetadata = SelectedItem;
}

void UUISharedAccessWidget::ShowUsers(const FGuid& UserId, const FString& Query, const FGuid SelectedUserId) {
	if (!IsValid(UserComboBoxWidget)) {
		return;
	}

	UserComboBoxWidget->ClearOptions();

	GET_MODULE_SUBSYSTEM(UserSubsystem, Api, User);
	if (!UserSubsystem) {
		return;
	}

	IApiBatchQueryRequestMetadata QueryMetadata;
	QueryMetadata.Offset = 0;
	QueryMetadata.Limit = 100;
	QueryMetadata.Query = Query;

	const auto Callback = MakeShared<FOnUserBatchRequestCompleted>();
	Callback->BindWeakLambda(this, [=](const FApiUserBatchMetadata InBatchMetadata, const bool bInIsSuccessful, const FString& Error) {
		if (bInIsSuccessful) {
			for (auto& Entity : InBatchMetadata.Entities) {
				UserComboBoxWidget->AddOption(FVeUserMetadata(*Entity));
			}
			if (SelectedUserId.IsValid()) {
				UserComboBoxWidget->SetSelectedOption(SelectedUserId);
			} else if (DefaultUserId.IsValid()) {
				UserComboBoxWidget->SetSelectedOption(DefaultUserId);
			}
		} else {
			VeLogErrorFunc("failed to register a portal: %s", *Error);
		}
	});

	UserSubsystem->GetBatch(QueryMetadata, Callback);
}

void UUISharedAccessWidget::SetMetadata(const FVeWorldMetadata& InWorldMetadata) {
	WorldMetadata = InWorldMetadata;
}

void UUISharedAccessWidget::UpdateSharedAccessMetadataRequest() {

	GET_MODULE_SUBSYSTEM(SharedAccessSubsystem, Api2, SharedAccess);
	if (SharedAccessSubsystem) {
		// 1. Create a callback.
		const auto CallbackPtr = MakeShared<FOnSharedAccessRequestCompleted2>();

		// 2. Bind callback.
		CallbackPtr->BindWeakLambda(this, [=](const FApi2SharedAccessMetadata& InMetadata, const EApi2ResponseCode InResponseCode, const FString& InError) {
			bool bSuccessful = InResponseCode == EApi2ResponseCode::Ok;
			if (!bSuccessful) {
				LogErrorF("failed to request: %s", *InError);
			}

			// SetIsProcessing(false);
			OnAccessMetadataResponse(InMetadata, bSuccessful, InError);
		});

		SharedAccessMetadata.CanView = CanViewCheckBoxWidget->IsChecked();
		SharedAccessMetadata.CanEdit = CanEditCheckBoxWidget->IsChecked();
		SharedAccessMetadata.CanDelete = CanDeleteCheckBoxWidget->IsChecked();
		SharedAccessMetadata.UserId = UserMetadata.Id;
		SharedAccessMetadata.EntityId = WorldMetadata.Id;
		SharedAccessMetadata.Owner = WorldMetadata.Owner.ToApiUserMetadata();


		// 3. Make the request using the callback.
		SharedAccessSubsystem->UpdateSharedAccess(SharedAccessMetadata, CallbackPtr);
	}
}

void UUISharedAccessWidget::OnAccessMetadataResponse(const FApi2SharedAccessMetadata& InMetadata, bool bSuccessful, const FString& Error) {
	GET_MODULE_SUBSYSTEM(NotificationSubsystem, UI, Notification);
	if (NotificationSubsystem) {
		if (!bSuccessful) {
			FUINotificationData NotificationData;
			NotificationData.Type = EUINotificationType::Failure;
			NotificationData.Header = LOCTEXT("UpdatePermissionsErrorNotificationHeader", "Error");
			NotificationData.Message = FText::Format(LOCTEXT("UpdatePermissionsErrorNotificationMessage", "Failed to give permissions {0}"), FText::FromString(Error));
			NotificationSubsystem->AddNotification(NotificationData);
		} else {
			FUINotificationData NotificationData;
			NotificationData.Type = EUINotificationType::Success;
			NotificationData.Header = LOCTEXT("UpdateObjectSuccessNotificationHeader", "Success");
			NotificationData.Message = LOCTEXT("UpdateObjectErrorNotificationMessage", "Successfully granted permissions");
			NotificationSubsystem->AddNotification(NotificationData);
		}
	}

}


#undef LOCTEXT_NAMESPACE
