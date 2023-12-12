// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.
#pragma once

#include "UIWidgetBase.h"
#include "Components/ScrollBox.h"
#include "UIServerListWidget.generated.h"

/**
 * 
 */
UCLASS()
class VEUI_API UUIServerListWidget final : public UUIWidgetBase
 {
	GENERATED_BODY()

public:
	
	UPROPERTY()
	TSubclassOf<UUIWidgetBase
> ChildWidgetClass;

	UPROPERTY()
	TArray<UUIWidgetBase
*> ChildWidgets;

	UPROPERTY(meta=(BindWidget))
	UScrollBox* ContainerWidget;

	UFUNCTION()
	void AddServer() {
	};

	UFUNCTION()
	void RefreshServerList() {
	};
};
