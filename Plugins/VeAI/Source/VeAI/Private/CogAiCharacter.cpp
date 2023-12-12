// Authors: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022-2023 VeVerse AS. All rights reserved.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "CogAiCharacter.h"

#include "ApiPropertySubsystem.h"
#include "CogAiNpcComponent.h"
#include "CogAiNpcSettingsComponent.h"
#include "EditorComponent.h"
#include "GameFrameworkEditorSubsystem.h"
#include "PlaceableComponent.h"
#include "VeAI.h"
#include "VeAiPlayerState.h"
#include "VeGameFramework.h"
#include "VeShared.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISense_Hearing.h"
#include "Perception/AISense_Sight.h"
#include "UIEditorCogAiPropertiesWidget.h"
#include "VeApi.h"

void ACogAiCharacter::BeginPlay() {
	Super::BeginPlay();

	if (IsValid(AIPerceptionStimuliSourceComponent)) {
		AIPerceptionStimuliSourceComponent->RegisterForSense(UAISense_Sight::StaticClass());
		AIPerceptionStimuliSourceComponent->RegisterForSense(UAISense_Hearing::StaticClass());
	} else {
		VeLogErrorFunc("AIPerceptionStimuliSourceComponent is invalid");
	}
}

void ACogAiCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason) {
	if (IsValid(AIPerceptionStimuliSourceComponent)) {
		AIPerceptionStimuliSourceComponent->UnregisterFromSense(UAISense_Sight::StaticClass());
		AIPerceptionStimuliSourceComponent->UnregisterFromSense(UAISense_Hearing::StaticClass());
	} else {
		VeLogErrorFunc("AIPerceptionStimuliSourceComponent is invalid");
	}

	Super::EndPlay(EndPlayReason);
}

void ACogAiCharacter::PostInitializeComponents() {
	Super::PostInitializeComponents();

	if (PlaceableComponent) {
		PlaceableComponent->GetOnPlaceableMetadataChanged().AddWeakLambda(this, [=](const FVePlaceableMetadata& InMetadata, EPlaceableChangeType InChange) {
			OnPlaceableMetadataUpdated(InMetadata);
		});
	}
}

void ACogAiCharacter::OnPlaceableMetadataUpdated(const FVePlaceableMetadata& InMetadata) {
	for (auto& Property : GetPlaceableMetadata().Properties) {
		if (Property.Name == TEXT("UrlAvatar")) {
			DefaultCogAiParams.UrlAvatar = Property.Value;
			continue;
		}
		if (Property.Name == TEXT("OpenAIKey")) {
			DefaultCogAiParams.OpenAIKey = Property.Value;
			continue;
		}
		if (Property.Name == TEXT("AiName")) {
			DefaultCogAiParams.AiName = Property.Value;
			continue;
		}
		if (Property.Name == TEXT("AiDescription")) {
			DefaultCogAiParams.AiDescription = Property.Value;
			continue;
		}
		if (Property.Name == TEXT("PersonalityTraits")) {
			DefaultCogAiParams.PersonalityTraits = Property.Value;
			continue;
		}
		if (Property.Name == TEXT("AiVoice")) {
			DefaultCogAiParams.AiVoice = Property.Value;
			continue;
		}
	}

	SetPlaceableCogAiParams(DefaultCogAiParams);
}

// Sets default values
ACogAiCharacter::ACogAiCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer) {
	PrimaryActorTick.bCanEverTick = true;

	CogAiNpcSettingsComponent = CreateDefaultSubobject<UCogAiNpcSettingsComponent>("CogAiNpcSettings");
	AIPerceptionStimuliSourceComponent = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>("AIPerceptionStimuliSourceComponent");

	PlaceableComponent = CreateDefaultSubobject<UPlaceableComponent>(TEXT("PlaceableComponent"));

}

