#pragma once

#include "Components/UIImageWidget.h"
#include "UIImageButtonWidget.generated.h"


UCLASS()
class VEUI_API UUIImageButtonWidget : public UUIImageWidget {
	GENERATED_BODY()

	DECLARE_EVENT(UUIImageButtonWidget, FOnUIImageButtonWidgetEvent);

public:
#pragma region Widgets

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UButton* PreviewImageButtonWidget;

#pragma endregion Widgets

	UPROPERTY(EditAnywhere, Category="UIImage")
	bool IsClickable;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="UIImage")
	USoundBase* PressedSound;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="UIImage")
	USoundBase* HoveredSound;

	FOnUIImageButtonWidgetEvent OnClicked;
	
protected:
	virtual void NativeOnInitializedShared() override;
	virtual void SynchronizeProperties() override;

	UFUNCTION()
	virtual void NativeOnClicked();
	
};