// Authors: Egor A. Pristavka, Nikolay Bulatov. 
// Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once

#include "InputAction.h"
#include "PlayerComponent.h"
#include "PlaceableFilePaginationComponent.generated.h"

class UInteractionObjectComponent;
class UInteractionSubjectComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class VEGAME_API UPlaceableFilePaginationComponent : public UPlayerComponent {
	GENERATED_BODY()

public:
	UPlaceableFilePaginationComponent();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

#pragma region Navigation

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPageChanged);

	FOnPageChanged OnPageChanged;

	virtual void BeginPlay() override;

	/** List of all page URLs. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Placeable|Pdf", meta=(DisplayPriority=0))
	TArray<FString> PageUrls;

	/** Current page index. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Placeable|Pdf")
	int32 CurrentPage = 0;

	/** Current page URL. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing="OnRep_CurrentPageUrl", Category="Placeable|Pdf")
	FString CurrentPageUrl;

	/** Called when current page URL has changed. */
	UFUNCTION()
	void OnRep_CurrentPageUrl() const;

	/** Go to the first page. */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void Authority_FirstPage();

	/** Go to the next page. */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void Authority_NextPage();

	/** Go to the previous page. */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void Authority_PreviousPage();

	/** Process navigation inputs at the client. */
	void ClientInputBegin(UInteractionSubjectComponent* Subject, UInteractionObjectComponent* Object, const FName& Name,
		UInputAction* Action, const FVector& Origin, const FVector& Location, const FVector& Normal);

	/** Process navigation events at the server. */
	void ServerEvent(UInteractionSubjectComponent* Subject, UInteractionObjectComponent* Object, const FName& Name, const TArray<uint8>& Args);

#pragma endregion

};
