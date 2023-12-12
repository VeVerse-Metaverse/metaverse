// Author: Egor A. Pristavka. Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#pragma once

#include "ManagedTextureRequester.h"
#include "Components/UIButtonWidget.h"
#include "UIWidgetBase.h"
#include "Components/TextBlock.h"
#include "UIBlockchainNftWidget.generated.h"

class ABlockchainNft;
class UImage;

/**
 * 
 */
UCLASS()
class VEGAME_API UUIBlockchainNftWidget : public UUIWidgetBase, public IManagedTextureRequester {
	GENERATED_BODY()

public:
	virtual void RegisterCallbacks() override;
	virtual void UnregisterCallbacks() override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TWeakObjectPtr<ABlockchainNft> BlockchainNftActor;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(BindWidgetOptional))
	UImage* ImageWidget;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(BindWidgetOptional))
	UImage* QrCodeImageWidget;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(BindWidgetOptional))
	UTextBlock* NameTextWidget;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(BindWidgetOptional))
	UTextBlock* DescriptionTextWidget;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(BindWidgetOptional))
	UTextBlock* OwnerTextWidget;

	/** Button to show QR code modal */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(BindWidgetOptional))
	UUIButtonWidget* MintButtonWidget;

	/** Button to close QR code modal */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(BindWidgetOptional))
	UUIButtonWidget* CloseQRCodeButtonWidget;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(BindWidgetOptional))
	UPanelWidget* GeneralInfoWidget;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(BindWidgetOptional))
	UPanelWidget* InfoWidget;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(BindWidgetOptional))
	UPanelWidget* MintWidget;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(BindWidgetOptional))
	UPanelWidget* QrCodeWidget;

	FString OwnerAddress = {};
	FString OwnerName = {};

	virtual void NativeOnTextureLoaded(const FTextureLoadResult& Result) override;

	/** Land parcel mint mode, show the mint button, displayed if the token is not minted yet */
	UFUNCTION(BlueprintCallable)
	void SwitchToMintMode() const;

	/** Hide QR code for minting */
	UFUNCTION(BlueprintCallable)
	void SwitchToMintQrCodeWidget();

	/** Land parcel information mode, displayed if the token is minted */
	UFUNCTION(BlueprintCallable)
	void SwitchToInfoMode() const;

	UFUNCTION(BlueprintCallable)
	void SetName(const FString& InName);

	UFUNCTION(BlueprintCallable)
	void SetDescription(const FString& InDescription);

	UFUNCTION(BlueprintCallable)
	void SetOwnerName(const FString& InOwnerName);

	UFUNCTION(BlueprintCallable)
	void SetOwnerAddress(const FString& InOwnerAddress);

	UFUNCTION(BlueprintCallable)
	void SetImageUrl(const FString& InImageUrl);

	UFUNCTION(BlueprintCallable)
	void UpdateQrCode(const FString& InUrl);

	void OnMintSuccess();

	UFUNCTION(BlueprintImplementableEvent)
	void OnMintSuccessBP();
};
