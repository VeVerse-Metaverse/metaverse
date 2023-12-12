
// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.

#pragma once

#include "UIWidgetBase.h"
#include "UICheckBoxWidget.generated.h"


UCLASS()
class VEUI_API UUICheckBoxWidget final : public UUIWidgetBase {
	GENERATED_BODY()

	DECLARE_EVENT_OneParam(UUICheckBoxWidget, FOnUICheckBoxChangedEvent, bool bInChecked);

public:
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UCheckBox* CheckBoxWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UButton* ButtonWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UTextBlock* TextWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Text;
	
	FOnUICheckBoxChangedEvent OnCheckStateChanged;

	bool IsChecked() const;
	void SetIsChecked(bool InIsChecked);
	
protected:
	virtual void NativeOnInitializedShared() override;
	virtual void SynchronizeProperties() override;

	UFUNCTION()
	virtual void NativeOnChanged(bool bInChecked);

private:
	UFUNCTION()
	void TextBlock_OnClicked();
	
};

