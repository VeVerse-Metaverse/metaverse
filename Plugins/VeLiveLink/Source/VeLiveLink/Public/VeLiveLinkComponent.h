// Author: Nikolay Bulatov, Egor A. Pristavka. 
// Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "LiveLinkTypes.h"
#include "VeLiveLinkTypes.h"
#include "VeLiveLinkComponent.generated.h"

class ILiveLinkClient;

/**
 * 
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class VELIVELINK_API UVeLiveLinkComponent : public USceneComponent {
	GENERATED_BODY()

public:
	UVeLiveLinkComponent();

	// virtual void InitializeComponent() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void InitializeComponent() override;
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintPure, Category="VeLiveLink")
	const FVeLiveLinkFrameData& GetFrameData() const { return FrameData; }

	UFUNCTION(BlueprintPure, Category="VeLiveLink")
	const TMap<EVeLiveLinkFaceBlendshapeIndex, float>& GetFaceBlendshapes() const { return FrameData.FaceBlendshapes; }

	UFUNCTION(BlueprintPure, Category="VeLiveLink")
	const TMap<EVeLiveLinkPoseLandmarkIndex, FVeLiveLinkPoseLandmarks>& GetPoseLandmarks() const { return FrameData.PoseLandmarks; }

	UFUNCTION(BlueprintPure, Category="VeLiveLink")
	const TMap<EVeLiveLinkHandLandmarkIndex, FVector>& GetLeftHandLandmarks() const { return FrameData.LeftHandLandmarks; }

	UFUNCTION(BlueprintPure, Category="VeLiveLink")
	const TMap<EVeLiveLinkHandLandmarkIndex, FVector>& GetRightHandLandmarks() const { return FrameData.RightHandLandmarks; }

	UFUNCTION(BlueprintPure, Category="VeLiveLink")
	const TMap<EVeLiveLinkPoseLandmarkIndex, FVeLiveLinkPoseLandmarks>& GetWorldPoseLandmarks() const { return WorldPoseLandmarks; }

	UFUNCTION(BlueprintPure, Category="VeLiveLink")
	const TMap<EVeLiveLinkHandLandmarkIndex, FVector>& GetWorldLeftHandLandmarks() const { return WorldLeftHandLandmarks; }

	UFUNCTION(BlueprintPure, Category="VeLiveLink")
	const TMap<EVeLiveLinkHandLandmarkIndex, FVector>& GetWorldRightHandLandmarks() const { return WorldRightHandLandmarks; }

	void SetEnableInput(bool Value);

protected:
	UPROPERTY(EditDefaultsOnly, Category="VeLiveLink")
	bool EnableFaceBlendshapes = true;

	UPROPERTY(EditDefaultsOnly, Category="VeLiveLink")
	bool EnablePoseLandmarks = true;

	UPROPERTY(EditDefaultsOnly, Category="VeLiveLink")
	bool EnableHandLandmarks = true;

	UPROPERTY(EditDefaultsOnly, Category="VeLiveLink")
	TArray<EVeLiveLinkFaceBlendshapeIndex> ReplicationFaceBlendshapes;

	UPROPERTY(EditDefaultsOnly, Category="VeLiveLink")
	TArray<EVeLiveLinkPoseLandmarkIndex> ReplicationPoseLandmarks;

	UPROPERTY(EditDefaultsOnly, Category="VeLiveLink")
	TArray<EVeLiveLinkHandLandmarkIndex> ReplicationHandLandmarks;

	UPROPERTY(EditAnywhere, Category="VeLiveLink")
	FName LiveLinkSubjectName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="VeLiveLink")
	bool bBindHand = true;

	UPROPERTY(ReplicatedUsing="OnRep_RawFrameData")
	TArray<uint8> RawFrameData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="VeLiveLink")
	bool EnableReplication = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="VeLiveLink")
	float ReplicationTime = 0.1;

	UFUNCTION()
	void OnRep_RawFrameData();

	UFUNCTION(Server, Unreliable)
	void Server_LiveLinkFrameData(const TArray<uint8>& InRawFrameData);

	TMap<EVeLiveLinkPoseLandmarkIndex, FVeLiveLinkPoseLandmarks> WorldPoseLandmarks;
	FVeLiveLinkFrameData FrameData;
	TMap<EVeLiveLinkHandLandmarkIndex, FVector> WorldLeftHandLandmarks;
	TMap<EVeLiveLinkHandLandmarkIndex, FVector> WorldRightHandLandmarks;


	void OnWindowFocusChanged(const bool bInIsFocused);
	void UpdateInput();

private:
	bool bEnableInput = false;
	bool bIsFocused = true;
	double ReplicationLastTime = 0.0f;

	ILiveLinkClient* LiveLinkClient_AnyThread;

	uint32 RawFrameDataSize = 0;

	void UpdateFrameData(float DeltaTime, FVeLiveLinkFrameData* InFrameData);
	void ReadFrameData(const TArray<uint8>& InRawFrameData);

	static void UpdateLocationFromMediapipe(FVector& Location);

	static void PrintLog(const FVeLiveLinkFrameData& FrameData);

	static FName ParseEnumName(FName EnumName) {
		const int32 BlendShapeEnumNameLength = 30; // Length of 'EVeLiveLinkPoseLandmarkIndex::'
		FString EnumString = EnumName.ToString();
		return FName(*EnumString.Right(EnumString.Len() - BlendShapeEnumNameLength));
	}
};
