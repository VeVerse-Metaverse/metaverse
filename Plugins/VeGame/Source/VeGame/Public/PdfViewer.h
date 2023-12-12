// Author: Egor A. Pristavka. 
// Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#pragma once

#include <PlaceableObject.h>

#include "Components/WidgetComponent.h"
#include "PdfViewer.generated.h"

UCLASS(Blueprintable, BlueprintType)
class VEGAME_API APdfViewer : public APlaceableObject {
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	APdfViewer();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FString> PageUrls;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing="OnRep_CurrentPageUrl")
	FString CurrentPageUrl;

	UFUNCTION()
	void OnRep_CurrentPageUrl();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 CurrentPage = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UWidgetComponent* WidgetComponent = nullptr;

	virtual void OnPlaceableMetadataChanged(const FVePlaceableMetadata& InMetadata) override;

	UFUNCTION(BlueprintCallable, BlueprintCosmetic)
	void Update();

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void Authority_FirstPage();

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void Authority_NextPage();

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void Authority_PreviousPage();

	virtual void ClientInputBegin_Implementation(UInteractionSubjectComponent* Subject, UInteractionObjectComponent* Object, const FName& Name, const FVector& Origin,
		const FVector& Location, const FVector& Normal) override;

	virtual void ServerEvent_Implementation(UInteractionSubjectComponent* Subject, UInteractionObjectComponent* Object, const FName& Name, const TArray<uint8>& Args) override;
};
