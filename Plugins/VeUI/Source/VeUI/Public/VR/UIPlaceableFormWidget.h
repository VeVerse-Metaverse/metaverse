// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.
#pragma once

#include "UIPlaceableClassWidget.h"
#include "UIWidgetBase.h"
#include "Structs/PlaceableAssetMetadata.h"
#include "Structs/PlaceableFileMetadata.h"
#include "Blueprint/UserWidget.h"
#include "UIPlaceableFormWidget.generated.h"

class UUITabBoxWidget;
class UUIObjectListPageWidget;
class UUIOpenSeaListPageWidget;
class UUIButtonWidget;
class UUIPlaceableUrlWidget;
class UUIOpenSeaAssetListWidget;
class UUIObjectListWidget;

UCLASS(HideDropdown)
class VEUI_API UUIPlaceableFormWidget : public UUIWidgetBase {
	GENERATED_BODY()

	DECLARE_EVENT_OneParam(UUIPlaceableFormWidget, FOnPlaceableFormAssetSelected, const FPlaceableAssetMetadata&);
	DECLARE_EVENT_OneParam(UUIPlaceableFormWidget, FOnPlaceableFormFileSelected, const FPlaceableFileMetadata&);

public:
	/** Getter for the asset selected event delegate. */
	FOnPlaceableFormAssetSelected& GetOnPlaceableFormAssetSelected() { return OnPlaceableFormAssetSelected; }

	/** Getter for the file selected event delegate. */
	FOnPlaceableFormFileSelected& GetOnPlaceableFormFileSelected() { return OnPlaceableFormFileSelected; }

#pragma region Widgets
	
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIButtonWidget* ObjectButtonWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UUIPageManagerWidget* PageManagerWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIButtonWidget* OpenSeaAssetButtonWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIButtonWidget* PlaceableUrlButtonWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIButtonWidget* PlaceableClassButtonWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIButtonWidget* PlaceableFileButtonWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIButtonWidget* CloseButtonWidget = nullptr;

#pragma endregion Widgets

	DECLARE_EVENT(UUIPlaceableFormWidget, FOnClosed);

	FOnClosed OnClosedDelegate;
	FOnClosed& GetOnPlaceableFormClosed() { return OnClosedDelegate; }

protected:
	/** List of object. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content")
	UUIObjectListPageWidget* ObjectListWidget = nullptr;

	/** List of NFTs. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content")
	UUIOpenSeaListPageWidget* OpenSeaAssetListWidget = nullptr;

	/** Url input form. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content")
	UUIPlaceableUrlWidget* PlaceableUrlWidget = nullptr;

	/** Class browser form. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content")
	UUIPlaceableClassWidget* PlaceableClassWidget = nullptr;

	/** Class browser form. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content")
	class UUIPlaceableFilePageWidget* PlaceableFilePage = nullptr;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UUIObjectListPageWidget> ObjectListWidgetClass = nullptr;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UUIOpenSeaListPageWidget> OpenSeaAssetListWidgetClass = nullptr;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UUIPlaceableUrlWidget> PlaceableUrlWidgetClass = nullptr;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UUIPlaceableClassWidget> PlaceableClassWidgetClass = nullptr;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UUIPlaceableFilePageWidget> PlaceableFilePageClass = nullptr;

protected:
	/** Asset selected event. */
	FOnPlaceableFormAssetSelected OnPlaceableFormAssetSelected;
	/** File selected event. */
	FOnPlaceableFormFileSelected OnPlaceableFormFileSelected;

	virtual void NativeOnInitialized() override;

	// todo: move to static subsystem or helper library
	static FString GetMediaType(const FString& MimeType, const FString& Url = TEXT(""));

	/** Opens the object tab. */
	void SwitchToObjectForm();

	/** Opens the open sea tab. */
	void SwitchToOpenSeaForm();

	/** Opens the url tab. */
	void SwitchToUrlForm();

	/** Opens the url tab. */
	void SwitchToClassForm();

	/** Opens the file tab . */
	void SwitchToPlaceableFileForm();

	/** Close the form, switch to the game input mode and notify. */
	void Close();

	/** Select the asset and notify others. */
	void SelectAssetAndClose(const FPlaceableAssetMetadata& InMetadata);
	/** Select the file and notify others. */
	void SelectFileAndClose(const FPlaceableFileMetadata& InMetadata);

};
