// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.
#pragma once

#include "UIBreadcrumbTrailWidget.h"
#include "Components/UIButtonWidget.h"
#include "UIWidgetBase.h"
#include "Components/TileView.h"
#include "PlatformFileInfo.h"
#include "UIFileBrowserWidget.generated.h"

/**
 * 
 */
UCLASS()
class VEUI_API UUIFileBrowserWidget final : public UUIWidgetBase
 {
	GENERATED_BODY()

	virtual void NativeConstruct() override;

	virtual void RegisterCallbacks() override;
	virtual void UnregisterCallbacks() override;

	FDelegateHandle OnCloseButtonWidgetDelegateHandle;
	FDelegateHandle OnOpenButtonWidgetDelegateHandle;
	FDelegateHandle OnCancelButtonWidgetDelegateHandle;
	FDelegateHandle OnFileListItemClickedDelegateHandle;
	FDelegateHandle OnListItemDoubleClickDelegateHandle;

	TArray<FString> AllowedExtensions = TArray<FString>{TEXT("jpg"), TEXT("jpeg")};

	void OnListItemIsHoveredChanged(UObject* Item, bool bIsHovered);
	void OnListItemClicked(UObject* InClickedItem);
	void OnListItemDoubleClicked(UObject* InClickedItem);
	
	void OnCloseButtonClicked() const;

	DECLARE_EVENT_OneParam(UUIFileBrowserWidget, FOnBrowseListItemClicked, UPlatformFileInfo*);
	FOnBrowseListItemClicked OnBrowseListItemClicked;

	void OnFileItemClicked(UPlatformFileInfo* InFile);
	void OnOpenButtonClicked();

public:
	void UpdateFileList(const FString& InPath, const TArray<FString>& InAllowedExtensions = {});
	void OnCancelButtonClicked() const;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Widget|Sound")
	USoundBase* ListItemHoverInSound;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Widget|Sound")
	USoundBase* ListItemHoverOutSound;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Widget|Sound")
	USoundBase* ListItemClickSound;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Widget|Sound")
	USoundBase* ListItemDoubleClickSound;
	
	DECLARE_EVENT_OneParam(UUIFileBrowserWidget, FOnFileSelected, const FPlatformFileInfo&);
	FOnFileSelected OnFileSelected;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UTileView* ContainerWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UUIButtonWidget* CloseButtonWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UUIButtonWidget* CancelButtonWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UUIButtonWidget* OpenButtonWidget;
	
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UUIBreadcrumbTrailWidget* BreadcrumbTrailWidget;

	DECLARE_EVENT(UUIFileBrowserWidget, FOnClosed);
	FOnClosed OnClosed;
};
