#include "Editor/UIEditorAssetBrowserItemWidget.h"

#include "Api2PlaceableMetadataObject.h"
#include "Blueprint/DragDropOperation.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Editor/UIEditorAssetBrowserDragDropOperation.h"
#include "Editor/UIEditorAssetBrowserItemDragWidget.h"
#include "Components/UIButtonWidget.h"
#include "Components/TextBlock.h"
#include "Components/UIImageWidget.h"

void UUIEditorAssetBrowserItemWidget::NativeConstruct() {
	Super::NativeConstruct();
}

void UUIEditorAssetBrowserItemWidget::NativeDestruct() {
	Super::NativeDestruct();
}

void UUIEditorAssetBrowserItemWidget::NativeOnListItemObjectSet(UObject* ListItemObject) {
	if (const auto* ApiMetadataWrapper = Cast<UApi2PlaceableClassMetadataObject>(ListItemObject); IsValid(ApiMetadataWrapper)) {
		if (const auto ApiMetadataPtr = ApiMetadataWrapper->Metadata) {
			PlaceableClassMetadata = FVePlaceableClassMetadata(*ApiMetadataPtr);

			NameTextWidget->SetText(FText::FromString(PlaceableClassMetadata.Name));
			DescriptionTextWidget->SetText(FText::FromString(PlaceableClassMetadata.Description));
			CategoryTextWidget->SetText(FText::FromString(PlaceableClassMetadata.Category));

			if (PreviewImageWidget) {
				const TArray<EApiFileType> Types = {
					EApiFileType::ImagePreview,
					EApiFileType::ImageFull,
				};

				if (const auto* FileMetadata = FindFileMetadata(PlaceableClassMetadata.Files, Types)) {
					PreviewImageWidget->ShowImage(FileMetadata->Url);
				} else {
					PreviewImageWidget->ShowDefaultImage();
				}
			}
		}
	}

	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);
}

FReply UUIEditorAssetBrowserItemWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) {
	// const auto ScreenSpacePosition = InMouseEvent.GetScreenSpacePosition();
	// DragOffset = InGeometry.AbsoluteToLocal(ScreenSpacePosition);
	return UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton).NativeReply;
}

void UUIEditorAssetBrowserItemWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) {
	if (!IsValid(DragWidgetClass)) {
		return;
	}
	DragWidget = CreateWidget<UUIEditorAssetBrowserItemDragWidget>(DragWidgetClass);

	UUIEditorAssetBrowserDragDropOperation* Operation = NewObject<UUIEditorAssetBrowserDragDropOperation>();
	Operation->Pivot = EDragPivot::MouseDown;
	Operation->DefaultDragVisual = DragWidget;
	Operation->ClassMetadata = PlaceableClassMetadata;
	Operation->DragOffset = DragOffset;

	OutOperation = Operation;
}
