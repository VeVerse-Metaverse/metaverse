#include "VeAiSimpleFsmState.h"
#include "Api2AiSimpleFsmRequest.h"
#include "VeAiEnums.h"

bool FVeAiSimpleFsmState::operator==(const FVeAiSimpleFsmState& Other) const {
	return NPC == Other.NPC && Action == Other.Action && Context == Other.Context && Target == Other.Target && Metadata == Other.Metadata;
}

void FVeAiSimpleFsmState::FromApiStateMetadata(const FApi2AiSimpleFsmStateMetadata& InApiStateMetadata) {
	NPC = InApiStateMetadata.NPC;
	Action = InApiStateMetadata.Action;
	Context = InApiStateMetadata.Context;
	Target = InApiStateMetadata.Target;
	Metadata = InApiStateMetadata.Metadata;
	Emotion = InApiStateMetadata.Emotion;
	if (Action == "go" || Action == "move" || Action == "walk" || Action == "run" || Action == "jump" || Action == "fly" || Action == "swim") {
		ActionType = EVeAiSimpleFsmActionType::Go;
	} else if (Action == "say" || Action == "talk" || Action == "speak") {
		ActionType = EVeAiSimpleFsmActionType::Say;
	} else if (Action == "give") {
		ActionType = EVeAiSimpleFsmActionType::Give;
	} else if (Action == "use" || Action == "activate" || Action == "interact") {
		ActionType = EVeAiSimpleFsmActionType::Use;
	} else {
		ActionType = EVeAiSimpleFsmActionType::None;
	}
}
