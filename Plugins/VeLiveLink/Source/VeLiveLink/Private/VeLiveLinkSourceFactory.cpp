// Author: Nikolay Bulatov, Egor A. Pristavka. 
// Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#include "VeLiveLinkSourceFactory.h"

#include "VeLiveLinkConnectionSettings.h"
#include "VeLiveLinkSource.h"
#include "ILiveLinkClient.h"

#define LOCTEXT_NAMESPACE "VeLiveLinkSourceFactory"

#if 1

// TSharedPtr<FVeLiveLinkSource> UVeLiveLinkSourceFactory2::Source;

TSharedPtr<FVeLiveLinkSource> UVeLiveLinkSourceFactory::CreateLiveLinkSource() {
	IModularFeatures& ModularFeatures = IModularFeatures::Get();

	if (ModularFeatures.IsModularFeatureAvailable(ILiveLinkClient::ModularFeatureName)) {
		ILiveLinkClient* LiveLinkClient = &IModularFeatures::Get().GetModularFeature<ILiveLinkClient>(ILiveLinkClient::ModularFeatureName);

		auto Source = MakeShared<FVeLiveLinkSource>(FVeLiveLinkConnectionSettings());
		LiveLinkClient->AddSource(Source);
		return Source;
	}

	return nullptr;
}

#else

UVeLiveLinkSourceFactory::FBuildCreationPanelDelegate UVeLiveLinkSourceFactory::OnBuildCreationPanel;

FText UVeLiveLinkSourceFactory::GetSourceDisplayName() const {
	return LOCTEXT("SourceDisplayName", "VeVerse Live Link");
}

FText UVeLiveLinkSourceFactory::GetSourceTooltip() const {
	return LOCTEXT("SourceTooltip", "Creates a connection to a VeVerse Live Link");
}

TSharedPtr<SWidget> UVeLiveLinkSourceFactory::BuildCreationPanel(FOnLiveLinkSourceCreated InOnLiveLinkSourceCreated) const {
	if (OnBuildCreationPanel.IsBound()) {
		return OnBuildCreationPanel.Execute(this, InOnLiveLinkSourceCreated);
	}
	return TSharedPtr<SWidget>();

}

TSharedPtr<ILiveLinkSource> UVeLiveLinkSourceFactory::CreateSource(const FString& ConnectionString) const {
	FVeLiveLinkConnectionSettings Settings;
	if (!ConnectionString.IsEmpty()) {
		// todo: check if this is the correct way to import text
		FVeLiveLinkConnectionSettings::StaticStruct()->ImportText(*ConnectionString, &Settings, nullptr, EPropertyPortFlags::PPF_None, nullptr, FVeLiveLinkConnectionSettings::StaticStruct()->GetName(), true);
	}
	return MakeShared<FVeLiveLinkSource>(Settings);
}

FString UVeLiveLinkSourceFactory::CreateConnectionString(const FVeLiveLinkConnectionSettings& Settings) {
	FString ConnectionString;
	FVeLiveLinkConnectionSettings::StaticStruct()->ExportText(ConnectionString, &Settings, nullptr, nullptr, EPropertyPortFlags::PPF_None, nullptr, true);
	return ConnectionString;
}

#endif

#undef LOCTEXT_NAMESPACE
