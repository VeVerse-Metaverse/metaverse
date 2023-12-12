// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.

#pragma once
#include "UITabButtonWidget.h"
#include "UIWidgetBase.h"


#include "UITabBoxWidget.generated.h"

UCLASS()
class UUITabBoxWidget final : public UUIWidgetBase
 {
	GENERATED_BODY()
	
	virtual void NativePreConstruct() override;

	virtual void NativeConstruct() override;
	
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Widget|Content", meta=(InstanceEditable))
	TArray<FString> TabNames;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Widget|Content", meta=(InstanceEditable))
	int ActiveIndex;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="Widget|Content")
	TSubclassOf<UUITabButtonWidget> TabButtonClass;

	FOnTabSelectedEvent OnTabSelectedEvent;
	
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UHorizontalBox* HorizontalBox = nullptr;

protected:
	UPROPERTY()
	TArray<UUITabButtonWidget*> Tabs;

	UFUNCTION()
	void OnTabSelectedCallback(int Index, FString TabName);
};
