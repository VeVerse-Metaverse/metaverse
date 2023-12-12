// Author: Nikolay Bulatov, Egor A. Pristavka. 
// Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#pragma once

#include "Animation/AnimNodeBase.h"

#include "CoreMinimal.h"
#include "LiveLinkRetargetAsset.h"
#include "LiveLinkTypes.h"
#include "VeLiveLinkTypes.h"

#include "AnimNode_VeLiveLinkPose.generated.h"


class ILiveLinkClient;


PRAGMA_DISABLE_DEPRECATION_WARNINGS

USTRUCT(BlueprintInternalUseOnly)
struct VELIVELINK_API FAnimNode_VeLiveLinkPose : public FAnimNode_Base
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input)
	FPoseLink InputPose;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SourceData, meta = (PinShownByDefault))
	TMap<EVeLiveLinkFaceBlendshapeIndex, float> FaceBlendshapes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, NoClear, Category = Retarget, meta = (NeverAsPin))
	TSubclassOf<ULiveLinkRetargetAsset> RetargetAsset;

	UPROPERTY(transient)
	TObjectPtr<ULiveLinkRetargetAsset> CurrentRetargetAsset;

public:
	FAnimNode_VeLiveLinkPose();

	//~ FAnimNode_Base interface
	virtual void Initialize_AnyThread(const FAnimationInitializeContext& Context) override;
	virtual void CacheBones_AnyThread(const FAnimationCacheBonesContext & Context) override;
	virtual void Update_AnyThread(const FAnimationUpdateContext & Context) override;
	virtual void Evaluate_AnyThread(FPoseContext& Output) override;
	virtual bool HasPreUpdate() const { return true; }
	virtual void PreUpdate(const UAnimInstance* InAnimInstance) override;
	virtual void GatherDebugData(FNodeDebugData& DebugData) override;
	//~ End of FAnimNode_Base interface

	bool Serialize(FArchive& Ar);

protected:
	virtual void OnInitializeAnimInstance(const FAnimInstanceProxy* InProxy, const UAnimInstance* InAnimInstance) override;

private:

	ILiveLinkClient* LiveLinkClient_AnyThread;

	// Delta time from update so that it can be passed to retargeter
	float CachedDeltaTime;

	static FName ParseEnumName(FName EnumName)
	{
		const int32 BlendShapeEnumNameLength = 28; // Length of 'EVeLiveLinkBlendshapeIndex::'
		FString EnumString = EnumName.ToString();
		return FName(*EnumString.Right(EnumString.Len() - BlendShapeEnumNameLength));
	}
};

PRAGMA_ENABLE_DEPRECATION_WARNINGS


template<> struct TStructOpsTypeTraits<FAnimNode_VeLiveLinkPose> : public TStructOpsTypeTraitsBase2<FAnimNode_VeLiveLinkPose>
{
	enum 
	{ 
		WithSerializer = true
	};
};
