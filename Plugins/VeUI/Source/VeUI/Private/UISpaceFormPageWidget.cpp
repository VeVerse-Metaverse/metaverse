// Authors: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022-2023 VeVerse AS. All rights reserved.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "UISpaceFormPageWidget.h"

#if PLATFORM_DESKTOP
#include "VePlatform.h"
#include "IVePlatform.h"
#endif
#include "VeUI.h"
#include "UIPageManagerWidget.h"
#include "Subsystems/UINotificationSubsystem.h"
#include "Subsystems/UIDialogSubsystem.h"
#include "Components/ComboBoxString.h"
#include "Components/CheckBox.h"
#include "Components/UIImageWidget.h"
#include "Components/UIFormInputWidget.h"
#include "Components/UIMultilineInputWidget.h"
#include "Components/UIInputWidget.h"
#include "Components/UILoadingWidget.h"
#include "GenericPlatform/GenericPlatformHttp.h"
#include "VeApi.h"
#include "VeApi2.h"
#include "VeShared.h"
#include "Api2FileSubsystem.h"
#include "Api2WorldSubsystem.h"
#include "UISharedAccessListPageWidget.h"
#include "UISharedAccessWidget.h"


#define LOCTEXT_NAMESPACE "VeUI"

UUISpaceFormPageWidget::UUISpaceFormPageWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer) {

	bNoHistory = true;
}

void UUISpaceFormPageWidget::NativeOnInitialized() {
	Super::NativeOnInitialized();

	DefaultMaps.Emplace(TEXT("/ArtheonGallery/Showcase/Maps/TemplateGallery"));
	DefaultMaps.Emplace(TEXT("/Braccio_Nuovo_Vatican_Museums/Maps/IlluminatedSculptureMuseum"));

	if (MapComboBoxWidget) {
		MapComboBoxWidget->ClearOptions();
		for (auto& Map : DefaultMaps) {
			MapComboBoxWidget->AddOption(Map);
		}
	}

	if (CancelButtonWidget) {
		CancelButtonWidget->GetOnButtonClicked().AddUObject(this, &UUISpaceFormPageWidget::CancelButton_OnClicked);
	}

	if (SubmitButtonWidget) {
		SubmitButtonWidget->GetOnButtonClicked().AddUObject(this, &UUISpaceFormPageWidget::SubmitButton_OnClicked);
	}

	if (IsValid(FileBrowserButtonWidget)) {
		FileBrowserButtonWidget->GetOnButtonClicked().AddUObject(this, &UUISpaceFormPageWidget::FileBrowserButton_OnClicked);
	}

	if (IsValid(UrlInputWidget)) {
		if (!UrlInputWidget->OnTextCommitted.IsBound()) {
			UrlInputWidget->OnTextCommitted.AddWeakLambda(this, [this](const FText& InText, const ETextCommit::Type InCommitMethod) {
				OnUrlTextCommittedCallback(InText, InCommitMethod);
			});
		}
	}
	
	if (SharedAccessWidget) {
		SharedAccessWidget->OnUserListButtonClicked.AddWeakLambda(this, [=]() {
			if (auto* Page = GetPageManager()->OpenPage<UUISharedAccessListPageWidget>(SharedAccessListPageClass, Metadata.Id.ToString())) {
				Page->SetParentPage(this);
			}
		});
	}
}

void UUISpaceFormPageWidget::OnUrlTextCommittedCallback(const FText& InText, ETextCommit::Type InCommitMethod) {
	PreviewFilePath = InText.ToString();
}

void UUISpaceFormPageWidget::NativeConstruct() {
	Super::NativeConstruct();

	SetIsProcessing(false);
}

void UUISpaceFormPageWidget::NativeOnRefresh() {
	Super::NativeOnRefresh();

	SetMetadata(Metadata);
}

