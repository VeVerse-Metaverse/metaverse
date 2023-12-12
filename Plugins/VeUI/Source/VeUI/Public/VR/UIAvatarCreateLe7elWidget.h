// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.

#pragma once

#include "ApiPersonaMetadata.h"
#include "ApiUserMetadata.h"
#include "ManagedTextureRequester.h"
#include "Components/UIInputWidget.h"
#include "UIWidget.h"
#include "UIAvatarCreateLe7elWidget.generated.h"

/**
 * 
 */
UCLASS()
class VEUI_API UUIAvatarCreateLe7elWidget final : public UUIWidget {
	GENERATED_BODY()

	virtual void RegisterCallbacks() override;
	virtual void UnregisterCallbacks() override;

	virtual void NativeConstruct() override;
	
	
	//void SetIsLoading(bool bInIsLoading) const;
	
	FDelegateHandle OnCloseButtonWidgetDelegateHandle;
	FDelegateHandle OnFollowButtonWidgetDelegateHandle;
	FDelegateHandle OnCancelButtonClickedDelegateHandle;
	FDelegateHandle OnSubmitButtonClickedDelegateHandle;
	FDelegateHandle OnCreateButtonClickedDelegateHandle;
	

public:
	
	void SuccessUpdateAvatarFiles(const bool bInSuccessful, const FString& InError) const;
	//void SuccessPostDefaultAvatarFiles(const bool bInSuccessful, const FString& InError) const;
	void SuccessCreatePersona(const FApiPersonaMetadata& InMetadata,bool bInSuccessful, const FString& InError);
	void CreatePersona();
	void UpdateFilePersona(const FApiPersonaMetadata& InMetadata);
	//void SetDefaultPersona(const FApiPersonaMetadata& InMetadata);
	void UpdateAvatarFiles();
	
	FApiUserMetadata Metadata;

	void SetMetadata(const FApiUserMetadata& InMetadata);

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UUIButtonWidget* CloseButtonWidget;
	
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UUIButtonWidget* CreateButtonWidget;
	
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UUIInputWidget* PersonaNameWidget;

	DECLARE_EVENT(UUIUserAddFileLink, FOnClosed);
	FOnClosed OnClosedDelegate;

	DECLARE_EVENT(UUIUserAddFileLink, FOnSubmit);
	FOnSubmit OnSubmitDelegate;
	
	DECLARE_EVENT(UUIUserAddFileLink, FOnCreate);
	FOnCreate OnCreateDelegate;
};

