// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.
#pragma once
#include "UIWidgetBase.h"

#include "Components/Image.h"

#include "UIProgressWidget.generated.h"

/**
 * 
 */
UCLASS()
class VEUI_API UUIProgressWidget : public UUIWidgetBase
 {
	GENERATED_BODY()

	virtual void NativeConstruct() override;

public:
	/** Update progress ratio. */
	UFUNCTION(BlueprintCallable)
	void SetAlpha(float InAlpha);

	/** Update progress ratio. */
	UFUNCTION(BlueprintCallable)
	void SetProgressRatio(float InRatio);

	/** Progress bar widget. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(BindWidget))
	UImage* ProgressBarWidget;

	/**
	 * Base progress material.
	 * @note Must have a ratio parameter that controls fill ratio.
	 * @note Must have a alpha parameter that controls transparency.
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UMaterialInterface* ProgressMaterial;

	/** Active material instance. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UMaterialInstanceDynamic* ProgressMID;
};
