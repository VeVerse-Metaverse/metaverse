// Authors: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022-2023 VeVerse AS. All rights reserved.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#if WITH_EDITORONLY_DATA

#include "CogAiDetailsCustomization.h"
#include "CogAiMessage.h"
#include "CogAiNpcComponent.h"
#include "VeAiCharacter.h"
#include "PropertyEditorModule.h"
#include "DetailLayoutBuilder.h"
#include "DetailCategoryBuilder.h"
#include "DetailWidgetRow.h"

#define LOCTEXT_NAMESPACE "VeAI"

void FCogAiMessageDetails::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) {
	// Edit the "AI" category
	IDetailCategoryBuilder& AiCategory = DetailBuilder.EditCategory("AI");

	// Add properties to the category
	AiCategory.AddProperty(GET_MEMBER_NAME_CHECKED(UCogAiMessage, Role));
	AiCategory.AddProperty(GET_MEMBER_NAME_CHECKED(UCogAiMessage, Parameters));
}

void FCogAiActionMessageDetails::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) {
	// Edit the "AI" category
	IDetailCategoryBuilder& AiCategory = DetailBuilder.EditCategory("AI");

	// Add properties to the category
	AiCategory.AddProperty(GET_MEMBER_NAME_CHECKED(UCogAiActionMessage, Role));
	AiCategory.AddProperty(GET_MEMBER_NAME_CHECKED(UCogAiActionMessage, Parameters));
	AiCategory.AddProperty(GET_MEMBER_NAME_CHECKED(UCogAiActionMessage, Type));
}

void FCogAiSystemMessageDetails::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) {
	// Edit the "AI" category
	IDetailCategoryBuilder& AiCategory = DetailBuilder.EditCategory("AI");

	// Add properties to the category
	AiCategory.AddProperty(GET_MEMBER_NAME_CHECKED(UCogAiSystemMessage, Role));
	AiCategory.AddProperty(GET_MEMBER_NAME_CHECKED(UCogAiSystemMessage, Parameters));
	AiCategory.AddProperty(GET_MEMBER_NAME_CHECKED(UCogAiSystemMessage, Type));
}

void FCogAiWhoamiSystemMessageDetails::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) {
	// Edit the "AI" category
	IDetailCategoryBuilder& AiCategory = DetailBuilder.EditCategory("AI");

	// Add properties to the category
	AiCategory.AddProperty(GET_MEMBER_NAME_CHECKED(UCogAiWhoamiSystemMessage, Role));
	AiCategory.AddProperty(GET_MEMBER_NAME_CHECKED(UCogAiWhoamiSystemMessage, Parameters));
	AiCategory.AddProperty(GET_MEMBER_NAME_CHECKED(UCogAiWhoamiSystemMessage, Type));
	AiCategory.AddProperty(GET_MEMBER_NAME_CHECKED(UCogAiWhoamiSystemMessage, Name));
	AiCategory.AddProperty(GET_MEMBER_NAME_CHECKED(UCogAiWhoamiSystemMessage, Description));
	AiCategory.AddProperty(GET_MEMBER_NAME_CHECKED(UCogAiWhoamiSystemMessage, Faction));
	AiCategory.AddProperty(GET_MEMBER_NAME_CHECKED(UCogAiWhoamiSystemMessage, Personality));
}

void FCogAiActionsSystemMessageDetails::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) {
	// Edit the "AI" category
	IDetailCategoryBuilder& AiCategory = DetailBuilder.EditCategory("AI");

	// Add properties to the category
	AiCategory.AddProperty(GET_MEMBER_NAME_CHECKED(UCogAiActionsSystemMessage, Role));
	AiCategory.AddProperty(GET_MEMBER_NAME_CHECKED(UCogAiActionsSystemMessage, Parameters));
	AiCategory.AddProperty(GET_MEMBER_NAME_CHECKED(UCogAiActionsSystemMessage, Type));
	AiCategory.AddProperty(GET_MEMBER_NAME_CHECKED(UCogAiActionsSystemMessage, Actions));
}

void FCogAiContextSystemMessageDetails::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) {
	// Edit the "AI" category
	IDetailCategoryBuilder& AiCategory = DetailBuilder.EditCategory("AI");

	// Add properties to the category
	AiCategory.AddProperty(GET_MEMBER_NAME_CHECKED(UCogAiContextSystemMessage, Role));
	AiCategory.AddProperty(GET_MEMBER_NAME_CHECKED(UCogAiContextSystemMessage, Parameters));
	AiCategory.AddProperty(GET_MEMBER_NAME_CHECKED(UCogAiContextSystemMessage, Type));
	AiCategory.AddProperty(GET_MEMBER_NAME_CHECKED(UCogAiContextSystemMessage, Description));
	AiCategory.AddProperty(GET_MEMBER_NAME_CHECKED(UCogAiContextSystemMessage, Location));
	AiCategory.AddProperty(GET_MEMBER_NAME_CHECKED(UCogAiContextSystemMessage, Time));
	AiCategory.AddProperty(GET_MEMBER_NAME_CHECKED(UCogAiContextSystemMessage, Weather));
	AiCategory.AddProperty(GET_MEMBER_NAME_CHECKED(UCogAiContextSystemMessage, Vibe));
}

