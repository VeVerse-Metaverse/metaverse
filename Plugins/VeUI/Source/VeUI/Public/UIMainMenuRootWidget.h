// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.
#pragma once


#include "UIBaseRootWidget.h"
#include "VeWorldMetadata.h"
#include "UIMainMenuRootWidget.generated.h"

class UUISocialPanelWidget;
class UUIPagePanelWidget;
class UUIPageManagerWidget;


/**
 * 
 */
UCLASS()
class VEUI_API UUIMainMenuRootWidget final : public UUIBaseRootWidget {
	GENERATED_BODY()

public:
#pragma region Widgets

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category="Widget", meta=(BindWidget))
	UUIPageManagerWidget* PageManagerWidget;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category="Widget", meta=(BindWidget))
	UUIPagePanelWidget* PagePanelWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UUISocialPanelWidget* SocialPanelWidget = nullptr;

#pragma endregion Widgets

	UPROPERTY(EditAnywhere, Category="Page", meta=(DisplayName="World Detail Page"))
	TSubclassOf<class UUISpaceDetailPageWidget> WorldDetailPageClass;

	void ShowMenuWorldDetail(const FGuid& InWorldId);
	
protected:
	virtual void NativeOnInitialized() override;

};
