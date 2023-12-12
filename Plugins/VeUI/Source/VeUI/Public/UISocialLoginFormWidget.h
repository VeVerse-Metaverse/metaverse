// Author: Egor Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.

// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.

#include "UIPageContentWidget.h"
#include "UISocialLoginFormWidget.generated.h"

/**
 * 
 */
UCLASS(HideDropdown)
class VEUI_API UUISocialLoginFormWidget : public UUIPageContentWidget
{
	GENERATED_BODY()

public:
#pragma region Widgets
	
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UUIFormInputWidget* EmailInputWidget;
	
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UUIButtonWidget* LoginButtonWidget;

#pragma endregion Widgets

protected:
	virtual void NativeOnInitialized() override;

	
};