void ACogAiCharacter::OnAiPlayerStateChange() {
	Super::OnAiPlayerStateChange();

	if (HasAuthority()) {
		if (IsValid(AiPlayerState)) {
			AiPlayerState->GetOnAiUserMetadataUpdated().AddWeakLambda(this, [=](const FVeUserMetadata& InUserMetadata) {
				if (IsValid(CogAiNpcSettingsComponent)) {
					if (const auto* Name = CogAiNpcSettingsComponent->PerceptibleProperties.Find(TEXT("name")); !Name || Name->IsEmpty()) {
						CogAiNpcSettingsComponent->PerceptibleProperties.Add("name", InUserMetadata.Name);
					}

					if (const auto* Description = CogAiNpcSettingsComponent->PerceptibleProperties.Find(TEXT("description")); !Description || Description->IsEmpty()) {
						CogAiNpcSettingsComponent->PerceptibleProperties.Add("description", InUserMetadata.Description);
					}

					if (const auto* InspectDescription = CogAiNpcSettingsComponent->PerceptibleProperties.Find(TEXT("inspectDescription")); !InspectDescription || InspectDescription->IsEmpty()) {
						CogAiNpcSettingsComponent->PerceptibleProperties.Add("inspectDescription", InUserMetadata.Description);
					}
				}
			});
		}
	}
}

const FVePlaceableMetadata& ACogAiCharacter::GetPlaceableMetadata() const {
	return PlaceableComponent->GetMetadata();
}

UUIPlaceablePropertiesWidget* ACogAiCharacter::GetPlaceablePropertiesWidget_Implementation(APlayerController* PlayerController) {
	if (!IsValid(PlaceablePropertiesClass)) {
		return nullptr;
	}

	auto* PropertiesWidget = CreateWidget<UUIEditorCogAiPropertiesWidget>(PlayerController, PlaceablePropertiesClass);
	if (!PropertiesWidget) {
		VeLogErrorFunc("Failed to create a properties widget instance");
		return nullptr;
	}

	PropertiesWidget->SetProperties(CurrentCogAiParams.UrlAvatar, CurrentCogAiParams.OpenAIKey, CurrentCogAiParams.AiName, CurrentCogAiParams.AiDescription, CurrentCogAiParams.PersonalityTraits, CurrentCogAiParams.AiVoice);

	PropertiesWidget->GetOnSave().BindWeakLambda(this, [=]() {
		Properties_OnSave(PropertiesWidget);
	});

	PropertiesWidget->GetOnChanged().BindWeakLambda(this, [=]() {
		Properties_OnChanged(PropertiesWidget);
	});

	return PropertiesWidget;
}

void ACogAiCharacter::Properties_OnSave(UUIEditorCogAiPropertiesWidget* PropertiesWidget) {
	GET_MODULE_SUBSYSTEM(PropertySubsystem, Api, Property);
	if (PropertySubsystem) {
		SetPlaceableState(EPlaceableState::Uploading);

		TArray<FApiUpdatePropertyMetadata> Properties;
		Properties.Emplace(
			TEXT("UrlAvatar"),
			TEXT("string"),
			CurrentCogAiParams.UrlAvatar
			);
		Properties.Emplace(
			TEXT("OpenAIKey"),
			TEXT("string"),
			CurrentCogAiParams.OpenAIKey
			);
		Properties.Emplace(
			TEXT("AiName"),
			TEXT("string"),
			CurrentCogAiParams.AiName
			);
		Properties.Emplace(
			TEXT("AiDescription"),
			TEXT("string"),
			CurrentCogAiParams.AiDescription
			);
		Properties.Emplace(
			TEXT("PersonalityTraits"),
			TEXT("string"),
			CurrentCogAiParams.PersonalityTraits
			);
		Properties.Emplace(
			TEXT("AiVoice"),
			TEXT("string"),
			CurrentCogAiParams.AiVoice
			);

		const auto Callback = MakeShared<FOnGenericRequestCompleted>();
		Callback->BindWeakLambda(this, [=](const bool bInSuccessful, const FString& InError) {
			SetPlaceableState(EPlaceableState::None);
			if (!bInSuccessful) {
				VeLogErrorFunc("failed to update properties");
				return;
			}
			UpdatePlaceableOnServer();
		});

		PropertySubsystem->Update(GetPlaceableMetadata().Id, Properties, 0, Callback);
	}
}