void FCogAiInspectSystemMessageDetails::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) {
	// Edit the "AI" category
	IDetailCategoryBuilder& AiCategory = DetailBuilder.EditCategory("AI");

	// Add properties to the category
	AiCategory.AddProperty(GET_MEMBER_NAME_CHECKED(UCogAiInspectSystemMessage, Role));
	AiCategory.AddProperty(GET_MEMBER_NAME_CHECKED(UCogAiInspectSystemMessage, Parameters));
	AiCategory.AddProperty(GET_MEMBER_NAME_CHECKED(UCogAiInspectSystemMessage, Type));
	AiCategory.AddProperty(GET_MEMBER_NAME_CHECKED(UCogAiInspectSystemMessage, TargetName));
	AiCategory.AddProperty(GET_MEMBER_NAME_CHECKED(UCogAiInspectSystemMessage, TargetDescription));
}

void FCogAiPerceptionSystemMessageDetails::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) {
	// Edit the "AI" category
	IDetailCategoryBuilder& AiCategory = DetailBuilder.EditCategory("AI");

	// Add properties to the category
	AiCategory.AddProperty(GET_MEMBER_NAME_CHECKED(UCogAiPerceptionSystemMessage, Role));
	AiCategory.AddProperty(GET_MEMBER_NAME_CHECKED(UCogAiPerceptionSystemMessage, Parameters));
	AiCategory.AddProperty(GET_MEMBER_NAME_CHECKED(UCogAiPerceptionSystemMessage, Type));
	AiCategory.AddProperty(GET_MEMBER_NAME_CHECKED(UCogAiPerceptionSystemMessage, Visual));
	AiCategory.AddProperty(GET_MEMBER_NAME_CHECKED(UCogAiPerceptionSystemMessage, Audio));
	AiCategory.AddProperty(GET_MEMBER_NAME_CHECKED(UCogAiPerceptionSystemMessage, Other));
}

void FCogAiNpcComponentDetails::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) {
	IDetailCategoryBuilder& Category = DetailBuilder.EditCategory("AI Debug");

	Category.AddCustomRow(FText::GetEmpty())
			.NameContent()
		[
			SNew(STextBlock)
			.Text(FText::FromString("Save Message History"))
			.Font(DetailBuilder.GetDetailFont())
		]
		.ValueContent()
		.MaxDesiredWidth(125.0f)
		[
			SNew(SButton)
			.Text(FText::FromString("Save"))
			.OnClicked_Lambda([&DetailBuilder]() {
							 TArray<TWeakObjectPtr<UObject>> SelectedObjects;
							 DetailBuilder.GetObjectsBeingCustomized(SelectedObjects);

							 for (const TWeakObjectPtr<UObject>& ObjectWeakPtr : SelectedObjects) {
								 if (const UCogAiNpcComponent* Component = Cast<UCogAiNpcComponent>(ObjectWeakPtr.Get())) {
									 FString FilePath = FPaths::ProjectSavedDir() + "MessageHistory.txt";
									 Component->SaveMessageHistoryToFile(FilePath);
								 }
							 }

							 return FReply::Handled();
						 })
		];

	Category.AddCustomRow(FText::GetEmpty())
			.NameContent()
		[
			SNew(STextBlock)
			.Text(FText::FromString("Take a Debug Action"))
			.Font(DetailBuilder.GetDetailFont())
		]
		.ValueContent()
		.MaxDesiredWidth(125.0f)
		[
			SNew(SButton)
			.Text(FText::FromString("Take Action"))
			.OnClicked_Lambda([&DetailBuilder]() {
							 TArray<TWeakObjectPtr<UObject>> SelectedObjects;
							 DetailBuilder.GetObjectsBeingCustomized(SelectedObjects);

							 for (const TWeakObjectPtr<UObject>& ObjectWeakPtr : SelectedObjects) {
								 if (UCogAiNpcComponent* Component = Cast<UCogAiNpcComponent>(ObjectWeakPtr.Get())) {
									 UCogAiActionMessage* ActionMessage = NewObject<UCogAiActionMessage>(Component);
									 ActionMessage->Type = Component->DebugAction;
									 ActionMessage->Parameters = Component->DebugActionParameters;
									 Component->TakeAction(ActionMessage);
								 }
							 }

							 return FReply::Handled();
						 })
		];
}

