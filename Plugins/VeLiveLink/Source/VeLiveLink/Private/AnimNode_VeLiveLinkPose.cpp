// Author: Nikolay Bulatov, Egor A. Pristavka. 
// Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#include "AnimNode_VeLiveLinkPose.h"

#include "Animation/AnimInstanceProxy.h"
#include "Animation/AnimTrace.h"
#include "Features/IModularFeatures.h"
#include "ILiveLinkClient.h"
#include "LiveLinkCustomVersion.h"
#include "LiveLinkRemapAsset.h"
#include "Roles/LiveLinkAnimationRole.h"
#include "Roles/LiveLinkAnimationTypes.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AnimNode_VeLiveLinkPose)

FAnimNode_VeLiveLinkPose::FAnimNode_VeLiveLinkPose()
	: RetargetAsset(ULiveLinkRemapAsset::StaticClass())
	  , CurrentRetargetAsset(nullptr)
	  , LiveLinkClient_AnyThread(nullptr)
	  , CachedDeltaTime(0.0f) {}

void FAnimNode_VeLiveLinkPose::OnInitializeAnimInstance(const FAnimInstanceProxy* InProxy, const UAnimInstance* InAnimInstance) {
	CurrentRetargetAsset = nullptr;

	Super::OnInitializeAnimInstance(InProxy, InAnimInstance);
}

void FAnimNode_VeLiveLinkPose::Initialize_AnyThread(const FAnimationInitializeContext& Context) {
	InputPose.Initialize(Context);
}

void FAnimNode_VeLiveLinkPose::PreUpdate(const UAnimInstance* InAnimInstance) {
	ILiveLinkClient* ThisFrameClient = nullptr;
	IModularFeatures& ModularFeatures = IModularFeatures::Get();
	if (ModularFeatures.IsModularFeatureAvailable(ILiveLinkClient::ModularFeatureName)) {
		ThisFrameClient = &IModularFeatures::Get().GetModularFeature<ILiveLinkClient>(ILiveLinkClient::ModularFeatureName);
	}
	LiveLinkClient_AnyThread = ThisFrameClient;

	// Protection as a class graph pin does not honor rules on abstract classes and NoClear
	UClass* RetargetAssetPtr = RetargetAsset.Get();
	if (!RetargetAssetPtr || RetargetAssetPtr->HasAnyClassFlags(CLASS_Abstract)) {
		RetargetAssetPtr = ULiveLinkRemapAsset::StaticClass();
		RetargetAsset = RetargetAssetPtr;
	}

	if (!CurrentRetargetAsset || RetargetAssetPtr != CurrentRetargetAsset->GetClass()) {
		CurrentRetargetAsset = NewObject<ULiveLinkRetargetAsset>(const_cast<UAnimInstance*>(InAnimInstance), RetargetAssetPtr);
		CurrentRetargetAsset->Initialize();
	}
}

void FAnimNode_VeLiveLinkPose::Update_AnyThread(const FAnimationUpdateContext& Context) {
	InputPose.Update(Context);

	GetEvaluateGraphExposedInputs().Execute(Context);

	// Accumulate Delta time from update
	CachedDeltaTime += Context.GetDeltaTime();

	// TRACE_ANIM_NODE_VALUE(Context, TEXT("SubjectName"), LiveLinkSubjectName.Name);
}

