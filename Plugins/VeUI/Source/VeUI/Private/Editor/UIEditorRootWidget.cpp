#include "Editor/UIEditorRootWidget.h"

#include "Kismet/GameplayStatics.h"
#include "Editor/UIEditorAssetBrowserDragDropOperation.h"
#include "Editor/UIEditorDetailsWidget.h"
#include "Editor/UIEditorAssetBrowserWidget.h"
#include "Editor/UIEditorToolbarWidget.h"
#include "VeGameFramework.h"
#include "EditorComponent.h"
#include "VeShared.h"
#include "VeUI.h"


void UUIEditorRootWidget::NativeOnInitialized() {
	Super::NativeOnInitialized();

	if (DetailsWidget) {
		DetailsWidget->GetOnMouseEnter().AddUObject(this, &UUIEditorRootWidget::PanelMouseEnter);
		DetailsWidget->GetOnMouseLeave().AddUObject(this, &UUIEditorRootWidget::PanelMouseLeave);
	}

	if (AssetBrowserWidget) {
		AssetBrowserWidget->GetOnMouseEnter().AddUObject(this, &UUIEditorRootWidget::PanelMouseEnter);
		AssetBrowserWidget->GetOnMouseLeave().AddUObject(this, &UUIEditorRootWidget::PanelMouseLeave);
	}

	if (ToolbarWidget) {
		ToolbarWidget->GetOnMouseEnter().AddUObject(this, &UUIEditorRootWidget::PanelMouseEnter);
		ToolbarWidget->GetOnMouseLeave().AddUObject(this, &UUIEditorRootWidget::PanelMouseLeave);
	}

	if (CloseButtonWidget) {
		CloseButtonWidget->GetOnButtonClicked().AddWeakLambda(this, [=]() {
			NativeOnClose();
		});
	}
}

void UUIEditorRootWidget::NativeConstruct() {
	Super::NativeConstruct();

	GET_MODULE_SUBSYSTEM(EditorSubsystem, GameFramework, Editor);
	if (EditorSubsystem) {
		EditorSubsystem->GetOnEditorComponentChanged().AddUObject(this, &UUIEditorRootWidget::BindToEditorComponent);
		BindToEditorComponent(EditorSubsystem->GetEditorComponent(), nullptr);
	}
}

void UUIEditorRootWidget::NativeDestruct() {
	Super::NativeDestruct();

	GET_MODULE_SUBSYSTEM(EditorSubsystem, GameFramework, Editor);
	if (EditorSubsystem) {
		EditorSubsystem->GetOnEditorComponentChanged().RemoveAll(this);
		BindToEditorComponent(nullptr, EditorSubsystem->GetEditorComponent());
	}
}

void UUIEditorRootWidget::BindToEditorComponent(UEditorComponent* NewEditorComponent, UEditorComponent* OldEditorComponent) {
	if (OldEditorComponent) {
		OldEditorComponent->GetOnGizmoStateChanged().RemoveAll(this);
	}

	if (NewEditorComponent) {
		NewEditorComponent->GetOnGizmoStateChanged().AddWeakLambda(this, [=](ETransformType /*GizmoType*/, bool bTransformInProgress, ETransformationDomain /*Domain*/) {
			NativeOnUpdateInputMode();
		});
	}
}

void UUIEditorRootWidget::PanelMouseEnter() {
	if (Hovered == false) {
		Hovered = true;
		NativeOnUpdateInputMode();
	}
}

void UUIEditorRootWidget::PanelMouseLeave() {
	if (Hovered == true) {
		Hovered = false;
		NativeOnUpdateInputMode();
	}
}

bool UUIEditorRootWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) {
	if (Hovered) {
		return false;
	}

	auto* Operation = Cast<UUIEditorAssetBrowserDragDropOperation>(InOperation);
	if (!Operation) {
		return false;
	}

	GET_MODULE_SUBSYSTEM(EditorSubsystem, GameFramework, Editor);
	if (EditorSubsystem) {
		auto* EditorComponent = EditorSubsystem->GetEditorComponent();
		if (EditorComponent) {
			const FVector2d ScreenPosition = InDragDropEvent.GetScreenSpacePosition() - FVector2d(InGeometry.AbsolutePosition);
			EditorComponent->Owner_OnDrop(Operation->ClassMetadata, ScreenPosition);
		}
	}

	return Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
}

void UUIEditorRootWidget::NativeOnShow() {
	SetVisibility(ESlateVisibility::Visible);
	if (AssetBrowserWidget) {
		AssetBrowserWidget->Refresh();
	}
}

void UUIEditorRootWidget::NativeOnUpdateInputMode() {
	if (Hovered) {
		SetInputModeUiOnly();
	} else {
		SetInputModeGameAndUi();
	}
}
