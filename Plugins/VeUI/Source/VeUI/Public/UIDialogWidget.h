// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.
#pragma once

#include "UIDialogData.h"
#include "UIWidgetBase.h"
#include "UIDialogWidget.generated.h"

class UUIDialogWidget;

/** Dialog widget and clicked button index. */
DECLARE_DELEGATE_TwoParams(FUIDialogButtonClicked, UUIDialogWidget*, const uint8&);

/**
 * 
 */
UCLASS()
class VEUI_API UUIDialogWidget final : public UUIWidgetBase {
	GENERATED_BODY()

public:
#pragma region Widgets
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, meta=(BindWidget))
	class UTextBlock* HeaderWidget;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, meta=(BindWidget))
	class UTextBlock* MessageWidget;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, meta=(BindWidget))
	class UUIButtonWidget* FirstButtonWidget;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, meta=(BindWidget))
	class UUIButtonWidget* SecondButtonWidget;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, meta=(BindWidget))
	class UUIButtonWidget* ThirdButtonWidget;

#pragma endregion Widgets
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	FUIDialogData DialogData;
	
	FUIDialogButtonClicked OnButtonClicked;

	void Open(const FUIDialogData& InDialogData, const FUIDialogButtonClicked& InOnButtonClicked);
	void Close();

	void HideAndDestroy();

protected:
	virtual void NativeOnInitializedShared() override;

private:
	void UpdateButtons() const;
	void HideButtons() const;
};
