// Author: Egor A. Pristavka
// Copyright (c) 2022 LE7EL AS. All right reserved.

#pragma once

#include "UIWidgetBase.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "VePlaceableMetadata.h"
#include "UIEditorAssetBrowserItemWidget.generated.h"

class UUIEditorAssetBrowserItemDragWidget;
/**
 * 
 */
UCLASS()
class VEUI_API UUIEditorAssetBrowserItemWidget : public UUIWidgetBase, public IUserObjectListEntry {
	GENERATED_BODY()

public:
#pragma region Widgets

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	class UUIImageWidget* PreviewImageWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	class UTextBlock* NameTextWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	class UTextBlock* CategoryTextWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	class UTextBlock* DescriptionTextWidget;

#pragma endregion Widgets
	
protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UUIEditorAssetBrowserItemDragWidget> DragWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVePlaceableClassMetadata PlaceableClassMetadata;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FVector2D DragOffset;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Drag")
	TObjectPtr<UUIEditorAssetBrowserItemDragWidget> DragWidget;

	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;

	FDelegateHandle OnConfirmButtonWidgetClickedDelegateHandle;
	FDelegateHandle OnConfirmButtonClickedDelegateHandle;

};
