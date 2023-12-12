// Copyright (c) 2022 LE7EL AS. All right reserved.

#include "Editor/UIEditorDetailsWidget.h"

#include "VeShared.h"
#include "VeUI.h"
#include "VeGameFramework.h"
#include "GameFrameworkEditorSubsystem.h"
#include "Components/TextBlock.h"
#include "Components/EditableText.h"
#include "Components/Border.h"
#include "Components/BorderSlot.h"
#include "Components/UIButtonWidget.h"
#include "UI/UIPlaceablePropertiesWidget.h"
#include "PlaceableComponent.h"
#include "EditorComponent.h"
#include "PlaceableLib.h"

void UUIEditorDetailsWidget::NativeOnInitialized() {
	Super::NativeOnInitialized();

	SetVisibility(ESlateVisibility::Collapsed);
	Clear();

	if (SaveButtonWidget) {
		SaveButtonWidget->SetIsEnabled(false);
		SaveButtonWidget->GetOnButtonClicked().AddWeakLambda(this, [=]() {
			if (PropertiesWidget.IsValid()) {
				PropertiesWidget->Save();
			}
		});
	}

	if (ResetButtonWidget) {
		ResetButtonWidget->SetIsEnabled(false);
		ResetButtonWidget->GetOnButtonClicked().AddWeakLambda(this, [=]() {
			if (PropertiesWidget.IsValid()) {
				PropertiesWidget->Reset();
			}
		});
	}
}

void UUIEditorDetailsWidget::NativeConstruct() {
	Super::NativeConstruct();

	if (!ContainerWidget) {
		return;
	}

	GET_MODULE_SUBSYSTEM(EditorSubsystem, GameFramework, Editor);
	if (EditorSubsystem) {
		EditorSubsystem->GetOnEditorComponentChanged().AddUObject(this, &UUIEditorDetailsWidget::BindToEditorComponent);
		BindToEditorComponent(EditorSubsystem->GetEditorComponent(), nullptr);
	}
}

void UUIEditorDetailsWidget::NativeDestruct() {
	Super::NativeDestruct();

	GET_MODULE_SUBSYSTEM(EditorSubsystem, GameFramework, Editor);
	if (EditorSubsystem) {
		EditorSubsystem->GetOnEditorComponentChanged().RemoveAll(this);
		BindToEditorComponent(nullptr, EditorSubsystem->GetEditorComponent());
	}
}

void UUIEditorDetailsWidget::BindToEditorComponent(UEditorComponent* NewEditorComponent, UEditorComponent* OldEditorComponent) {
	if (OldEditorComponent) {
		OldEditorComponent->GetOnSelectionChanged().RemoveAll(this);
	}

	if (NewEditorComponent) {
		NewEditorComponent->GetOnSelectionChanged().AddWeakLambda(this, [=](const TArray<AActor*>& SelectedActors) {
			if (SelectedActors.Num() == 0) {
				SetVisibility(ESlateVisibility::Collapsed);
				return;
			}

			if (SelectedActors.Num() > 1) {
				SetVisibility(ESlateVisibility::Collapsed);
				return;
			}

			// Show properties for single placeable actor

			Clear();
			SetVisibility(ESlateVisibility::SelfHitTestInvisible);

			SelectedActor = SelectedActors.Last();
			if (SelectedActor.IsValid()) {

				if (NameTextWidget) {
					NameTextWidget->SetText(FText::FromString(SelectedActor->GetName() + TEXT(" ")));
				}

				if (auto* PlaceableInterface = Cast<IPlaceable>(SelectedActor.Get())) {
					PlaceableInterface->GetOnPlaceableStateChanged().AddUObject(this, &UUIEditorDetailsWidget::PlaceableInterface_OnPlaceableStateChanged);
					PlaceableInterface_OnPlaceableStateChanged(PlaceableInterface->GetPlaceableState(), PlaceableInterface->GetPlaceableRatio());

					PlaceableComponent = UPlaceableLib::GetPlaceableComponent(SelectedActor.Get());
					if (PlaceableComponent.IsValid()) {
						if (IdTextWidget) {
							IdTextWidget->SetText(FText::FromString(PlaceableComponent->GetMetadata().Id.ToString(EGuidFormats::DigitsWithHyphensLower)));
						}
					}

					PropertiesWidget = PlaceableInterface->Execute_GetPlaceablePropertiesWidget(SelectedActor.Get(), GetOwningPlayer());
					if (PropertiesWidget.IsValid()) {
						if (SaveButtonWidget) {
							PropertiesWidget->GetOnSaveEnableChanged().BindWeakLambda(this, [=](bool bInIsEnabled) {
								bSaveEnableWidget = bInIsEnabled;
								UpdateButtons();
							});
							bSaveEnableWidget = PropertiesWidget->GetSaveEnabled();
							UpdateButtons();
						}

						if (ResetButtonWidget) {
							PropertiesWidget->GetOnResetEnableChanged().BindWeakLambda(this, [=](bool bInIsEnabled) {
								bResetEnableWidget = bInIsEnabled;
								UpdateButtons();
							});
							bResetEnableWidget = PropertiesWidget->GetResetEnabled();
							UpdateButtons();
						}

						if (auto* PanelSlot = Cast<UBorderSlot>(ContainerWidget->AddChild(PropertiesWidget.Get()))) {
							PanelSlot->SetPadding({8.f, 8.f, 8.f, 8.f});
							// PanelSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Fill);
							// PanelSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Fill);
						}
					}
				}
			}
		});
	}
}

void UUIEditorDetailsWidget::Clear() {
	if (auto* PlaceableInterface = Cast<IPlaceable>(SelectedActor.Get())) {
		PlaceableInterface->GetOnPlaceableStateChanged().RemoveAll(this);
	}

	SelectedActor.Reset();
	PlaceableComponent.Reset();
	PropertiesWidget.Reset();

	if (ContainerWidget) {
		ContainerWidget->ClearChildren();
	}

	if (NameTextWidget) {
		NameTextWidget->SetText(FText());
	}

	if (IdTextWidget) {
		IdTextWidget->SetText(FText());
	}
}

void UUIEditorDetailsWidget::UpdateButtons() {
	if (SaveButtonWidget) {
		SaveButtonWidget->SetIsEnabled(bSaveEnableInterface && bSaveEnableWidget);
	}
	if (ResetButtonWidget) {
		ResetButtonWidget->SetIsEnabled(bResetEnableInterface && bResetEnableWidget);
	}
}

void UUIEditorDetailsWidget::PlaceableInterface_OnPlaceableStateChanged(EPlaceableState State, float Ratio) {
	if (SaveButtonWidget) {
		switch (State) {
		case EPlaceableState::Downloading:
			if (SaveButtonWidget) {
				SaveButtonWidget->SetButtonText(FText::FromString(TEXT("Downloading...")));
			}
			bSaveEnableInterface = false;
			bResetEnableInterface = false;
			break;
		case EPlaceableState::Uploading:
			if (SaveButtonWidget) {
				SaveButtonWidget->SetButtonText(FText::FromString(TEXT("Uploading...")));
			}
			bSaveEnableInterface = false;
			bResetEnableInterface = false;
			break;
		default:
			if (SaveButtonWidget) {
				SaveButtonWidget->SetButtonText(FText::FromString(TEXT("Save")));
			}
			bSaveEnableInterface = true;
			bResetEnableInterface = true;
		}
	}

	UpdateButtons();
}
