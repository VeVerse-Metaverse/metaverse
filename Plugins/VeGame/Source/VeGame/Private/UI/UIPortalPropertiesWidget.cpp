// Authors: Egor A. Pristavka, Nikolay Bulatov. 
// Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "UI/UIPortalPropertiesWidget.h"
#include "VePortal.h"
#include "VeGameModule.h"
#include "VeApi.h"
#include "VeApi2.h"
#include "Api2WorldSubsystem.h"
#include "UIPlayerController.h"
#include "Components/UIComboBoxSpace.h"
#include "Components/UIComboBoxPortal.h"


void UUIPortalPropertiesWidget::NativeOnInitialized() {
	Super::NativeConstruct();

	UpdateSaveEnabled();
	
	if (NameInputWidget) {
		NameInputWidget->OnTextChanged.AddWeakLambda(this, [=](const FText& Text) {
			UpdateSaveEnabled();
		});
	}

	if (WorldInputWidget) {
		WorldInputWidget->OnTextCommitted.AddWeakLambda(this, [=](const FText& Text, ETextCommit::Type) {
			if (Text.ToString() != SpaceFilter) {
				SpaceFilter = Text.ToString();
				ShowWorlds(SpaceFilter);
			}
		});
	}

	if (PortalInputWidget) {
		PortalInputWidget->OnTextCommitted.AddWeakLambda(this, [=](const FText& Text, ETextCommit::Type) {
			if (Text.ToString() != PortalFilter) {
				PortalFilter = Text.ToString();
				const FGuid WorldId = WorldComboBoxWidget->GetSelectedOption().Id;
				ShowPortals(WorldId, PortalFilter);
			}
		});
	}

	if (WorldComboBoxWidget) {
		WorldComboBoxWidget->OnSelectionChanged.AddDynamic(this, &UUIPortalPropertiesWidget::SpaceComboBox_OnSelectionChanged);
	}

	if (PortalComboBoxWidget) {
		PortalComboBoxWidget->OnSelectionChanged.AddDynamic(this, &UUIPortalPropertiesWidget::PortalComboBox_OnSelectionChanged);
	}

	if (RefreshRenderTargetButtonWidget) {
		RefreshRenderTargetButtonWidget->GetOnButtonClicked().AddWeakLambda(this,[this]{
			if (RefreshRenderTargetButtonClicked.IsBound()) {
				RefreshRenderTargetButtonClicked.Execute();
			}
		});
	}
}

FString UUIPortalPropertiesWidget::GetPortalName() const {
	return NameInputWidget->GetText().ToString();
}

FGuid UUIPortalPropertiesWidget::GetPortalDestinationId() const {
	return PortalComboBoxWidget->GetSelectedOption().Id;
}

void UUIPortalPropertiesWidget::SetMetadata(const FVePortalMetadata& InMetadata) {
	PortalMetadata = InMetadata;

	NameInputWidget->SetText(FText::FromString(PortalMetadata.Name));

	DefaultWorldId = PortalMetadata.Destination.World.Id;
	DefaultPortalId = PortalMetadata.Destination.Id;

	ShowWorlds(PortalMetadata.Destination.World.Name, PortalMetadata.Destination.World.Id);
}

