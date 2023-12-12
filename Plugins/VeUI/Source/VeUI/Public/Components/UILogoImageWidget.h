#pragma once
#include "UIWidgetBase.h"
#include "UILogoImageWidget.generated.h"

UCLASS(BlueprintType, Blueprintable, meta=(DisplayName="Logo Image Widget"))
class UUILogoImageWidget : public UUIWidgetBase {
	GENERATED_BODY()

public:
#pragma region Widgets

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UUIImageWidget* ImageWidget;

#pragma endregion Widgets

	void ShowImage();
	

protected:
	virtual void NativeConstruct() override;
};
