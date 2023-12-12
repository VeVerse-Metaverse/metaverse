// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.

#pragma once

#include "ApiPersonaMetadataObject.h"
#include "ApiUserMetadata.h"
#include "ManagedTextureRequester.h"
#include "UIAvatarListItemWidget.h"
#include "UIAvatarLe7elListItemWidget.h"
#include "Components/UICommonPaginationWidget.h"
#include "UIRPMAvatarLinkWidget.h"
#include "UIWidget.h"
#include "Components/Image.h"
#include "Components/TileView.h"
#include "UIAvatarPreviewWidget.generated.h"



/**
 * 
 */
UCLASS()
class VEUI_API UUIAvatarPreviewWidget final : public UUIWidget, public IManagedTextureRequester {
	GENERATED_BODY()

	virtual void RegisterCallbacks() override;
	virtual void UnregisterCallbacks() override;

	virtual void NativeConstruct() override;

	virtual void NativeOnTextureLoaded(const FTextureLoadResult& InResult) override;

	void SetMetadata();


	//void SetIsLoading(bool bInIsLoading) const;

	FDelegateHandle OnCloseButtonWidgetDelegateHandle;
	FDelegateHandle OnFollowButtonWidgetDelegateHandle;
	FDelegateHandle OnCancelButtonClickedDelegateHandle;
	FDelegateHandle OnSubmitButtonClickedDelegateHandle;
	FDelegateHandle OnAddFileLinkWidgetClosedDelegateHandle;
	FDelegateHandle OnEditButtonClickedDelegateHandle;
	FDelegateHandle OnChooseLe7elAvatarButtonClickedClosedDelegateHandle;
	FDelegateHandle OnChooseRPMAvatarButtonClickedClosedDelegateHandle;
	FDelegateHandle OnCreateLe7elButtonClickedClosedDelegateHandle;
	FDelegateHandle OnCreateRPMButtonClickedClosedDelegateHandle;
	FDelegateHandle OnSubmitButtonClickedClosedDelegateHandle;
	FDelegateHandle OnObjectListItemClickedDelegateHandle;
	FDelegateHandle OnLe7elObjectListItemClickedDelegateHandle;
	FDelegateHandle OnPersonaCreateBoundDelegateHandle;
	FDelegateHandle OnLe7elPersonaCreateBoundDelegateHandle;
	FDelegateHandle OnPersonaClickBoundDelegateHandle;
	FDelegateHandle OnLe7elPersonaClickBoundDelegateHandle;
	FDelegateHandle OnNotifyAvatarDelegateHandle;
	FDelegateHandle OnNotifyLe7elAvatarDelegateHandle;
	FDelegateHandle OnPersonaImageUpdatedDelegateHandle;
	FDelegateHandle OnGetCurrenPersonaMetadataDelegateHandle;
	FDelegateHandle OnPaginationPageChangeDelegateHandle;
	FDelegateHandle OnLe7elPaginationPageChangeDelegateHandle;


	void SetIsProcessing(bool bInIsLoading) const;
	
public:
	void Refresh();
	void GetRPMPersonas(const IApiBatchQueryRequestMetadata& RequestMetadata);
	void GetLe7elPersonas(const IApiBatchQueryRequestMetadata& RequestMetadata);
	void SetRPMPersonas(const FApiPersonaBatchMetadata& BatchMetadata);
	void SetLe7elPersonas(const FApiPersonaBatchMetadata& BatchMetadata);
	void OnRPMPersonasLoaded(const FApiPersonaBatchMetadata& InMetadata, bool bSuccessful, const FString& Error);
	void OnLe7elPersonasLoaded(const FApiPersonaBatchMetadata& InMetadata, bool bSuccessful, const FString& Error);
	void SuccessPersonaImageUpdated(const FGuid& PersonaId, const bool bInSuccessful, const FString& InError);