void ACogAiCharacter::UpdatePlaceableOnServer() {
	GET_MODULE_SUBSYSTEM(EditorSubsystem, GameFramework, Editor);
	if (EditorSubsystem) {
		if (auto* EditorComponent = EditorSubsystem->GetEditorComponent(); IsValid(EditorComponent)) {
			EditorComponent->Owner_UpdatePlaceables({this});
		}
	}
}

void ACogAiCharacter::SetPlaceableState(EPlaceableState InState, float InRatio) {
	SetPlaceableStateHelper(InState, InRatio);
}

void ACogAiCharacter::Properties_OnChanged(UUIEditorCogAiPropertiesWidget* PropertiesWidget) {
	FPlaceableCogAiParams NewCogAiParams;
	NewCogAiParams.OpenAIKey = PropertiesWidget->GetOpenAiKey();
	NewCogAiParams.UrlAvatar = PropertiesWidget->GetUrl();
	NewCogAiParams.AiName = PropertiesWidget->GetAiName();
	NewCogAiParams.AiDescription = PropertiesWidget->GetAiDescription();
	NewCogAiParams.PersonalityTraits = PropertiesWidget->GetPersonalityTraits();
	NewCogAiParams.AiVoice = PropertiesWidget->GetAiVoice();
	SetPlaceableCogAiParams(NewCogAiParams);
}

void ACogAiCharacter::SetPlaceableCogAiParams(const FPlaceableCogAiParams& InDefaultCogAiParams) {
	CurrentCogAiParams = InDefaultCogAiParams;
	Authority_SetCurrentPersonaFileUrl(CurrentCogAiParams.UrlAvatar);
	CogAiNpcSettingsComponent->UserKey = CurrentCogAiParams.OpenAIKey;
	CogAiNpcSettingsComponent->SetName(CurrentCogAiParams.AiName);
	CogAiNpcSettingsComponent->SetDescription(CurrentCogAiParams.AiDescription);
	if (!CurrentCogAiParams.AiVoice.IsEmpty()) {
		if (CurrentCogAiParams.AiVoice == "Female (Deep)") {
			CurrentCogAiVoice = ECogAiVoice::FemaleC;
		} else if (CurrentCogAiParams.AiVoice == "Female (Neutral)") {
			CurrentCogAiVoice = ECogAiVoice::FemaleE;
		} else if (CurrentCogAiParams.AiVoice == "Female (High)") {
			CurrentCogAiVoice = ECogAiVoice::FemaleF;
		} else if (CurrentCogAiParams.AiVoice == "Female (Intense)") {
			CurrentCogAiVoice = ECogAiVoice::FemaleG;
		} else if (CurrentCogAiParams.AiVoice == "Female (Bright)") {
			CurrentCogAiVoice = ECogAiVoice::FemaleH;
		} else if (CurrentCogAiParams.AiVoice == "Male (Intense)") {
			CurrentCogAiVoice = ECogAiVoice::MaleA;
		} else if (CurrentCogAiParams.AiVoice == "Male (Calm)") {
			CurrentCogAiVoice = ECogAiVoice::MaleD;
		} else if (CurrentCogAiParams.AiVoice == "Male (Bright)") {
			CurrentCogAiVoice = ECogAiVoice::MaleI;
		} else if (CurrentCogAiParams.AiVoice == "Male (Deep)") {
			CurrentCogAiVoice = ECogAiVoice::MaleJ;
		} else {
			CurrentCogAiVoice = ECogAiVoice::FemaleC;
		}
	}
}
