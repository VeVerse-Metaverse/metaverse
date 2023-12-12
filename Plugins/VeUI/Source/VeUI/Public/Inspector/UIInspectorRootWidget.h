// Copyright (c) 2022 LE7EL AS. All right reserved.

#pragma once

#include "UIBaseRootWidget.h"
#include "UIInspectorRootWidget.generated.h"

class UPlaceableComponent;

/**  */
UCLASS(HideDropdown)
class VEUI_API UUIInspectorRootWidget final : public UUIBaseRootWidget {
	GENERATED_BODY()

public:
#pragma region Widgets

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UUIButtonWidget* CloseButtonWidget;

	/** Object preview container. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UPanelWidget* PreviewContainerWidget;

	/** Properties container. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UScrollBox* PropertyContainerWidget;

#pragma endregion

	bool StartInspection(AActor* InInspectedActor);
	
protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeOnUpdateInputMode() override;

	/** Currently inspected actor. */
	TWeakObjectPtr<AActor> InspectedActor;
	/** Placeable component of the inspected actor. */
	TWeakObjectPtr<UPlaceableComponent> PlaceableComponent;
	/** Widget used to display the main object preview. For 3d objects it can be render target preview, for 2d objects an image. */
	TWeakObjectPtr<UUserWidget> PreviewWidget;
	/** Widget used to display object properties. Usually placed as the right side bar. */
	TWeakObjectPtr<UUserWidget> PropertiesWidget;

	/** Resets widget state. */
	void Clear();
public:
	DECLARE_EVENT(UUIInspectorWidget, FOnClosed);

	FOnClosed OnClosedDelegate;
};