void UUISpaceFormPageWidget::SetMetadata(const FVeWorldMetadata& InMetadata) {
	Metadata = InMetadata;

	if (SharedAccessWidget) {
		SharedAccessWidget->SetMetadata(InMetadata);
	}
	
	if (!Metadata.Map.IsEmpty()) {
		if (!DefaultMaps.Contains(Metadata.Map)) {
			MapComboBoxWidget->AddOption(Metadata.Map);
		}
		MapComboBoxWidget->SetSelectedOption(Metadata.Map);
	} else {
		MapComboBoxWidget->SetSelectedIndex(0);
		Metadata.Map = MapComboBoxWidget->GetOptionAtIndex(0);
	}

	NameInputWidget->SetText(FText::FromString(Metadata.Name));
	DescriptionInputWidget->SetText(FText::FromString(Metadata.Description));
	PublicCheckBoxWidget->SetIsChecked(Metadata.bPublic);

#if !PLATFORM_WINDOWS && !PLATFORM_LINUX && !PLATFORM_MAC
	FileBrowserButtonWidget->Hide();
#endif
}

void UUISpaceFormPageWidget::UpdatePublicMetadata() {
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
		});

		SharedAccessMetadata.Public = PublicCheckBoxWidget->IsChecked();
		SharedAccessMetadata.EntityId = Metadata.Id;
		SharedAccessMetadata.Owner = Metadata.Owner.ToApiUserMetadata();
		
		// 3. Make the request using the callback.
		SharedAccessSubsystem->UpdatePublicAccess(SharedAccessMetadata, CallbackPtr);
	}
}

FVeWorldMetadata UUISpaceFormPageWidget::GetNewMetadata() const {
	auto Result = Metadata;

	Result.Name = NameInputWidget->GetText().ToString();
	Result.Description = DescriptionInputWidget->GetText().ToString();
	Result.bPublic = PublicCheckBoxWidget->IsChecked();
	Result.Map = MapComboBoxWidget->GetSelectedOption();

	return Result;
}

bool UUISpaceFormPageWidget::IsChanged() const {
	const auto NewMetadata = GetNewMetadata();

	return NewMetadata.Name != Metadata.Name
		|| NewMetadata.Description != Metadata.Description
		|| NewMetadata.bPublic != Metadata.bPublic
		|| NewMetadata.Map != Metadata.Map
		|| !PreviewFilePath.IsEmpty();
}

void UUISpaceFormPageWidget::SubmitButton_OnClicked() {
	if (bIsProcessing || !IsChanged()) {
		return;
	}
	const auto NewMetadata = GetNewMetadata();

	// Create world
	if (!NewMetadata.Id.IsValid()) {
		GET_MODULE_SUBSYSTEM(SpaceSubsystem, Api2, World);
		if (SpaceSubsystem) {
			const auto CallbackPtr = MakeShared<FOnWorldRequestCompleted2>();
			CallbackPtr->BindWeakLambda(this, [this, CallbackPtr](const FApiSpaceMetadata& InMetadata, const EApi2ResponseCode InResponseCode, const FString& InError) {
				const bool bSuccessful = InResponseCode == EApi2ResponseCode::Ok;
				if (!bSuccessful) {
					VeLogErrorFunc("failed to request: %s", *InError);
				}

				OnUpdateRequestCompleted(InMetadata, bSuccessful, InError);
			});

			SetIsProcessing(true);
			SpaceSubsystem->Create(NewMetadata, CallbackPtr);
		}
	}

	// Update world
	else {
		GET_MODULE_SUBSYSTEM(SpaceSubsystem, Api, Space);
		if (SpaceSubsystem) {
			const auto CallbackPtr = MakeShared<FOnSpaceRequestCompleted>();
			CallbackPtr->BindWeakLambda(this, [this, CallbackPtr](const FApiSpaceMetadata& InMetadata, const bool bInSuccessful, const FString& InError) {
				if (!bInSuccessful) {
					VeLogErrorFunc("failed to request: %s", *InError);
				}

				OnUpdateRequestCompleted(InMetadata, bInSuccessful, InError);
			});

			SetIsProcessing(true);
			if (!SpaceSubsystem->Update(NewMetadata, CallbackPtr)) {
				CallbackPtr->ExecuteIfBound({}, false, TEXT(""));
			}
		}
	}

	//Update Public Metadata
	UpdatePublicMetadata();
	
	//Apiv2
	// else {
	// 	GET_MODULE_SUBSYSTEM(SpaceSubsystem, Api2, World);
	// 	if (SpaceSubsystem) {
	// 		const auto CallbackPtr = MakeShared<FOnSpaceRequestCompleted2>();
	// 		CallbackPtr->BindWeakLambda(this, [this, CallbackPtr](const FApiSpaceMetadata& InMetadata, const EApi2ResponseCode InResponseCode, const FString& InError) {
	// 			const bool bInSuccessful = InResponseCode == EApi2ResponseCode::Ok;
	// 			if (!bInSuccessful) {
	// 				VeLogErrorFunc("failed to request: %s", *InError);
	// 			}
	//
	// 			OnUpdateRequestCompleted(InMetadata, bInSuccessful, InError);
	// 		});
	//
	// 		SetIsProcessing(true);
	// 		SpaceSubsystem->Update(NewMetadata, CallbackPtr);
	// 	}
	// }
}

