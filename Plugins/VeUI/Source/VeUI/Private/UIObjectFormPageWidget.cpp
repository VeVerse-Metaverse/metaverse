// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.

#include "UIObjectFormPageWidget.h"

#include "UIPageManagerWidget.h"
#include "Components/UIButtonWidget.h"
#include "Components/UIInputWidget.h"
#include "Components/CheckBox.h"
#include "Components/ComboBoxString.h"
#include "Components/UIMultilineInputWidget.h"
#include "Components/UILoadingWidget.h"
#include "Components/UIImageButtonWidget.h"
#include "Subsystems/UINotificationSubsystem.h"
#include "Subsystems/UIDialogSubsystem.h"
#if PLATFORM_DESKTOP
#include "VePlatform.h"
#include "IVePlatform.h"
#endif
#include "VeApi.h"
#include "VeApi2.h"
#include "VeUI.h"
#include "PlaceableLib.h"
#include "Api2FileSubsystem.h"

#define LOCTEXT_NAMESPACE "VeUI"


UUIObjectFormPageWidget::UUIObjectFormPageWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer) {

	bNoHistory = true;
}

void UUIObjectFormPageWidget::NativeOnInitialized() {
	Super::NativeOnInitialized();

	if (IsValid(CancelButtonWidget)) {
		CancelButtonWidget->GetOnButtonClicked().AddUObject(this, &UUIObjectFormPageWidget::CancelButton_OnClicked);
	}

	if (IsValid(SubmitButtonWidget)) {
		SubmitButtonWidget->GetOnButtonClicked().AddUObject(this, &UUIObjectFormPageWidget::SubmitButton_OnClicked);
	}

	if (IsValid(PreviewBrowserButtonWidget)) {
		PreviewBrowserButtonWidget->GetOnButtonClicked().AddUObject(this, &UUIObjectFormPageWidget::PreviewBrowserButton_OnClicked);
	}

	if (IsValid(ModelBrowserButtonWidget)) {
		ModelBrowserButtonWidget->GetOnButtonClicked().AddUObject(this, &UUIObjectFormPageWidget::ModelBrowserButton_OnClicked);
	}
}

void UUIObjectFormPageWidget::NativeConstruct() {
	Super::NativeConstruct();

	SetIsProcessing(false);
}

void UUIObjectFormPageWidget::SetMetadata(const FApiObjectMetadata& InMetadata) {
	Metadata = InMetadata;

	if (PreviewImageWidget) {
		PreviewImageWidget->ShowImage(Metadata.Files, EApiFileType::ImagePreview);
	}

	PublicCheckBoxWidget->SetIsChecked(Metadata.bPublic);

	NameInputWidget->SetText(FText::FromString(Metadata.Name));
	DescriptionInputWidget->SetText(FText::FromString(Metadata.Description));
	ArtistInputWidget->SetText(FText::FromString(Metadata.Artist));
	TypeInputWidget->SetText(FText::FromString(Metadata.Type));

	WidthInputWidget->SetText(FText::AsNumber(Metadata.Width));
	HeightInputWidget->SetText(FText::AsNumber(Metadata.Height));
	ScaleMultiplierInputWidget->SetText(FText::AsNumber(Metadata.ScaleMultiplier));
	DateInputWidget->SetText(FText::FromString(Metadata.Date));
	DimensionsInputWidget->SetText(FText::FromString(Metadata.Dimensions));
	MediumInputWidget->SetText(FText::FromString(Metadata.Medium));
	SourceInputWidget->SetText(FText::FromString(Metadata.Source));
	SourceUrlInputWidget->SetText(FText::FromString(Metadata.SourceUrl));
	LicenseInputWidget->SetText(FText::FromString(Metadata.License));
	CopyrightInputWidget->SetText(FText::FromString(Metadata.Copyright));
	CreditInputWidget->SetText(FText::FromString(Metadata.Credit));
	OriginInputWidget->SetText(FText::FromString(Metadata.Origin));
	LocationInputWidget->SetText(FText::FromString(Metadata.Location));

#if !PLATFORM_WINDOWS && !PLATFORM_LINUX && !PLATFORM_MAC
	PreviewBrowserButtonWidget->Hide();
	ModelBrowserButtonWidget->Hide();
#endif
}

