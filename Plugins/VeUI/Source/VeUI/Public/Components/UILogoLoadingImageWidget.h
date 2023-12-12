#pragma once
#include "UIWidgetBase.h"
#include "UILogoLoadingImageWidget.generated.h"

UCLASS(BlueprintType, Blueprintable, meta=(DisplayName="Logo Image Widget"))
class UUILogoLoadingImageWidget : public UUIWidgetBase {
	GENERATED_BODY()

public:
#pragma region Widgets

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UUIImageWidget* ImageWidget;

#pragma endregion Widgets

protected:
	virtual void NativeConstruct() override;
};