void UUISpaceFormPageWidget::FileBrowserButton_OnClicked() {
#if PLATFORM_DESKTOP
	const FString DialogTitle(TEXT("DialogTitle"));
	const FString DefaultPath(TEXT(""));
	const FString FileTypes("Images(*.jpg,*.jpeg,*.png)|*.jpg;*.jpeg;*.png");
	TArray<FString> OutFileNames;

	void* ParentWindowPtr = FSlateApplication::Get().GetActiveTopLevelWindow()->GetNativeWindow()->GetOSWindowHandle();
	IVePlatform* VePlatform = FVePlatformModule::Get()->GetPlatform();
	if (VePlatform) {
		uint32 SelectionFlag = EVeFileDialogFlags::None; //A value of 0 represents single file selection while a value of 1 represents multiple file selection
		VePlatform->OpenFileDialog(ParentWindowPtr, DialogTitle, DefaultPath, FString(""), FileTypes, SelectionFlag, OutFileNames);
		if (OutFileNames.Num()) {
			PreviewFilePath = FPaths::ConvertRelativePathToFull(OutFileNames[0]);
			if (PreviewImageWidget) {
				PreviewImageWidget->ShowImage(FString::Printf(TEXT("file:///%s"), *PreviewFilePath));
			}
		}
	}
#endif
}

void UUISpaceFormPageWidget::UploadFiles(TSharedRef<FOnGenericRequestCompleted2> InCallback) {
	if (!PreviewFilePath.IsEmpty()) {
		UploadFiles_Preview(InCallback);
		return;
	}

	// No files to upload
	InCallback->ExecuteIfBound(EApi2ResponseCode::Ok, TEXT(""));
}

void UUISpaceFormPageWidget::UploadFiles_Preview(TSharedRef<FOnGenericRequestCompleted2> InCallback) {
	FString& Url = PreviewFilePath;

	const auto MimeType = FGenericPlatformHttp::GetMimeType(Url);

	GET_MODULE_SUBSYSTEM(FileUpload, Api2, File);
	if (!FileUpload) {
		InCallback->ExecuteIfBound(EApi2ResponseCode::Failed, TEXT(""));
		return;
	}

	const auto Callback = MakeShared<FOnFileRequestCompleted2>();
	Callback->BindWeakLambda(this, [=](const FApiFileMetadata& InMetadata, const EApi2ResponseCode InResponseCode, const FString& InError) {
		if (InResponseCode != EApi2ResponseCode::Ok) {
			VeLogErrorFunc("failed to request: %s", *InError);
		}

		InCallback->ExecuteIfBound(InResponseCode, InError);
	});

	FApi2FileUploadRequestMetadata UploadFileMetadata;
	UploadFileMetadata.EntityId = Metadata.Id;
	UploadFileMetadata.FilePath = Url;
	UploadFileMetadata.SetFileType(MimeType, Url);
	UploadFileMetadata.Mime = MimeType;

	FileUpload->Upload2(UploadFileMetadata, Callback);
}

