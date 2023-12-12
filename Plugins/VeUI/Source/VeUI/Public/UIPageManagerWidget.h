// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.
#pragma once


#include "UIWidgetBase.h"
#include "UIPageManagerWidget.generated.h"

class UUIAnimationWidget;
class UUIPageContentWidget;
struct FPageInfo;


/**
 * 
 */
UCLASS()
class VEUI_API UUIPageManagerWidget : public UUIWidgetBase {
	GENERATED_BODY()

	DECLARE_MULTICAST_DELEGATE_OneParam(FOnPage, UUIPageContentWidget* /*Page*/);
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnPageManager, UUIPageManagerWidget* /*PageManager*/);

	enum class ENavigationStatus : uint8 {
		None,
		Next,
		Previous
	};

public:
	UUIPageManagerWidget(const FObjectInitializer& ObjectInitializer);

#pragma region Fields

	/** Container for child animation widget. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UOverlay* ContainerWidget;

	UPROPERTY(EditAnywhere, Category="Page", meta=(DisplayName="Default Animation"))
	TSubclassOf<UUIAnimationWidget> DefaultAnimationClass;

	UPROPERTY(EditAnywhere, Category="Page", meta=(DisplayName="Home Page"))
	TSubclassOf<UUIPageContentWidget> HomePageClass;

	UPROPERTY(EditAnywhere, Category="Page")
	bool bAutoOpenHomePage = true;

#pragma endregion Fields

	/** Create and show page. */
	UFUNCTION(BlueprintCallable, Category="Widget")
	UUIPageContentWidget* OpenPage(TSubclassOf<UUIPageContentWidget> Class, const FString& Options = TEXT(""));

	template <class T>
	T* OpenPage(TSubclassOf<UUIPageContentWidget> Class, const FString& Options = TEXT("")) {
		return Cast<T>(OpenPage(Class, Options));
	}

	/** Open home page from property */
	UFUNCTION(BlueprintCallable, Category="Widget")
	UUIPageContentWidget* OpenHomePage(const FString& Options = TEXT(""));

	template <class T>
	T* OpenHomePage(const FString& Options = TEXT("")) {
		return Cast<T>(OpenHomePage(Options));
	}

	UFUNCTION(BlueprintCallable, Category="Widget")
	bool CanShowFirstPage() const;
	
	/** Show first page from history */
	UFUNCTION(BlueprintCallable, Category="Widget")
	UUIPageContentWidget* ShowFirstPage(bool Refresh = false);

	UFUNCTION(BlueprintCallable, Category="Widget")
	bool CanShowNextPage() const;

	/** Show next page from history */
	UFUNCTION(BlueprintCallable, Category="Widget")
	UUIPageContentWidget* ShowNextPage(bool Refresh = false);

	UFUNCTION(BlueprintCallable, Category="Widget")
	bool CanShowPrevPage() const;
	
	/** Show previous page from history */
	UFUNCTION(BlueprintCallable, Category="Widget")
	UUIPageContentWidget* ShowPrevPage(bool Refresh = false);

	/** Remove page from history */
	UFUNCTION(BlueprintCallable, Category="Widget")
	bool RemovePage(UUIPageContentWidget* Page);

	/** Close all pages and remove from history */
	UFUNCTION(BlueprintCallable, Category="Widget")
	void CloseAllPages();

	/** Show page and add to history */
	UFUNCTION(BlueprintCallable, Category="Widget")
	void ShowPage(UUIPageContentWidget* NewPage, bool bAddToHistory = true);

	/** Hide current page */
	UFUNCTION(BlueprintCallable, Category="Widget")
	void HidePage();

	/** Get current page */
	UUIPageContentWidget* GetCurrentPage() const;

	int32 GetHistoryIndex() const { return HistoryCurrentIndex; }
	int32 GetHistoryNum() const { return HistoryAnimations.Num(); }

	FOnPage& GetOnPageShow() { return OnPageShow; }
	FOnPage& GetOnPageHide() { return OnPageHide; }
	FOnPageManager& GetOnHistoryChanged() { return OnHistoryChanged; }

protected:
	virtual void NativeOnInitializedShared() override;
	virtual void NativeConstruct() override;

	TWeakObjectPtr<UUIPageContentWidget> QueuePageForShow;
	TWeakObjectPtr<UUIAnimationWidget> CurrentAnimationWidget;

	void ShowAnimationWidget(UUIAnimationWidget* AnimationWidget);

	void CurrentAnimationWidget_OnShowFinishedCallback(UUIAnimationWidget* InPageAnimationWidget);

	virtual void NativeOnPageShow(UUIPageContentWidget* Page);
	virtual void NativeOnPageHide(UUIPageContentWidget* Page);
	virtual void NativeOnHistoryChanged();

	UPROPERTY()
	TArray<UUIAnimationWidget*> HistoryAnimations;

	int32 HistoryCurrentIndex = INDEX_NONE;

private:
	FDelegateHandle CurrentAnimationWidget_OnShowFinishedHandle;
	FOnPage OnPageShow;
	FOnPage OnPageHide;
	FOnPageManager OnHistoryChanged;
	bool PlayAnimationPage = false;

};
