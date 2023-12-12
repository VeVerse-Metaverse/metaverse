// 

#pragma once

#include "Blueprint/UserWidget.h"
#include "UITextPropertyWidget.generated.h"

/**
 * 
 */
UCLASS()
class VEUI_API UUITextPropertyWidget : public UUserWidget {
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Title;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Value;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TitleSize = 0.3;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool HideIfValueEmpty = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSlateFontInfo Font;

	void SetValue(const FText& InTitle, const FText& InValue);
	void SetTitleSize(float InValue);
	void SetVisible(bool InValue);

protected:
#pragma region Widgets

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UTextBlock* TitleTextWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UTextBlock* ValueTextWidget;

#pragma endregion Widgets

	virtual void NativeOnInitialized() override;
	virtual void SynchronizeProperties() override;

};
