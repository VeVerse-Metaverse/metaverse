// Copyright (c) 2022 LE7EL AS. All right reserved.

#include "Inspector/UIInspectorRootWidget.h"

#include "Components/EditableText.h"
#include "Components/UIButtonWidget.h"
#include "PlaceableComponent.h"
#include "VeShared.h"
#include "Components/ScrollBox.h"
#include "Components/ScrollBoxSlot.h"
#include "VeUI.h"
#include "Inspector/IInspectable.h"

void UUIInspectorRootWidget::NativeOnInitialized() {
	Super::NativeOnInitialized();

	Clear();

	if (CloseButtonWidget) {
		CloseButtonWidget->SetIsEnabled(false);
		CloseButtonWidget->GetOnButtonClicked().AddWeakLambda(this, [=] {
			OnClosedDelegate.Broadcast();
		});
	}
}

void UUIInspectorRootWidget::NativeOnUpdateInputMode() {
	SetInputModeGameAndUi();
}

bool UUIInspectorRootWidget::StartInspection(AActor* InInspectedActor) {
	if (auto* PlayerController = GetOwningPlayer()) {
		if (IsValid(PropertyContainerWidget)) {
			PropertyContainerWidget->ClearChildren();
		}

		if (IsValid(PreviewContainerWidget)) {
			PreviewContainerWidget->ClearChildren();
		}

		if (IsValid(InInspectedActor)) {
			InspectedActor = InInspectedActor;
			PlaceableComponent = InspectedActor->FindComponentByClass<UPlaceableComponent>();

			if (const auto* InspectableInterface = Cast<IInspectable>(InspectedActor)) {
				PropertiesWidget = InspectableInterface->Execute_GetInspectPropertiesWidget(InspectedActor.Get(), PlayerController);
				if (PropertiesWidget.IsValid()) {
					if (auto* PanelSlot = Cast<UScrollBoxSlot>(PropertyContainerWidget->AddChild(PropertiesWidget.Get()))) {
						PanelSlot->SetPadding({8.f, 8.f, 8.f, 8.f});
					}
				} else {
					VeLogErrorFunc("failed to create an inspect properties widget");
				}

				PreviewWidget = InspectableInterface->Execute_GetInspectPreviewWidget(InspectedActor.Get(), PlayerController);
				if (PreviewWidget.IsValid()) {
					PreviewContainerWidget->AddChild(PreviewWidget.Get());
				} else {
					VeLogErrorFunc("failed to create an inspect preview widget");
				}

				if (PropertiesWidget.IsValid() || PreviewWidget.IsValid()) {
					return true;
				}
			} else {
				VeLogErrorFunc("failed to cast to an interface");
			}
		} else {
			Clear();
		}
	}
	return false;
}

void UUIInspectorRootWidget::Clear() {
	InspectedActor.Reset();
	PlaceableComponent.Reset();
	PropertiesWidget.Reset();
}
