// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.


#pragma once

#include "IPlaceable.h"
#include "Components/ActorComponent.h"
#include "PlaceableComponent.generated.h"

UENUM(BlueprintType)
enum class EPlaceableChangeType : uint8 {
	Placed,
	Loaded,
	Updated
};

USTRUCT()
struct FVePlaceableMetadataContainer {
	GENERATED_BODY()

	UPROPERTY()
	FVePlaceableMetadata Metadata;

	UPROPERTY()
	int32 UpdateId = 0;

	void SetMetadata(const FVePlaceableMetadata& InMetadata) {
		Metadata = InMetadata;
		++UpdateId;
	}
};

UCLASS(ClassGroup=(Placeable), meta=(BlueprintSpawnableComponent))
class VEGAMEFRAMEWORK_API UPlaceableComponent final : public UActorComponent {
	GENERATED_BODY()

public:
	UPlaceableComponent();

private:
	UPROPERTY(ReplicatedUsing="OnRep_MetadataContainer")
	FVePlaceableMetadataContainer MetadataContainer;

	UFUNCTION()
	void OnRep_MetadataContainer();

public:
	UFUNCTION(BlueprintPure, Category="Placeable")
	const FVePlaceableMetadata& GetMetadata() const { return MetadataContainer.Metadata; }

	class FOnPlaceableMetadataChanged : public TMulticastDelegate<void(const FVePlaceableMetadata& InMetadata, EPlaceableChangeType InChange)> {
		friend class UPlaceableComponent;
	};

	FOnPlaceableMetadataChanged& GetOnPlaceableMetadataChanged() { return OnPlaceableMetadataChanged; }

	/** Called at the server when user drops a new placeable actor into scene. */
	UFUNCTION()
	void LocalServer_OnPlaced(const FVePlaceableMetadata& InMetadata);

	/** Called at the server when placeable is loaded. */
	UFUNCTION()
	void LocalServer_OnLoaded(const FVePlaceableMetadata& InMetadata);

	/** Called at the server when placeable is loaded. */
	UFUNCTION()
	void LocalServer_OnUpdated(const FVePlaceableMetadata& InMetadata);

	UFUNCTION()
	void OnTransformed(const FTransform& Transform);

	/** Called when object is being selected */
	UFUNCTION()
	void OnSelected();

	/** Called when object is being deselected */
	UFUNCTION()
	void OnDeselected();

	UFUNCTION()
	void OnDestroyed();

private:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	FOnPlaceableMetadataChanged OnPlaceableMetadataChanged;

	void LocalClient_OnPlaced(const FVePlaceableMetadata& InMetadata);
	void LocalClient_OnUpdated(const FVePlaceableMetadata& InMetadata);
	void LocalClient_OnLoaded(const FVePlaceableMetadata& InMetadata);

	UFUNCTION(Client, Reliable)
	void RemoteClient_OnPlaced(const FVePlaceableMetadata& InMetadata);

	UFUNCTION(Client, Reliable)
	void RemoteClient_OnUpdated(const FVePlaceableMetadata& InMetadata);

	UFUNCTION(Client, Reliable)
	void RemoteClient_OnLoaded(const FVePlaceableMetadata& InMetadata);

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

};
