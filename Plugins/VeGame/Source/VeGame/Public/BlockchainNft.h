// Author: Egor A. Pristavka. Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#pragma once

#include "BlockchainRequest.h"
#include "UIBlockchainNftWidget.h"
#include "Actors/InteractiveActor.h"
#include "Components/WidgetComponent.h"
#include "BlockchainNft.generated.h"

UENUM(BlueprintType)
enum class EBlockchainNftOwnership : uint8 {
	None,
	Self,
	Other
};

UCLASS(BlueprintType, Blueprintable)
class VEGAME_API ABlockchainNft : public AInteractiveActor {
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	ABlockchainNft();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	/** Id of the linked token */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 TokenId;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString OwnerAddress;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString Name;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString Description;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString ImageUrl;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UWidgetComponent* WidgetComponent;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSubclassOf<UUIBlockchainNftWidget> WidgetClass;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UUIBlockchainNftWidget* Widget = nullptr;

	UPROPERTY(VisibleAnywhere)
	FString OwnerName;

	UFUNCTION(BlueprintCallable)
	void ShowWidgetComponent() const;

	UFUNCTION(BlueprintCallable)
	void HideWidgetComponent() const;

	UFUNCTION(BlueprintCallable)
	void RequestMetadata();

	/** Requests the owner public address */
	bool RequestOwnerAddress();

	/** Requests the token metadata */
	bool RequestTokenMetadata();

	void SetOwnerAddress(const FString& InOwnerAddress);
	void SetOwnerName(const FString& InOwnerName);
	void SetName(const FString& InName);
	void SetDescription(const FString& InDescription);
	void SetImageUrl(const FString& InImageUrl) const;

	UFUNCTION(BlueprintImplementableEvent)
	void OnOwnerAddressChanged();

	UFUNCTION(BlueprintImplementableEvent)
	void OnMetadataChanged();

	UFUNCTION(BlueprintCallable)
	EBlockchainNftOwnership GetOwnership() const;

	/** Callback for successful metadata request */
	void OnMetadataReceived(const FBlockchainTokenMetadata& InMetadata);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bReadyForMint = false;

	void SetReadyForMint(bool bInReadyForMint);
};