void FVeAiCharacterDetails::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) {
	IDetailCategoryBuilder& Category = DetailBuilder.EditCategory("AI");

	Category.AddCustomRow(FText::GetEmpty())
			.NameContent()
		[
			SNew(STextBlock)
			.Text(FText::FromString("Current Emotion Apply (Debug)"))
			.Font(DetailBuilder.GetDetailFont())
		]
		.ValueContent()
		.MaxDesiredWidth(125.0f)
		[
			SNew(SButton)
			.Text(FText::FromString("Apply"))
			.OnClicked_Lambda([&DetailBuilder]() {
							 TArray<TWeakObjectPtr<UObject>> SelectedObjects;
							 DetailBuilder.GetObjectsBeingCustomized(SelectedObjects);

							 for (const TWeakObjectPtr<UObject>& ObjectWeakPtr : SelectedObjects) {
								 if (AVeAiCharacter* Character = Cast<AVeAiCharacter>(ObjectWeakPtr.Get())) {
									 Character->SetEmotion(Character->CurrentEmotion, Character->CurrentEmotionIntensity);
								 }
							 }

							 return FReply::Handled();
						 })
		];
}

void RegisterCogAiDetailsCustomization() {
	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	PropertyModule.RegisterCustomClassLayout(UCogAiMessage::StaticClass()->GetFName(), FOnGetDetailCustomizationInstance::CreateStatic(&FCogAiMessageDetails::MakeInstance));
	PropertyModule.RegisterCustomClassLayout(UCogAiActionMessage::StaticClass()->GetFName(), FOnGetDetailCustomizationInstance::CreateStatic(&FCogAiActionMessageDetails::MakeInstance));
	PropertyModule.RegisterCustomClassLayout(UCogAiSystemMessage::StaticClass()->GetFName(), FOnGetDetailCustomizationInstance::CreateStatic(&FCogAiSystemMessageDetails::MakeInstance));
	PropertyModule.RegisterCustomClassLayout(UCogAiWhoamiSystemMessage::StaticClass()->GetFName(), FOnGetDetailCustomizationInstance::CreateStatic(&FCogAiWhoamiSystemMessageDetails::MakeInstance));
	PropertyModule.RegisterCustomClassLayout(UCogAiActionsSystemMessage::StaticClass()->GetFName(), FOnGetDetailCustomizationInstance::CreateStatic(&FCogAiActionsSystemMessageDetails::MakeInstance));
	PropertyModule.RegisterCustomClassLayout(UCogAiContextSystemMessage::StaticClass()->GetFName(), FOnGetDetailCustomizationInstance::CreateStatic(&FCogAiContextSystemMessageDetails::MakeInstance));
	PropertyModule.RegisterCustomClassLayout(UCogAiInspectSystemMessage::StaticClass()->GetFName(), FOnGetDetailCustomizationInstance::CreateStatic(&FCogAiInspectSystemMessageDetails::MakeInstance));
	PropertyModule.RegisterCustomClassLayout(UCogAiPerceptionSystemMessage::StaticClass()->GetFName(), FOnGetDetailCustomizationInstance::CreateStatic(&FCogAiPerceptionSystemMessageDetails::MakeInstance));
	PropertyModule.RegisterCustomClassLayout(UCogAiNpcComponent::StaticClass()->GetFName(), FOnGetDetailCustomizationInstance::CreateStatic(&FCogAiNpcComponentDetails::MakeInstance));
	PropertyModule.RegisterCustomClassLayout(AVeAiCharacter::StaticClass()->GetFName(), FOnGetDetailCustomizationInstance::CreateStatic(&FVeAiCharacterDetails::MakeInstance));
}

void UnregisterCogAiDetailsCustomization() {
	if (FModuleManager::Get().IsModuleLoaded("PropertyEditor")) {
		FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
		PropertyModule.UnregisterCustomClassLayout(UCogAiMessage::StaticClass()->GetFName());
		PropertyModule.UnregisterCustomClassLayout(UCogAiActionMessage::StaticClass()->GetFName());
		PropertyModule.UnregisterCustomClassLayout(UCogAiSystemMessage::StaticClass()->GetFName());
		PropertyModule.UnregisterCustomClassLayout(UCogAiWhoamiSystemMessage::StaticClass()->GetFName());
		PropertyModule.UnregisterCustomClassLayout(UCogAiActionsSystemMessage::StaticClass()->GetFName());
		PropertyModule.UnregisterCustomClassLayout(UCogAiContextSystemMessage::StaticClass()->GetFName());
		PropertyModule.UnregisterCustomClassLayout(UCogAiInspectSystemMessage::StaticClass()->GetFName());
		PropertyModule.UnregisterCustomClassLayout(UCogAiPerceptionSystemMessage::StaticClass()->GetFName());
		PropertyModule.UnregisterCustomClassLayout(UCogAiNpcComponent::StaticClass()->GetFName());
		PropertyModule.UnregisterCustomClassLayout(AVeAiCharacter::StaticClass()->GetFName());
	}
}

#undef LOCTEXT_NAMESPACE

#endif // WITH_EDITOR_ONLY_DATA
