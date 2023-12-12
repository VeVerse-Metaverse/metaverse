// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.

#pragma once
#include "VeShared.h"


class VEUI_API FUIBreadcrumbSubsystem final : public IModuleSubsystem {
public:
	const static  FName Name;

	virtual void Initialize() override;
	virtual void Shutdown() override;

	TArray<FText> GetBreadcrumbs() const;
	void Push(const FText& InBreadcrumb);
	void Pop();
	void Reset();

	DECLARE_EVENT_OneParam(FUIBreadcrumbSubsystem, FOnBreadcrumbPushed, const FText& /* Breadcrumb */);
	FOnBreadcrumbPushed OnBreadcrumbPushed;

	DECLARE_EVENT_OneParam(FUIBreadcrumbSubsystem, FOnBreadcrumbPopped, const FText& /* Breadcrumb */);
	FOnBreadcrumbPopped OnBreadcrumbPopped;

	DECLARE_EVENT(FUIBreadcrumbSubsystem, FOnBreadcrumbReset);
	FOnBreadcrumbReset OnBreadcrumbReset;

private:
	TArray<FText> Breadcrumbs = {};
};

typedef TSharedPtr<FUIBreadcrumbSubsystem> FUIBreadcrumbSubsystemPtr;
