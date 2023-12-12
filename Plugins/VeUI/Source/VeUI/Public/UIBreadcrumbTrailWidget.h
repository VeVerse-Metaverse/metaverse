// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.
#pragma once

#include "UIBreadcrumbDelimiterWidget.h"
#include "UIBreadcrumbItemWidget.h"
#include "UIWidgetBase.h"

#include "Components/HorizontalBox.h"
#include "UIBreadcrumbTrailWidget.generated.h"

USTRUCT()
struct VEUI_API FBreadcrumbInfo {
	GENERATED_BODY()

	UPROPERTY()
	UUIBreadcrumbItemWidget* TextWidget = nullptr;

	UPROPERTY()
	UUIBreadcrumbDelimiterWidget* DelimiterWidget = nullptr;
};

UCLASS()
class VEUI_API UUIBreadcrumbTrailWidget final : public UUIWidgetBase
 {
	GENERATED_BODY()

	virtual void RegisterCallbacks() override;
	virtual void UnregisterCallbacks() override;

	FDelegateHandle OnBreadcrumbPushedDelegateHandle;
	FDelegateHandle OnBreadcrumbPoppedDelegateHandle;
	FDelegateHandle OnBreadcrumbResetDelegateHandle;

	void OnBreadcrumbPushed(const FText& InBreadcrumbText);
	void OnBreadcrumbPopped(const FText& InBreadcrumbText);
	void OnBreadcrumbReset();

public:
	UPROPERTY(EditAnywhere)
	TSubclassOf<UUIBreadcrumbItemWidget> BreadcrumbItemWidgetClass = UUIBreadcrumbItemWidget::StaticClass();

	UPROPERTY(EditAnywhere)
	TSubclassOf<UUIBreadcrumbDelimiterWidget> BreadcrumbDelimiterWidgetClass = UUIBreadcrumbDelimiterWidget::StaticClass();

	UPROPERTY(EditAnywhere, meta=(BindWidgetOptional))
	UHorizontalBox* CrumbContainer;

	TArray<FBreadcrumbInfo> Crumbs;

	void Reset();
	void PushCrumb(const FText& InText);
	void PopCrumb();
	bool HasCrumbs() const;
};
