// Author: Egor A. Pristavka. 
// Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.


#pragma once

#include "Actors/InteractiveActor.h"
#include "UIPageContentWidget.h"
#include "VePropertyMetadata.h"
#include "VePlaceableMetadata.h"
#include "PlaceableObject.generated.h"

UCLASS()
class VEGAME_API APlaceableObject : public AInteractiveActor {
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	APlaceableObject();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;


	//==========================================================================

public:
#pragma region Interaction

	virtual void ClientInputBegin_Implementation(UInteractionSubjectComponent* Subject, UInteractionObjectComponent* Object, const FName& Name, const FVector& Origin,
		const FVector& Location, const FVector& Normal);

#pragma endregion Interaction

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	TArray<FVePropertyMetadata> GetProperties() const;
	virtual TArray<FVePropertyMetadata> GetProperties_Implementation() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void SetProperties(const TArray<FVePropertyMetadata>& Properties);
	virtual void SetProperties_Implementation(const TArray<FVePropertyMetadata>& Properties);

	UPROPERTY(meta=(DeprecatedProperty))
	TSubclassOf<class UDEPRECATED_UIPlaceableDialogWidget> PlaceableDialogClass_DEPRECATED;

	UPROPERTY(meta=(DeprecatedProperty))
	TSubclassOf<class UDEPRECATED_UIPlaceablePropertiesWidgetOLD> PropertiesWidgetClass_DEPRECATED;

	/** Set placeable metadata and request the object metadata if required. */
	void SetPlaceableMetadata(const FVePlaceableMetadata& InPlaceableMetadata);

protected:
	/** Shared placeable metadata. */
	UPROPERTY(ReplicatedUsing=OnRep_PlaceableMetadata, BlueprintReadOnly)
	FVePlaceableMetadata SharedPlaceableMetadata;

	UFUNCTION()
	void OnRep_PlaceableMetadata(const FVePlaceableMetadata& OldValue);

	virtual void OnPlaceableMetadataChanged(const FVePlaceableMetadata& InMetadata);

	void ShowDialog(UInteractionSubjectComponent* Subject, UInteractionObjectComponent* Object);

	virtual void ServerEvent_Implementation(UInteractionSubjectComponent* Subject, UInteractionObjectComponent* Object, const FName& Name, const TArray<uint8>& Args);

private:
	UPROPERTY(meta=(DeprecatedProperty))
	TObjectPtr<UDEPRECATED_UIPlaceableDialogWidget> PlaceableDialogWidget_DEPRECATED;

};
