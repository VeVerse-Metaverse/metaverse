// Authors: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022-2023 VeVerse AS. All rights reserved.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "VeAiSubjectInfoWidget.h"

#include "VeAiSimpleFsmBehaviorComponent.h"
#include "VeAiSimpleFsmSubjectComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Components/TextBlock.h"

#define LOCTEXT_NAMESPACE "VeUI"

void UVeAiSubjectInfoWidget::RegisterCallbacks() {
	Super::RegisterCallbacks();
}

void UVeAiSubjectInfoWidget::UnregisterCallbacks() {
	Super::UnregisterCallbacks();
}

void UVeAiSubjectInfoWidget::NativeConstruct() {
	Super::NativeConstruct();
}

void UVeAiSubjectInfoWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime) {
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (BehaviorComponent.IsValid()) {
		auto* BehaviorComponentPtr = BehaviorComponent.Get();
		if (IsValid(SubjectWidget)) {
			SubjectWidget->SetText(FText::FromString(BehaviorComponentPtr->GetSubject()));
		}

		if (IsValid(ActionWidget)) {
			ActionWidget->SetText(FText::FromString(BehaviorComponentPtr->GetAction()));
		}

		if (IsValid(ContextWidget)) {
			ContextWidget->SetText(FText::FromString(BehaviorComponentPtr->GetContext()));
		}

		if (IsValid(TargetWidget)) {
			TargetWidget->SetText(FText::FromString(BehaviorComponentPtr->GetTarget()));
		}

		if (IsValid(MetadataWidget)) {
			MetadataWidget->SetText(FText::FromString(BehaviorComponentPtr->GetMetadata()));
		}

		if (IsValid(ActionTypeWidget)) {
			switch (BehaviorComponentPtr->GetActionType()) {
			case EVeAiSimpleFsmActionType::None:
				ActionTypeWidget->SetText(LOCTEXT("None", "None"));
				break;
			case EVeAiSimpleFsmActionType::Go:
				ActionTypeWidget->SetText(LOCTEXT("Go", "Go"));
				break;
			case EVeAiSimpleFsmActionType::Give:
				ActionTypeWidget->SetText(LOCTEXT("Give", "Give"));
				break;
			case EVeAiSimpleFsmActionType::Take:
				ActionTypeWidget->SetText(LOCTEXT("Take", "Take"));
				break;
			case EVeAiSimpleFsmActionType::Use:
				ActionTypeWidget->SetText(LOCTEXT("Use", "Use"));
				break;
			case EVeAiSimpleFsmActionType::Say:
				ActionTypeWidget->SetText(LOCTEXT("Say", "Say"));
				break;
			default:
				ActionTypeWidget->SetText(LOCTEXT("Unknown", "Unknown"));
				break;
			}
		}

		if (IsValid(SubjectEntityWidget)) {
			const auto Text = IsValid(BehaviorComponentPtr->GetSubjectEntity()) ? BehaviorComponentPtr->GetSubjectEntity()->GetName() : "";
			SubjectEntityWidget->SetText(FText::FromString(Text));
		}

		if (IsValid(TargetEntityWidget)) {
			const auto Text = IsValid(BehaviorComponentPtr->GetTargetEntity()) ? BehaviorComponentPtr->GetTargetEntity()->GetName() : "";
			TargetEntityWidget->SetText(FText::FromString(Text));
		}

		if (IsValid(ContextEnityWidget)) {
			const auto Text = IsValid(BehaviorComponentPtr->GetContextEntity()) ? BehaviorComponentPtr->GetContextEntity()->GetName() : "";
			ContextEnityWidget->SetText(FText::FromString(Text));
		}

		if (IsValid(SubjectActorWidget)) {
			const auto Text = IsValid(BehaviorComponentPtr->GetSubjectActor()) ? BehaviorComponentPtr->GetSubjectActor()->GetName() : "";
			SubjectActorWidget->SetText(FText::FromString(Text));
		}

		if (IsValid(TargetActorWidget)) {
			const auto Text = IsValid(BehaviorComponentPtr->GetTargetActor()) ? BehaviorComponentPtr->GetTargetActor()->GetName() : "";
			TargetActorWidget->SetText(FText::FromString(Text));
		}

		if (IsValid(ContextActorWidget)) {
			const auto Text = IsValid(BehaviorComponentPtr->GetContextActor()) ? BehaviorComponentPtr->GetContextActor()->GetName() : "";
			ContextActorWidget->SetText(FText::FromString(Text));
		}
	}
}

#undef LOCTEXT_NAMESPACE
