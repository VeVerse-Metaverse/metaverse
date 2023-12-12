// Author: Egor A. Pristavka
// Copyright (c) 2022 LE7EL AS. All right reserved.

#pragma once

#include "UIBaseRootWidget.h"
#include "UIEditorRootWidget.generated.h"

/**
 * Main HUD widget for the in-game editor mode. Contains editor toolbar, asset browser and selected object properties widgets.
 * Handles drop events initiated by dragging AssetBrowser list item into the screen. 
 */
UCLASS(HideDropdown)
class VEUI_API UUIEditorRootWidget final : public UUIBaseRootWidget {
	GENERATED_BODY()

public:
#pragma region Widgets

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	TObjectPtr<class UUIEditorToolbarWidget> ToolbarWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	TObjectPtr<class UUIEditorDetailsWidget> DetailsWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	TObjectPtr<class UUIEditorAssetBrowserWidget> AssetBrowserWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	TObjectPtr<class UUIButtonWidget> CloseButtonWidget;

#pragma endregion Widgets

	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeOnShow() override;
	virtual void NativeOnUpdateInputMode() override;

	void BindToEditorComponent(class UEditorComponent* NewEditorComponent, class UEditorComponent* OldEditorComponent);

	void PanelMouseEnter();
	void PanelMouseLeave();

private:
	bool Hovered = false;

};