void UUISpaceFormPageWidget::OnUpdateRequestCompleted(const FApiSpaceMetadata& InMetadata, const bool bSuccessful, const FString& Error) {
	bool bCreate = !Metadata.Id.IsValid();

	const auto Callback = MakeShared<FOnGenericRequestCompleted2>();
	Callback->BindWeakLambda(this, [=](const EApi2ResponseCode InResponseCode, const FString& InError) {
		SetIsProcessing(false);

		const FString Object = (InResponseCode == EApi2ResponseCode::Ok) ? TEXT("a file") : TEXT("a world");

		if (InResponseCode != EApi2ResponseCode::Ok) {
			if (bCreate) {
				VeLogWarningFunc("Failed to create %s", *Object);
			} else {
				VeLogWarningFunc("Failed to update %s", *Object);
			}

			GET_MODULE_SUBSYSTEM(NotificationSubsystem, UI, Notification);
			if (NotificationSubsystem) {
				FUINotificationData NotificationData;
				NotificationData.Type = EUINotificationType::Failure;
				NotificationData.Header = LOCTEXT("UpdateObjectErrorNotificationHeader", "Error");
				if (bCreate) {
					NotificationData.Message = FText::Format(LOCTEXT("UpdateObjectErrorNotificationMessage", "Failed to create {0}"), FText::FromString(Object));
				} else {
					NotificationData.Message = FText::Format(LOCTEXT("UpdateObjectErrorNotificationMessage", "Failed to update {0}"), FText::FromString(Object));
				}
				NotificationSubsystem->AddNotification(NotificationData);
			}

			return;
		}

		GET_MODULE_SUBSYSTEM(NotificationSubsystem, UI, Notification);
		if (NotificationSubsystem) {
			FUINotificationData NotificationData;
			NotificationData.Type = EUINotificationType::Success;
			NotificationData.Header = LOCTEXT("UpdateObjectSuccessNotificationHeader", "Success");
			if (bCreate) {
				NotificationData.Message = LOCTEXT("UpdateObjectErrorNotificationMessage", "Successfully created a world");
			} else {
				NotificationData.Message = LOCTEXT("UpdateObjectErrorNotificationMessage", "Successfully updated a world");
			}
			NotificationSubsystem->AddNotification(NotificationData);
		}

		if (bCreate) {
			GetPageManager()->OpenPage(DetailPageClass, *Metadata.Id.ToString(EGuidFormats::DigitsWithHyphensLower));
		} else {
			GetPageManager()->ShowPrevPage(true);
		}
	});

	if (bSuccessful) {
		Metadata = FVeWorldMetadata(InMetadata);
		UploadFiles(Callback);
	} else {
		Callback->ExecuteIfBound(EApi2ResponseCode::Failed, Error);
	}
}

void UUISpaceFormPageWidget::CancelButton_OnClicked() {
	if (!IsChanged()) {
		GetPageManager()->ShowPrevPage();
		return;
	}

	GET_MODULE_SUBSYSTEM(DialogSubsystem, UI, Dialog);
	if (DialogSubsystem) {
		FUIDialogData DialogData;
		DialogData.Type = EUIDialogType::OkCancel;
		DialogData.HeaderText = NSLOCTEXT("VeUI", "ConfirmCloseDialogHeader", "Confirm");
		DialogData.MessageText = NSLOCTEXT("VeUI", "ConfirmCloseDialogMessage", "You will lose your changes");
		DialogData.FirstButtonText = NSLOCTEXT("VeUI", "ConfirmCloseDialogButtonOk", "OK");
		DialogData.SecondButtonText = NSLOCTEXT("VeUI", "ConfirmCloseDialogButtonCancel", "Cancel");
		DialogData.bCloseOnButtonClick = true;

		FUIDialogButtonClicked OnDialogButtonClicked;
		OnDialogButtonClicked.BindUObject(this, &UUISpaceFormPageWidget::OnCancelDialogButtonClicked);

		DialogSubsystem->ShowDialog(DialogData, OnDialogButtonClicked);
	}
}

void UUISpaceFormPageWidget::OnCancelDialogButtonClicked(UUIDialogWidget*, const uint8& InButtonIndex) {
	if (InButtonIndex == 0) {
		GetPageManager()->ShowPrevPage();
	}
}

void UUISpaceFormPageWidget::SetIsProcessing(const bool InProcessing) {
	bIsProcessing = InProcessing;
	if (LoadingWidget) {
		if (InProcessing) {
			LoadingWidget->Show();
		} else {
			LoadingWidget->Hide();
		}
	}
}


#undef LOCTEXT_NAMESPACE
