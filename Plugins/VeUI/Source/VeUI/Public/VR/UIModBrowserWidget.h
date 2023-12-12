// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.
#pragma once

#include "UINavigatableWidget.h"
#include "Components/UIButtonWidget.h"
#include "UIModBrowserWidget.generated.h"

class UUIModListWidget;
class UUIMetaverseFilterWidget;
class UUIModDetailWidget;

UCLASS()
class VEUI_API UUIModBrowserWidget final : public UUINavigatableWidget {
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	virtual void RegisterCallbacks() override;
	virtual void UnregisterCallbacks() override;

	virtual void OnNavigate() override;

	// FDelegateHandle OnModBatchLoadedDelegateHandle;
	// FDelegateHandle OnModListItemClickedDelegateHandle;
	FDelegateHandle OnModDetailWidgetClosedDelegateHandle;
	// FDelegateHandle OnAuthSubsystemUserLoginDelegateHandle;
	FDelegateHandle OnModSelectedDelegateHandle;
	FDelegateHandle OnCloseButtonClickedDelegateHandle;
	

	/** List of mods. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UUIModListWidget* ModListWidget;

	/** Mod details. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIModDetailWidget* ModDetailWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIButtonWidget* CloseButtonWidget;

	DECLARE_EVENT(UUIFileBrowserWidget, FOnClosed);
	FOnClosed OnClosedDelegate;

	//void GetMods(const IApiBatchQueryRequestMetadata& RequestMetadata = IApiBatchQueryRequestMetadata()) const;

#pragma region Navigation
private:
	bool bModListWidgetVisible;
	bool bModDetailWidgetVisible;

	/** Fades in the reset password widget. */
	UFUNCTION(BlueprintCallable, Category="Widget|Content")
	void ShowModListWidget(bool bAnimate = true, bool bPlaySound = true);

	/** Fades in the reset password widget. */
	UFUNCTION(BlueprintCallable, Category="Widget|Content")
	void ShowModDetailWidget(bool bAnimate = true, bool bPlaySound = true);

	/** Fades out the login widget. */
	UFUNCTION(BlueprintCallable, Category="Widget|Content")
	void HideModListWidget(bool bAnimate = true, bool bPlaySound = true);

	/** Fades out the login widget. */
	UFUNCTION(BlueprintCallable, Category="Widget|Content")
	void HideModDetailWidget(bool bAnimate = true, bool bPlaySound = true);

	UFUNCTION(BlueprintCallable, Category="Widget|Content")
	void NavigateToModList(bool bAnimate = true, bool bPlaySound = true);

	UFUNCTION(BlueprintCallable, Category="Widget|Content")
	void NavigateToModDetail(bool bAnimate = true, bool bPlaySound = true);
	
public:
	virtual void Navigate(const FString& Options) override;
	
private:
#pragma endregion
};
