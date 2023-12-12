// Author: Nikolay Bulatov, Egor A. Pristavka. 
// Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#include "AnimGraphNode_VeLiveLinkPose.h"
#include "EdGraph/EdGraphSchema.h"
#include "Animation/AnimAttributes.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AnimGraphNode_VeLiveLinkPose)

#define LOCTEXT_NAMESPACE "VeLiveLinkAnimNode"

FText UAnimGraphNode_VeLiveLinkPose::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return LOCTEXT("NodeTitle", "VeLiveLink Pose");
}

FText UAnimGraphNode_VeLiveLinkPose::GetTooltipText() const
{
	return LOCTEXT("NodeTooltip", "Retrieves replicated current pose associated with the supplied subject");
}

FText UAnimGraphNode_VeLiveLinkPose::GetMenuCategory() const
{
	return LOCTEXT("NodeCategory", "VeLiveLink");
}

void UAnimGraphNode_VeLiveLinkPose::GetOutputLinkAttributes(FNodeAttributeArray& OutAttributes) const
{
	OutAttributes.Add(UE::Anim::FAttributes::Curves);
	OutAttributes.Add(UE::Anim::FAttributes::Attributes);
}

#undef LOCTEXT_NAMESPACE