void FAnimNode_VeLiveLinkPose::Evaluate_AnyThread(FPoseContext& Output) {
	InputPose.Evaluate(Output);

	if (!LiveLinkClient_AnyThread || !CurrentRetargetAsset) {
		return;
	}

#if 1

	// UVeLiveLinkServerConnection::ARKitUpdateStaticData

	FLiveLinkBaseStaticData StaticData;
	FLiveLinkBaseStaticData* BaseStaticData = &StaticData;

	//Update property names array
	StaticData.PropertyNames.Reset((int32)EVeLiveLinkFaceBlendshapeIndex::MAX);

	//Iterate through all valid blend shapes to extract names
	const UEnum* EnumPtr = StaticEnum<EVeLiveLinkFaceBlendshapeIndex>();
	for (int32 Shape = 0; Shape < (int32)EVeLiveLinkFaceBlendshapeIndex::MAX; Shape++) {
		if (FaceBlendshapes.Contains((EVeLiveLinkFaceBlendshapeIndex)Shape)) {
			//Blendshapes don't change over time. If they were to change, we would need to keep track
			//of previous values to always have frame data matching static data
			const FName ShapeName = ParseEnumName(EnumPtr->GetNameByValue(Shape));
			StaticData.PropertyNames.Add(ShapeName);
		}
	}


	// UVeLiveLinkServerConnection::ARKitPublishBlendShapes

	FLiveLinkFrameDataStruct FrameDataStruct(FLiveLinkBaseFrameData::StaticStruct());
	FLiveLinkBaseFrameData* BaseFrameData = FrameDataStruct.Cast<FLiveLinkBaseFrameData>();
	BaseFrameData->WorldTime = FPlatformTime::Seconds();
	// FrameData->MetaData.SceneTime = FrameTime;
	BaseFrameData->PropertyValues.Reserve((int32)EVeLiveLinkFaceBlendshapeIndex::MAX);

	// Iterate through all of the blend shapes copying them into the LiveLink data type
	for (int32 Shape = 0; Shape < (int32)EVeLiveLinkFaceBlendshapeIndex::MAX; Shape++) {
		if (FaceBlendshapes.Contains((EVeLiveLinkFaceBlendshapeIndex)Shape)) {
			const float CurveValue = FaceBlendshapes.FindChecked((EVeLiveLinkFaceBlendshapeIndex)Shape);
			BaseFrameData->PropertyValues.Add(CurveValue);
		}
	}

	check(BaseFrameData);


	CurrentRetargetAsset->BuildPoseAndCurveFromBaseData(CachedDeltaTime, BaseStaticData, BaseFrameData, Output.Pose, Output.Curve);
	CachedDeltaTime = 0.f; // Reset so that if we evaluate again we don't "create" time inside of the retargeter

#else
	FLiveLinkSubjectFrameData SubjectFrameData;

	TSubclassOf<ULiveLinkRole> SubjectRole = LiveLinkClient_AnyThread->GetSubjectRole_AnyThread(LiveLinkSubjectName);
	if (SubjectRole)
	{
		if (LiveLinkClient_AnyThread->DoesSubjectSupportsRole_AnyThread(LiveLinkSubjectName, ULiveLinkAnimationRole::StaticClass()))
		{
			//Process animation data if the subject is from that type
			if (LiveLinkClient_AnyThread->EvaluateFrame_AnyThread(LiveLinkSubjectName, ULiveLinkAnimationRole::StaticClass(), SubjectFrameData))
			{
				FLiveLinkSkeletonStaticData* SkeletonData = SubjectFrameData.StaticData.Cast<FLiveLinkSkeletonStaticData>();
				FLiveLinkAnimationFrameData* FrameData = SubjectFrameData.FrameData.Cast<FLiveLinkAnimationFrameData>();
				check(SkeletonData);
				check(FrameData);
	
				CurrentRetargetAsset->BuildPoseFromAnimationData(CachedDeltaTime, SkeletonData, FrameData, Output.Pose);
				CurrentRetargetAsset->BuildPoseAndCurveFromBaseData(CachedDeltaTime, SkeletonData, FrameData, Output.Pose, Output.Curve);
				CachedDeltaTime = 0.f; // Reset so that if we evaluate again we don't "create" time inside of the retargeter
			}
		}
		else if (LiveLinkClient_AnyThread->DoesSubjectSupportsRole_AnyThread(LiveLinkSubjectName, ULiveLinkBasicRole::StaticClass()))
		{
			//Otherwise, fetch basic data that contains property / curve data
			if (LiveLinkClient_AnyThread->EvaluateFrame_AnyThread(LiveLinkSubjectName, ULiveLinkBasicRole::StaticClass(), SubjectFrameData))
			{
				FLiveLinkBaseStaticData* BaseStaticData = SubjectFrameData.StaticData.Cast<FLiveLinkBaseStaticData>();
				FLiveLinkBaseFrameData* BaseFrameData = SubjectFrameData.FrameData.Cast<FLiveLinkBaseFrameData>();
				check(BaseStaticData);
				check(BaseFrameData);
	
				CurrentRetargetAsset->BuildPoseAndCurveFromBaseData(CachedDeltaTime, BaseStaticData, BaseFrameData, Output.Pose, Output.Curve);
				CachedDeltaTime = 0.f; // Reset so that if we evaluate again we don't "create" time inside of the retargeter
			}
		}
	}
#endif
}

void FAnimNode_VeLiveLinkPose::CacheBones_AnyThread(const FAnimationCacheBonesContext& Context) {
	Super::CacheBones_AnyThread(Context);
	InputPose.CacheBones(Context);
}

void FAnimNode_VeLiveLinkPose::GatherDebugData(FNodeDebugData& DebugData) {
	// FString DebugLine = FString::Printf(TEXT("LiveLink - SubjectName: %s"), *LiveLinkSubjectName.ToString());

	// DebugData.AddDebugItem(DebugLine);
	InputPose.GatherDebugData(DebugData);
}

bool FAnimNode_VeLiveLinkPose::Serialize(FArchive& Ar) {
	Ar.UsingCustomVersion(FLiveLinkCustomVersion::GUID);

	UScriptStruct* Struct = FAnimNode_VeLiveLinkPose::StaticStruct();

	if (Ar.IsLoading() || Ar.IsSaving()) {
		Struct->SerializeTaggedProperties(Ar, (uint8*)this, Struct, nullptr);
	}

#if WITH_EDITORONLY_DATA
	//Take old data and put it in new data structure
	if (Ar.IsLoading()) {
		if (Ar.CustomVer(FLiveLinkCustomVersion::GUID) < FLiveLinkCustomVersion::NewLiveLinkRoleSystem) {
			PRAGMA_DISABLE_DEPRECATION_WARNINGS
			// LiveLinkSubjectName.Name = SubjectName_DEPRECATED;
			PRAGMA_ENABLE_DEPRECATION_WARNINGS
		}
	}
#endif

	return true;
}
