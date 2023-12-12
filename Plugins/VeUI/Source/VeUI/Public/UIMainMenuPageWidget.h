// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.
#pragma once

#include "UIPageContentWidget.h"
#include "UIMainMenuPageWidget.generated.h"

class UUIButtonWidget;

/**
 * 
 */
UCLASS(HideDropdown)
class VEUI_API UUIMainMenuPageWidget : public UUIPageContentWidget {
	GENERATED_BODY()

public:
	/** Open Home level */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIButtonWidget* HomeButtonWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIButtonWidget* UsersButtonWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIButtonWidget* ServersButtonWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIButtonWidget* PackagesButtonWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIButtonWidget* WorldsButtonWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIButtonWidget* EventsButtonWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIButtonWidget* ArtheonMenuButtonWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIButtonWidget* AvatarButtonWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIButtonWidget* SettingsButtonWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIButtonWidget* AboutButtonWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIButtonWidget* LogoutButtonWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIButtonWidget* ExitButtonWidget;
	
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIButtonWidget* ObjectsButtonWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIButtonWidget* OpenSeaNftButtonWidget;
	
	UPROPERTY(EditAnywhere, Category="Page|SubPages", meta=(DisplayName="Users Page"))
	TSubclassOf<UUIPageContentWidget> UsersPageClass;

	UPROPERTY(EditAnywhere, Category="Page|SubPages", meta=(DisplayName="Packages Page"))
	TSubclassOf<UUIPageContentWidget> PackagesPageClass;

	UPROPERTY(EditAnywhere, Category="Page|SubPages", meta=(DisplayName="Worlds Page"))
	TSubclassOf<UUIPageContentWidget> WorldsPageClass;

	UPROPERTY(EditAnywhere, Category="Page|SubPages", meta=(DisplayName="Events Page"))
	TSubclassOf<UUIPageContentWidget> EventsPageClass;

	UPROPERTY(EditAnywhere, Category="Page|SubPages", meta=(DisplayName="Servers Page"))
	TSubclassOf<UUIPageContentWidget> ServersPageClass;

	UPROPERTY(EditAnywhere, Category="Page|SubPages", meta=(DisplayName="Artheon Menu Page"))
	TSubclassOf<UUIPageContentWidget> ArtheonMenuPageClass;

	UPROPERTY(EditAnywhere, Category="Page|SubPages", meta=(DisplayName="Objects Page"))
	TSubclassOf<UUIPageContentWidget> ObjectsPageClass;

	UPROPERTY(EditAnywhere, Category="Page|SubPages", meta=(DisplayName="OpenSea NFTs Page"))
	TSubclassOf<UUIPageContentWidget> OpenSeaNftPageClass;
	
	UPROPERTY(EditAnywhere, Category="Page|SubPages", meta=(DisplayName="Avatar Page"))
	TSubclassOf<UUIPageContentWidget> AvatarPageClass;

	UPROPERTY(EditAnywhere, Category="Page|SubPages", meta=(DisplayName="Settings Page"))
	TSubclassOf<UUIPageContentWidget> SettingsPageClass;

	UPROPERTY(EditAnywhere, Category="Page|SubPages", meta=(DisplayName="About Page"))
	TSubclassOf<UUIPageContentWidget> AboutPageClass;

protected:
	void NativeOnInitialized() override;
};
