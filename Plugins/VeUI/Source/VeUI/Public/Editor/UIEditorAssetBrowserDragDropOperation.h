// 

#pragma once

#include "Blueprint/DragDropOperation.h"
#include "VePlaceableMetadata.h"
#include "UIEditorAssetBrowserDragDropOperation.generated.h"

/**
 * 
 */
UCLASS(HideDropdown)
class VEUI_API UUIEditorAssetBrowserDragDropOperation : public UDragDropOperation {
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Metaverse")
	FVector2D DragOffset;

	/** Metadata of the dragged object class. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Metaverse")
	FVePlaceableClassMetadata ClassMetadata;
};
