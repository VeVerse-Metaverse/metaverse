// Author: Nikolay Bulatov, Egor A. Pristavka. 
// Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#pragma once

#include "AnimGraphNode_Base.h"
#include "AnimNode_VeLiveLinkPose.h"

#include "AnimGraphNode_VeLiveLinkPose.generated.h"

UCLASS()
class VELIVELINKGRAPHNODE_API UAnimGraphNode_VeLiveLinkPose : public UAnimGraphNode_Base
{
	GENERATED_BODY()

public:

	//~ UEdGraphNode interface
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual FText GetTooltipText() const override;
	virtual FText GetMenuCategory() const;
	//~ End of UEdGraphNode

	//~ Begin UK2Node interface
	virtual void GetOutputLinkAttributes(FNodeAttributeArray& OutAttributes) const override;
	//~ End UK2Node interface

public:

	UPROPERTY(EditAnywhere, Category = Settings)
	FAnimNode_VeLiveLinkPose Node;

};