FApiObjectMetadata UUIObjectFormPageWidget::GetNewMetadata() const {
	FApiObjectMetadata Result = Metadata;

	Result.bPublic = PublicCheckBoxWidget->IsChecked();

	Result.Name = NameInputWidget->GetText().ToString();
	Result.Description = DescriptionInputWidget->GetText().ToString();
	Result.Artist = ArtistInputWidget->GetText().ToString();
	Result.Type = TypeInputWidget->GetText().ToString();

	Result.Height = FCString::Atof(*HeightInputWidget->GetText().ToString());
	Result.Width = FCString::Atof(*WidthInputWidget->GetText().ToString());
	Result.ScaleMultiplier = FCString::Atof(*ScaleMultiplierInputWidget->GetText().ToString());
	Result.Date = DateInputWidget->GetText().ToString();
	Result.Dimensions = DimensionsInputWidget->GetText().ToString();
	Result.Medium = MediumInputWidget->GetText().ToString();
	Result.Source = SourceInputWidget->GetText().ToString();
	Result.SourceUrl = SourceUrlInputWidget->GetText().ToString();
	Result.License = LicenseInputWidget->GetText().ToString();
	Result.Copyright = CopyrightInputWidget->GetText().ToString();
	Result.Credit = CreditInputWidget->GetText().ToString();
	Result.Origin = OriginInputWidget->GetText().ToString();
	Result.Location = LocationInputWidget->GetText().ToString();

	return Result;
}

bool UUIObjectFormPageWidget::IsChanged() const {
	const auto NewMetadata = GetNewMetadata();

	return NewMetadata.bPublic != Metadata.bPublic

		|| NewMetadata.Name != Metadata.Name
		|| NewMetadata.Description != Metadata.Description
		|| NewMetadata.Artist != Metadata.Artist
		|| NewMetadata.Height != Metadata.Height
		|| NewMetadata.Width != Metadata.Width
		|| NewMetadata.ScaleMultiplier != Metadata.ScaleMultiplier
		|| NewMetadata.Type != Metadata.Type

		|| NewMetadata.Date != Metadata.Date
		|| NewMetadata.Dimensions != Metadata.Dimensions
		|| NewMetadata.Medium != Metadata.Medium
		|| NewMetadata.Source != Metadata.Source
		|| NewMetadata.SourceUrl != Metadata.SourceUrl
		|| NewMetadata.License != Metadata.License
		|| NewMetadata.Copyright != Metadata.Copyright
		|| NewMetadata.Credit != Metadata.Credit
		|| NewMetadata.Origin != Metadata.Origin
		|| NewMetadata.Location != Metadata.Location

		|| !PreviewFilePath.IsEmpty()
		|| !ModelFilePath.IsEmpty();
}

