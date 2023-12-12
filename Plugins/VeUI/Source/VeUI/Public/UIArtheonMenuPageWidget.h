// Author: Egor Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.

// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.

#include "UIPageContentWidget.h"
#include "UIArtheonMenuPageWidget.generated.h"

class UUIButtonWidget;

/**
 * 
 */
UCLASS(HideDropdown)
class VEUI_API UUIArtheonMenuPageWidget : public UUIPageContentWidget {
	GENERATED_BODY()

public:
	/** Open Home level */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIButtonWidget* HomeButtonWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIButtonWidget* ObjectsButtonWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIButtonWidget* OpenSeaNftButtonWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIButtonWidget* MainMenuButtonWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIButtonWidget* LogoutButtonWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIButtonWidget* ExitButtonWidget;

	UPROPERTY(EditAnywhere, Category="Page|SubPages", meta=(DisplayName="Objects Page"))
	TSubclassOf<UUIPageContentWidget> ObjectsPageClass;

	UPROPERTY(EditAnywhere, Category="Page|SubPages", meta=(DisplayName="OpenSea NFTs Page"))
	TSubclassOf<UUIPageContentWidget> OpenSeaNftPageClass;

	UPROPERTY(EditAnywhere, Category="Page|SubPages", meta=(DisplayName="Main Menu Page"))
	TSubclassOf<UUIPageContentWidget> MainMenuPageClass;

protected:
	virtual void RegisterCallbacks() override;
	virtual void UnregisterCallbacks() override;

	FDelegateHandle OnHomeButtonClickedDelegateHandle;
	FDelegateHandle OnLogoutButtonClickedDelegateHandle;
	FDelegateHandle OnExitButtonClickedDelegateHandle;
	FDelegateHandle OnOpenSeaNftButtonClickedDelegateHandle;
	FDelegateHandle OnObjectsButtonClickedDelegateHandle;
	FDelegateHandle OnMainMenuButtonClickedDelegateHandle;
};
