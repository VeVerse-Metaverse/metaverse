// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.
#pragma once
#include "UIWidgetBase.h"

#include "Components/TextBlock.h"

#include "UIServerListItemWidget.generated.h"

/**
 * 
 */
UCLASS()
class VEUI_API UUIServerListItemWidget final : public UUIWidgetBase
 {
	GENERATED_BODY()

public:
	/** Displays the hosted space name. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UTextBlock* SpaceNameWidget;

	/** Displays the game mode at the server. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UTextBlock* GameModeWidget;

	/** Displays current and max player count. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UTextBlock* PlayerCountWidget;

	/** Displays current and max player count. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UTextBlock* PingWidget;

};
