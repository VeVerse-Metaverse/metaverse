// Author: Egor A. Pristavka
// Copyright (c) 2022 LE7EL AS. All right reserved.

// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.

#pragma once

#include "UIButtonWidget.h"
#include "UIWidgetBase.h"

#include "UICommonPaginationWidget.generated.h"

/**
 * 
 */
UCLASS()
class VEUI_API UUICommonPaginationWidget final : public UUIWidgetBase
 {
	GENERATED_BODY()

protected:
	//virtual void NativeConstruct() override;

	void UpdateCurrentPageText() const;

	virtual void RegisterCallbacks() override;
	virtual void UnregisterCallbacks() override;

	FDelegateHandle OnFirstPageButtonClickedDelegateHandle;
	FDelegateHandle OnLastPageButtonClickedDelegateHandle;
	FDelegateHandle OnPreviousPageButtonClickedDelegateHandle;
	FDelegateHandle OnNextPageButtonClickedDelegateHandle;

public:
	UPROPERTY(EditAnywhere)
	bool ShowInfo = true;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIButtonWidget* FirstPageButtonWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIButtonWidget* LastPageButtonWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIButtonWidget* PreviousPageButtonWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIButtonWidget* NextPageButtonWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UTextBlock* CurrentPageTextWidget;

	DECLARE_EVENT_TwoParams(UUIPaginationWidget, FOnPageChanged, int32, int32);

	FOnPageChanged OnPageChanged;

	/** Is the total pages unknown or not significant and we should just allow moving forward indefinitely */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bUnlimited = false;

	UFUNCTION(BlueprintCallable)
	void SetUnlimited(bool bIsUnlimited);

	int32 CurrentPage = 0;

	int32 TotalPages = 0;

	UPROPERTY(EditAnywhere)
	int32 ItemsPerPage = 12;

	int32 TotalElements = 0;

	void ResetPagination();
	void SetPaginationOptions(int32 InTotalElements, int32 InItemsPerPage);
};
