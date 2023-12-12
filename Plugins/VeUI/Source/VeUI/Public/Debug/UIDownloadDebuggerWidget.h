// Copyright (c) 2022 LE7EL AS. All right reserved.

#pragma once

#include "Components/VerticalBox.h"
#include "UIDownloadDebuggerWidget.generated.h"

class UUIDownloadDebuggerItemWidget;

/**  */
UCLASS(HideDropdown)
class VEUI_API UUIDownloadDebuggerWidget final : public UUserWidget {
	GENERATED_BODY()

public:
#pragma region Widgets

	/** Object preview container. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	TObjectPtr<UVerticalBox> ContainerWidget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Widget|Content")
	TSubclassOf<UUserWidget> ItemWidgetClass;

#pragma endregion

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

};