	TSharedPtr<FApiUserMetadata> Metadata;
	FApiUserMetadata UserMetadata;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UTileView* ContainerWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UTileView* Le7elContainerWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UUICommonPaginationWidget* PaginationWidget;
	
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UUICommonPaginationWidget* Le7elPaginationWidget;
	
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UUILoadingWidget* LoadingWidget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Widget|Content")
	TSubclassOf<UUIAvatarListItemWidget> ListItemWidgetClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Widget|Content")
	TSubclassOf<UUIAvatarLe7elListItemWidget> Le7elListItemWidgetClass;

	DECLARE_EVENT_OneParam(UUIAvatarPreviewWidget, FOnPersonaListItemClicked, UApiPersonaMetadataObject*);
	FOnPersonaListItemClicked OnPersonaListItemClicked;

	DECLARE_EVENT_OneParam(UUIAvatarPreviewWidget, FOnLe7elPersonaListItemClicked, UApiPersonaMetadataObject*);
	FOnLe7elPersonaListItemClicked OnLe7elPersonaListItemClicked;

	DECLARE_EVENT_OneParam(UUIAvatarPreviewWidget, FOnObjectSelected, TSharedPtr<FApiPersonaMetadata>);
	FOnObjectSelected OnObjectSelected;
	
	DECLARE_EVENT_OneParam(UUIAvatarPreviewWidget, FOnLe7elObjectSelected, TSharedPtr<FApiPersonaMetadata>);
	FOnLe7elObjectSelected OnLe7elObjectSelected;

	UPROPERTY(EditAnywhere)
	int32 ItemsPerPage = 12;
	
	/** User avatar image widget. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UImage* PreviewImageWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UUIButtonWidget* CloseButtonWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UUIButtonWidget* EditButtonWidget;
	
	// UPROPERTY(BlueprintAssignable)
	// FEditAvatarButtonClicked EditButtonClicked;
	//
	// UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	// UUIButtonWidget* ChooseLe7elAvatarButtonWidget;
	//
	// UPROPERTY(BlueprintAssignable)
	// FChooseLe7elAvatarButtonClicked ChooseLe7elAvatarClicked;
	//
	// UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	// UUIButtonWidget* ChooseRPMAvatarButtonWidget;
	//
	// UPROPERTY(BlueprintAssignable)
	// FChooseRPMAvatarButtonClicked ChooseRPMAvatarClicked;
	//
	// UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	// UUIButtonWidget* CreateLe7elButtonWidget;
	//
	// UPROPERTY(BlueprintAssignable)
	// FCreateLe7elAvatarButtonClicked CreateLe7elAvatarClicked;
	//
	// UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	// UUIButtonWidget* CreateRPMButtonWidget;
	//
	// UPROPERTY(BlueprintAssignable)
	// FCreateRPMAvatarButtonClicked CreateRPMAvatarClicked;

	DECLARE_EVENT(UUIAvatarPreviewWidget, FOnClosed);
	FOnClosed OnClosedDelegate;

	DECLARE_EVENT(UUIAvatarPreviewWidget, FOnSubmit);
	FOnSubmit OnSubmitDelegate;
	
	// UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	// UUIRPMAvatarLinkWidget* RPMAvatarLinkWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UTextBlock* NameTextWidget;

private:
	// bool bRPMAvatarLinkWidgetVisible;
	//
	// UFUNCTION(BlueprintCallable, Category="Widget|Content")
	// void HideRPMAvatarLinkWidget(bool bAnimate = true, bool bPlaySound = true);
	//
	// UFUNCTION(BlueprintCallable, Category="Widget|Content")
	// void ShowRPMAvatarLinkWidget(bool bAnimate = true, bool bPlaySound = true);
	//
	// UFUNCTION(BlueprintCallable, Category="Widget|Content")
	// void NavigateToRPMAvatarLinkWidget(bool bAnimate = true, bool bPlaySound = true);
};
