// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.

#pragma once

#include "UIWidgetBase.h"
#include "UIPageContentWidget.generated.h"

class UUIAnimationWidget;
class UUIPageManagerWidget;
class UUIPageContentWidget;

USTRUCT()
struct FPageInfo {
	GENERATED_BODY()

	FText Title;
	UClass* Class = nullptr;
	UClass* ParentClass = nullptr;
	TWeakObjectPtr<UUIPageContentWidget> ParentPage;
	FString Options;

	bool IsValid() { return Class != nullptr; }
};

/**
 * 
 */
UCLASS(HideDropdown)
class VEUI_API UUIPageContentWidget : public UUIWidgetBase /*, public INavigatableInterface*/ {
	GENERATED_BODY()

	friend UUIPageManagerWidget;

	DECLARE_MULTICAST_DELEGATE_OneParam(FOnPage, UUIPageContentWidget* /*Page*/);

public:
	UUIPageContentWidget(const FObjectInitializer& ObjectInitializer);
	
	UPROPERTY(EditAnywhere, Category="Page", meta=(DisplayName="Animation"))
	TSubclassOf<UUIAnimationWidget> AnimationClass;

	/** Default parent page class for navigation chain. This class will use when ParentPage is null. */
	UPROPERTY(EditAnywhere, Category="Page", meta=(DisplayName="Parent Page"))
	TSubclassOf<UUIPageContentWidget> ParentPageClass;

	FPageInfo GetInfo() const;

	UFUNCTION(BlueprintPure, Category="Page")
	UUIPageManagerWidget* GetPageManager() const { return PageManager.Get(); }

	UFUNCTION(BlueprintPure, Category="Page")
	const FString& GetOptions() const { return PageOptions; }
	
	UFUNCTION(BlueprintCallable, Category="Page")
	void SetOptions(const FString& InOptions);
	
	UFUNCTION(BlueprintPure, Category="Page")
	FText GetTitle() const { return Title; }
	
	UFUNCTION(BlueprintCallable, Category="Page")
	void SetTitle(const FText& InTitle);

	UFUNCTION(BlueprintPure, Category="Page")
	UUIPageContentWidget* GetParentPage() const { return ParentPage.Get(); }

	UFUNCTION(BlueprintCallable, Category="Page")
	void SetParentPage(UUIPageContentWidget* InParentPage);
	
	virtual void SetPageManager(UUIPageManagerWidget* InPageManager);

	/** Refresh data of page */
	UFUNCTION(BlueprintCallable, Category="Page")
	void Refresh();
	
	FOnPage& GetOnShow() { return OnShow; }
	FOnPage& GetOnHide() { return OnHide; }
	FOnPage& GetOnInfoChanged() { return OnInfoChanged; }

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	/** Default page title. Use SetTitle function for change value. */
	UPROPERTY(EditAnywhere, Category="Page")
	FText Title;

	UPROPERTY(EditAnywhere, Category="Page")
	bool bNoHistory;	
	
	virtual void NativeOnRefresh();
	virtual void NativeOnShow();
	virtual void NativeOnHide();
	virtual void NativeOnInfoChanged();

private:
	TWeakObjectPtr<UUIPageManagerWidget> PageManager;
	FOnPage OnShow;
	FOnPage OnHide;
	FOnPage OnInfoChanged;
	FOnPage OnRefresh;

	/** Page options for HTTP Requests, update metadata, fill content, load textures, etc... */
	FString PageOptions;

	/** Parent page object. */
	TWeakObjectPtr<UUIPageContentWidget> ParentPage;

};