void UUIPortalPropertiesWidget::ShowWorlds(const FString& Query, const FGuid SelectedWorldId) {
	if (!IsValid(WorldComboBoxWidget)) {
		return;
	}

	WorldComboBoxWidget->ClearOptions();

	GET_MODULE_SUBSYSTEM(WorldSubsystem, Api2, World);
	if (!WorldSubsystem) {
		return;
	}

	FApiWorldIndexRequestMetadata QueryMetadata;
	QueryMetadata.Offset = 0;
	QueryMetadata.Limit = 100;
	QueryMetadata.Query = Query;

	const auto Callback = MakeShared<FOnWorldBatchRequestCompleted2>();
	Callback->BindWeakLambda(this, [=](const FApi2WorldBatchMetadata& InMetadata, const EApi2ResponseCode InResponseCode, const FString& InError) {
		const bool bSuccessful = InResponseCode == EApi2ResponseCode::Ok;
		if (bSuccessful) {
			for (auto& Entity : InMetadata.Entities) {
				WorldComboBoxWidget->AddOption(FVeWorldMetadata(*Entity));
			}
			if (SelectedWorldId.IsValid()) {
				WorldComboBoxWidget->SetSelectedOption(SelectedWorldId);
			} else if (DefaultWorldId.IsValid()) {
				WorldComboBoxWidget->SetSelectedOption(DefaultWorldId);
			}
		} else {
			VeLogErrorFunc("failed to register a portal: %s", *InError);
		}
	});

	WorldSubsystem->Index(QueryMetadata, Callback);
	HidePortals();
}

void UUIPortalPropertiesWidget::ShowPortals(const FGuid& WorldId, const FString& Query, const FGuid SelectedPortalId) {
	if (!IsValid(PortalComboBoxWidget)) {
		return;
	}

	PortalComboBoxWidget->ClearOptions();

	GET_MODULE_SUBSYSTEM(WorldSubsystem, Api2, World);
	if (!WorldSubsystem) {
		return;
	}

	IApiBatchQueryRequestMetadata QueryMetadata;
	QueryMetadata.Offset = 0;
	QueryMetadata.Limit = 100;
	QueryMetadata.Query = Query;

	const auto Callback = MakeShared<FOnPortalBatchRequestCompleted2>();
	Callback->BindWeakLambda(this, [=](const FApi2PortalBatchMetadata& InMetadata, EApi2ResponseCode InResponseCode, const FString& InError) {
		const bool bSuccessful = InResponseCode == EApi2ResponseCode::Ok;
		if (bSuccessful) {
			for (auto& Entity : InMetadata.Entities) {
				PortalComboBoxWidget->AddOption(FVePortalMetadata(*Entity));
			}
			if (SelectedPortalId.IsValid()) {
				PortalComboBoxWidget->SetSelectedOption(SelectedPortalId);
			} else if (DefaultPortalId.IsValid()) {
				PortalComboBoxWidget->SetSelectedOption(DefaultPortalId);
			}
		} else {
			VeLogErrorFunc("failed to register a portal: %s", *InError);
		}
	});

	WorldSubsystem->GetPortalBatch(WorldId, QueryMetadata, Callback);
}

void UUIPortalPropertiesWidget::HideSpaces() {
	if (WorldInputWidget) {
		WorldInputWidget->SetText(FText());
	}
	if (WorldComboBoxWidget) {
		WorldComboBoxWidget->ClearOptions();
	}
	SpaceFilter = TEXT("");
	HidePortals();
}

void UUIPortalPropertiesWidget::HidePortals() {
	if (PortalInputWidget) {
		PortalInputWidget->SetText(FText());
	}
	if (PortalComboBoxWidget) {
		PortalComboBoxWidget->ClearOptions();
	}
	PortalFilter = TEXT("");
}

void UUIPortalPropertiesWidget::UpdateSaveEnabled() {
	bool NameValid = false;
	// bool DestinationValid = false;

	if (NameInputWidget) {
		NameValid = !NameInputWidget->GetText().IsEmpty();
	}

	// if (PortalComboBoxWidget) {
	// 	DestinationValid = PortalComboBoxWidget->GetSelectedOption().Id.IsValid();
	// }

	SetSaveEnabled(NameValid /*&& DestinationValid*/);
}

void UUIPortalPropertiesWidget::SpaceComboBox_OnSelectionChanged(FVeWorldMetadata SelectedItem, ESelectInfo::Type SelectionType) {
	HidePortals();
	ShowPortals(SelectedItem.Id, TEXT(""));
}

void UUIPortalPropertiesWidget::PortalComboBox_OnSelectionChanged(FVePortalMetadata SelectedItem, ESelectInfo::Type SelectionType) {
	UpdateSaveEnabled();
}
