// Authors: Egor A. Pristavka, Nikolay Bulatov. 
// Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once

#include "Components/SceneComponent.h"
#include "GltfMeshComponent.generated.h"

USTRUCT()
struct FVeGltfMetadata {
	GENERATED_BODY()

	UPROPERTY()
	int32 NodeIndex = 0;

	UPROPERTY()
	USceneComponent* SceneComponent = nullptr;

	FVeGltfMetadata() = default;

	FVeGltfMetadata(int32 InNodeIndex, USceneComponent* InSceneComponent)
		: NodeIndex(InNodeIndex), SceneComponent(InSceneComponent) { }
};


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class VEGAME_API UGltfMeshComponent : public USceneComponent {
	GENERATED_BODY()

	DECLARE_EVENT(UGltfMeshComponent, FGltfMeshComponentEvent);

	DECLARE_EVENT_OneParam(UGltfMeshComponent, FGltfMeshComponentProgressEvent, float Progress);

public:
	// Sets default values for this component's properties
	UGltfMeshComponent();

	virtual void OnRegister() override;
	virtual void OnUnregister() override;

protected:
	virtual void InitializeComponent() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	void SetAssetUrl(const FString& InUrl);
	const FString& GetAssetUrl() const { return glTFRuntimeAssetUrl; }

	void ClearAsset();
	void SetIsVisible(bool IsVisible);
	void ScaleMultiplier(float Value);

	UStaticMeshComponent* GetBoundMeshComponent() const { return BoundMeshComponent.Get(); }

	FGltfMeshComponentEvent OnDownloadingBegin;
	FGltfMeshComponentProgressEvent OnDownloadingProgress;
	FGltfMeshComponentEvent OnDownloadingEnd;

protected:
	void ClearAsset_Server();
	void ClearAsset_Client();

	UPROPERTY()
	TObjectPtr<class UglTFRuntimeAsset> GltfRuntimeAsset;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(DisplayPriority=0))
	TObjectPtr<UStaticMesh> BoundMesh = nullptr;

	UPROPERTY(ReplicatedUsing=OnRep_GltfAssetComponents, meta=(DisplayPriority=0))
	TArray<FVeGltfMetadata> GltfAssetComponents;

	UFUNCTION()
	void OnRep_GltfAssetComponents();

	void DynamicMeshCreate();

	bool GltfAssetComponents_IsExists(USceneComponent* Component);

private:
	bool bIsVisible = false;

	void DynamicMeshCreate_Server(class UglTFRuntimeAsset* Asset);
	void DynamicMeshCreate_Client(class UglTFRuntimeAsset* Asset, const TArray<FVeGltfMetadata>& InGltfMetadataList);
	class UBoxComponent* DynamicMeshCreate_AddCollision(UStaticMeshComponent* InMeshComponent);
	const FVeGltfMetadata* FindGltfMetadataItem(const TArray<FVeGltfMetadata>& InGltfMetadataList, int32 InNodeIndex);

	TSharedPtr<class FVeDownload1> Download;

	UPROPERTY()
	TMap<int32, USceneComponent*> NodeComponents;

	FString glTFRuntimeAssetUrl;

	bool GltfAssetComponents_IsReceived = false;

	/** Mesh component for Gltf bound */
	UPROPERTY()
	TObjectPtr<UStaticMeshComponent> BoundMeshComponent;

	/** Scene component for Gltf meshes */
	UPROPERTY()
	TObjectPtr<USceneComponent> GltfSceneComponent;

};