void UUIObjectFormPageWidget::PreviewBrowserButton_OnClicked() {
	const FString DialogTitle(TEXT("Open Image File"));
	const FString DefaultPath(TEXT(""));
	const FString FileTypes("Images (*.jpg,*.jpeg,*.png)|*.jpg;*.jpeg;*.png");
	TArray<FString> OutFileNames;

#if PLATFORM_DESKTOP
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

void UUIObjectFormPageWidget::ModelBrowserButton_OnClicked() {
	const FString DialogTitle(TEXT("Open Model File"));
	const FString DefaultPath(TEXT(""));
	const FString FileTypes("Models (*.gltf;*.glb)|*.gltf;*.glb|Archive files (*.zip;*.gz)|*.zip;*.gz|All files (*.*)|*.*");
	TArray<FString> OutFileNames;

#if PLATFORM_DESKTOP
	void* ParentWindowPtr = FSlateApplication::Get().GetActiveTopLevelWindow()->GetNativeWindow()->GetOSWindowHandle();
	IVePlatform* VePlatform = FVePlatformModule::Get()->GetPlatform();
	if (VePlatform) {
		uint32 SelectionFlag = EVeFileDialogFlags::None; //A value of 0 represents single file selection while a value of 1 represents multiple file selection
		VePlatform->OpenFileDialog(ParentWindowPtr, DialogTitle, DefaultPath, FString(""), FileTypes, SelectionFlag, OutFileNames);
		if (OutFileNames.Num()) {
			ModelFilePath = FPaths::ConvertRelativePathToFull(OutFileNames[0]);
			ModelInputWidget->SetText(FText::FromString(FString::Printf(TEXT("file:///%s"), *ModelFilePath)));
		}
	}
#endif
}

void UUIObjectFormPageWidget::SubmitButton_OnClicked() {
	if (bIsProcessing || !IsChanged()) {
		return;
	}

	const FApiObjectMetadata NewMetadata = GetNewMetadata();

	GET_MODULE_SUBSYSTEM(ObjectSubsystem, Api, Object);
	if (ObjectSubsystem) {
		// Create object
		if (!NewMetadata.Id.IsValid()) {
			if (PreviewFilePath.IsEmpty() && ModelFilePath.IsEmpty()) {
				return;
			}

			const auto CallbackPtr = MakeShared<FOnObjectRequestCompleted>();
			CallbackPtr->BindWeakLambda(this, [this](const FApiObjectMetadata& InMetadata, const bool bInSuccessful, const FString& InError) {
				if (!bInSuccessful) {
					LogErrorF("failed to request: %s", *InError);
				}

				OnUpdateRequestCompleted(InMetadata, bInSuccessful, InError);
			});

			SetIsProcessing(true);
			ObjectSubsystem->Create(FApiUpdateObjectMetadata(NewMetadata), CallbackPtr);
		}
		// Update object
		else {
			const auto CallbackPtr = MakeShared<FOnObjectRequestCompleted>();
			CallbackPtr->BindWeakLambda(this, [this](const FApiObjectMetadata& InMetadata, const bool bInSuccessful, const FString& InError) {
				if (!bInSuccessful) {
					LogErrorF("failed to request: %s", *InError);
				}

				OnUpdateRequestCompleted(InMetadata, bInSuccessful, InError);
			});

			SetIsProcessing(true);
			ObjectSubsystem->Update(FApiUpdateObjectMetadata(NewMetadata), CallbackPtr);
		}
	}
}

void UUIObjectFormPageWidget::OnUpdateRequestCompleted(const FApiObjectMetadata& InMetadata, const bool bSuccessful, const FString& Error) {
	bool bCreate = !Metadata.Id.IsValid();

	const auto Callback = MakeShared<FOnGenericRequestCompleted>();
	Callback->BindWeakLambda(this, [=](const bool bInSuccessful, const FString& InError) {
		SetIsProcessing(false);

		const FString Object = (bSuccessful) ? TEXT("a files") : TEXT("an object");

		if (!bInSuccessful) {
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
				NotificationData.Message = LOCTEXT("UpdateObjectErrorNotificationMessage", "Successfully created an object");
			} else {
				NotificationData.Message = LOCTEXT("UpdateObjectErrorNotificationMessage", "Successfully updated an object");
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
		Metadata = InMetadata;
		UploadFiles(Callback);
	} else {
		Callback->ExecuteIfBound(false, Error);
	}
}

void UUIObjectFormPageWidget::UploadFiles(TSharedRef<FOnGenericRequestCompleted> InCallback) {
	const auto ModelCallback = MakeShared<FOnGenericRequestCompleted>();
	ModelCallback->BindWeakLambda(this, [=](const bool bInSuccessful, const FString& InError) {
		if (!bInSuccessful) {
			VeLogError("Model file upload error");
		}

		InCallback->ExecuteIfBound(bInSuccessful, InError);
	});

	const auto PreviewCallback = MakeShared<FOnGenericRequestCompleted>();
	PreviewCallback->BindWeakLambda(this, [=](const bool bInSuccessful, const FString& InError) {
		if (bInSuccessful) {
			if (!ModelFilePath.IsEmpty()) {
				UploadFiles_Model(ModelCallback);
				return;
			}
		} else {
			VeLogError("Preview file upload error");
		}

		InCallback->ExecuteIfBound(bInSuccessful, InError);
	});

	if (!PreviewFilePath.IsEmpty()) {
		UploadFiles_Preview(PreviewCallback);
		return;
	}

	if (!ModelFilePath.IsEmpty()) {
		UploadFiles_Model(ModelCallback);
		return;
	}

	// No files to upload
	InCallback->ExecuteIfBound(true, TEXT(""));
}

void UUIObjectFormPageWidget::UploadFiles_Preview(TSharedRef<FOnGenericRequestCompleted> InCallback) {
	FString& Url = PreviewFilePath;

	const auto MimeType = UPlaceableLib::GetFileMimeType(Url);

	GET_MODULE_SUBSYSTEM(FileUpload, Api2, File);
	if (!FileUpload) {
		InCallback->ExecuteIfBound(false, TEXT(""));
		return;
	}

	const auto Callback = MakeShared<FOnFileRequestCompleted>();
	Callback->BindWeakLambda(this, [=](const FApiFileMetadata& InMetadata, const bool bInSuccessful, const FString& InError) {
		if (!bInSuccessful) {
			VeLogErrorFunc("failed to request: %s", *InError);
		}

		InCallback->ExecuteIfBound(bInSuccessful, InError);
	});

	FApi2FileUploadRequestMetadata UploadFileMetadata;
	UploadFileMetadata.EntityId = Metadata.Id;
	UploadFileMetadata.FilePath = Url;
	UploadFileMetadata.SetFileType(MimeType, Url);
	UploadFileMetadata.Mime = MimeType;

	FileUpload->Upload(UploadFileMetadata, Callback);
}

void UUIObjectFormPageWidget::UploadFiles_Model(TSharedRef<FOnGenericRequestCompleted> InCallback) {
	FString& Url = ModelFilePath;

	// Get file mime type.
	const auto MimeType = UPlaceableLib::GetFileMimeType(Url);

	GET_MODULE_SUBSYSTEM(FileSubsystem, Api2, File);
	if (!FileSubsystem) {
		InCallback->ExecuteIfBound(false, TEXT(""));
		return;
	}

	const auto Callback = MakeShared<FOnFileRequestCompleted>();
	Callback->BindWeakLambda(this, [=](const FApiFileMetadata& InMetadata, const bool bInSuccessful, const FString& InError) {
		if (!bInSuccessful) {
			VeLogErrorFunc("failed to request: %s", *InError);
		}

		InCallback->ExecuteIfBound(bInSuccessful, InError);
	});

	FApi2FileUploadRequestMetadata UploadFileMetadata;
	UploadFileMetadata.EntityId = Metadata.Id;
	UploadFileMetadata.FilePath = Url;
	UploadFileMetadata.SetFileType(MimeType, Url);
	UploadFileMetadata.Mime = MimeType;

	FileSubsystem->Upload(UploadFileMetadata, Callback);
}

// void UUIObjectFormPageWidget::OnFileUploadRequestCompleted(const FApiFileMetadata& InFileMetadata, const bool bSuccessful, const FString& Error, bool IsNewRecord) {
// 	GET_MODULE_SUBSYSTEM(NotificationSubsystem, UI, Notification);
// 	if (NotificationSubsystem) {
// 		FUINotificationData NotificationData;
//
// 		if (bSuccessful) {
// 			NotificationData.Type = EUINotificationType::Success;
// 			NotificationData.Header = LOCTEXT("CreateObjectSuccessNotificationHeader", "Success");
//
// 			if (IsNewRecord) {
// 				NotificationData.Message = LOCTEXT("CreateObjectSuccessNotificationMessage", "Successfully created an object.");
// 			} else {
// 				NotificationData.Message = LOCTEXT("CreateObjectSuccessNotificationMessage", "Successfully update an object.");
// 			}
//
// 			PreviewFilePath.Empty();
//
// 			NotificationSubsystem->AddNotification(NotificationData);
//
// 			if (IsNewRecord) {
// 				GetPageManager()->OpenPage(DetailPageClass, *Metadata.Id.ToString());
// 			} else {
// 				GetPageManager()->ShowPrevPage(true);
// 			}
// 		} else {
// 			NotificationData.Type = EUINotificationType::Failure;
// 			NotificationData.Header = LOCTEXT("CreateObjectErrorNotificationHeader", "Error");
//
// 			if (IsNewRecord) {
// 				NotificationData.Message = LOCTEXT("CreateObjectErrorNotificationMessage", "Failed to create an object.");
// 				LogWarningF("Failed to create an object: %s", *Error);
// 			} else {
// 				NotificationData.Message = LOCTEXT("CreateObjectErrorNotificationMessage", "Failed to update an object.");
// 				LogWarningF("Failed to update an object: %s", *Error);
// 			}
//
// 			NotificationSubsystem->AddNotification(NotificationData);
// 		}
// 	}
// }

void UUIObjectFormPageWidget::CancelButton_OnClicked() {
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
		OnDialogButtonClicked.BindUObject(this, &UUIObjectFormPageWidget::OnCancelDialogButtonClicked);

		DialogSubsystem->ShowDialog(DialogData, OnDialogButtonClicked);
	}
}

void UUIObjectFormPageWidget::OnCancelDialogButtonClicked(UUIDialogWidget*, const uint8& InButtonIndex) {
	if (InButtonIndex == 0) {
		GetPageManager()->ShowPrevPage();
	}
}

void UUIObjectFormPageWidget::SetIsProcessing(const bool InProcessing) {
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
