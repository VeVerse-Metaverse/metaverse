// Authors: Egor A. Pristavka, Nikolay Bulatov. 
// Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once

#include "UI/UIPlaceablePropertiesWidget.h"
#include "UIWidgetBase.h"
#include "Components/UIInputWidget.h"
#include "VePortalMetadata.h"
#include "UIPortalPropertiesWidget.generated.h"

/**
 * 
 */
UCLASS(HideDropdown)
class VEGAME_API UUIPortalPropertiesWidget : public UUIPlaceablePropertiesWidget {
	GENERATED_BODY()

	DECLARE_DELEGATE(FOnRefreshRenderTarget);

public:
#pragma region Widgets
	
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UUIInputWidget* NameInputWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UUIInputWidget* WorldInputWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UUIComboBoxSpace* WorldComboBoxWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UUIInputWidget* PortalInputWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UUIComboBoxPortal* PortalComboBoxWidget;
	
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UUIButtonWidget* RefreshRenderTargetButtonWidget;

#pragma endregion Widgets

	FGuid GetPortalId() const { return PortalMetadata.Id; }
	FString GetPortalName() const;
	FGuid GetPortalDestinationId() const;

	FOnRefreshRenderTarget RefreshRenderTargetButtonClicked;
	
	void SetMetadata(const FVePortalMetadata& InMetadata);

protected:
	virtual void NativeOnInitialized() override;

	FVePortalMetadata PortalMetadata;
	
	void ShowWorlds(const FString& Query, const FGuid SelectedWorldId = FGuid());
	void ShowPortals(const FGuid& WorldId, const FString& Query, const FGuid SelectedPortalId = FGuid());

	void HideSpaces();
	void HidePortals();

	FString MetaverseFilter;
	FString SpaceFilter;
	FString PortalFilter;

	void UpdateSaveEnabled();

	UFUNCTION()
	void SpaceComboBox_OnSelectionChanged(FVeWorldMetadata SelectedItem, ESelectInfo::Type SelectionType);

	UFUNCTION()
	void PortalComboBox_OnSelectionChanged(FVePortalMetadata SelectedItem, ESelectInfo::Type SelectionType);

	FGuid DefaultMetaverseId;
	FGuid DefaultWorldId;
	FGuid DefaultPortalId;
};
