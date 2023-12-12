// Author: Khusan Yadgarov
#pragma once

#include "Api2SharedAccessMetadata.h"
#include "UIWidgetBase.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "UISharedAccessListItemWidget.generated.h"


/**
 * 
 */
UCLASS(HideDropdown)
class VEUI_API UUISharedAccessListItemWidget : public UUIWidgetBase, public IUserObjectListEntry {
	GENERATED_BODY()

public:
#pragma region Widgets
	
	/** Displays SharedAccess name. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UTextBlock* NameTextWidget;
	
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UCheckBox* CanViewCheckBoxWidget;
	
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UCheckBox* CanEditCheckBoxWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UCheckBox* CanDeleteCheckBoxWidget;

#pragma endregion Widgets

	UPROPERTY(EditAnywhere)
	UTexture2D* DefaultTexture;

protected:
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;

	FApi2SharedAccessMetadata Metadata;

};
