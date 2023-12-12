// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.
#pragma once

#include "UIPageContentWidget.h"
#include "ApiPersonaMetadataObject.h"
#include "UIAvatarPageWidget.generated.h"

/**
 * 
 */
UCLASS(HideDropdown)
class VEUI_API UUIAvatarPageWidget final : public UUIPageContentWidget {
	GENERATED_BODY()

public:
#pragma region Widgets

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UTileView* ContainerWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UUICommonPaginationWidget* PaginationWidget;
	
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptioonal))
	class UUILoadingWidget* LoadingWidget;

#pragma endregion Widgets

#pragma region Buttons

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UUIButtonWidget* CreateButtonWidget;

#pragma endregion Buttons

	UPROPERTY(EditAnywhere, Category="Page|SubPages", meta=(DisplayName="Form Page"))
	TSubclassOf<class UUIAvatarFormPageWidget> FormPageClass;

	UPROPERTY(EditAnywhere, meta=(DisplayName="Form Page"))
	UTextureRenderTarget2D* PersonaPreviewRenderTarget;

	UPROPERTY(EditAnywhere)
	int32 ItemsPerPage = 12;
	
protected:
	virtual void NativeOnInitialized() override;
	virtual void RegisterCallbacks() override;
	virtual void UnregisterCallbacks() override;
	virtual void NativeOnRefresh() override;

	/** Request metadata for PersonaList */
	IApiBatchQueryRequestMetadata RequestMetadata;

	/** Change on ListItem Click */
	FGuid SelectedPersonaId;

	void GetRPMPersonasRequest(const IApiBatchQueryRequestMetadata& InRequestMetadata);
	void OnGetRPMPersonasResponse(const FApiPersonaBatchMetadata& InMetadata, bool bSuccessful, const FString& Error);
	void SetRPMPersonas(const FApiPersonaBatchMetadata& BatchMetadata);

	/** Renders and saves persona preview to the cloud */
	void RenderAndSavePersonaPreview(const FGuid& PersonaId);

	void OnPersonaListItemClicked(const FGuid& PersonaId);
	void OnNotifyPersonaImageUpdated(const FGuid& PersonaId, const bool bInSuccessful, const FString& InError);
	void SetIsProcessing(bool bInIsLoading) const;

	bool CheckAvatarImagePreview(const FGuid& PersonaId);

private:
	FDelegateHandle OnCreateButtonClickedDelegateHandle;
	FDelegateHandle OnObjectListItemClickedDelegateHandle;
	FDelegateHandle OnNotifyPersonaCreatedDelegateHandle;
	FDelegateHandle OnNotifyPersonaSelectedDelegateHandle;
	FDelegateHandle OnNotifyPersonaLoadedDelegateHandle;
	FDelegateHandle OnPaginationPageChangeDelegateHandle;

	FGuid CurrentUserId;
};
