// Authors: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022-2023 VeVerse AS. All rights reserved.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "UIMainMenuStats.h"

#include "VeApi2.h"
#include "Api2AuthSubsystem.h"
#include "Api2StatsSubsystem.h"
#include "ApiPackageMetadata.h"
#include "ApiSpaceMetadata.h"
#include "VeShared.h"
#include "VeUI.h"
#include "Animation/UMGSequencePlayer.h"

void UUIMainMenuStats::UpdateStats() {
	if (!IsValid(StatsHeaderWidget)) {
		return;
	}

	if (!IsValid(StatsTextWidget)) {
		return;
	}

	if (!IsValid(StatsAuthorWidget)) {
		return;
	}

	StatsHeaderWidget->SetVisibility(ESlateVisibility::Collapsed);
	StatsTextWidget->SetVisibility(ESlateVisibility::Collapsed);
	StatsAuthorWidget->SetVisibility(ESlateVisibility::Collapsed);

	StatsHeaderWidget->SetText(FText{});
	StatsTextWidget->SetText(FText{});
	StatsAuthorWidget->SetText(FText{});

	// Open the next page
	switch (StatsPage) {
	case EStatsPage::Welcome: {
		StatsHeaderWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		StatsTextWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		StatsHeaderWidget->SetText(FText::FromString(TEXT("Welcome to")));
		StatsTextWidget->SetText(FText::FromString(TEXT("Metaverse")));
		break;
	}
	case EStatsPage::LatestPackage: {
		if (StatsMetadata.LatestPackage) {
			StatsHeaderWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			StatsTextWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			StatsHeaderWidget->SetText(FText::FromString(TEXT("Latest Package")));
			const auto Title = FText::FromString(StatsMetadata.LatestPackage->Name);
			StatsTextWidget->SetText(FText::Format(FText::FromString("{0}"), Title));
			if (StatsMetadata.LatestPackage->Owner) {
				StatsAuthorWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
				const auto Author = FText::FromString(StatsMetadata.LatestPackage->Owner->Name);
				StatsAuthorWidget->SetText(FText::Format(FText::FromString("by {0}"), Author));
			}
		}
		break;
	}
	case EStatsPage::LatestWorld: {
		if (StatsMetadata.LatestWorld) {
			StatsHeaderWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			StatsTextWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			StatsAuthorWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			StatsHeaderWidget->SetText(FText::FromString(TEXT("Latest World")));
			const auto Title = FText::FromString(StatsMetadata.LatestWorld->Name);
			StatsTextWidget->SetText(FText::Format(FText::FromString("{0}"), Title));
			const auto Author = FText::FromString(StatsMetadata.LatestWorld->Owner.Name);
			StatsAuthorWidget->SetText(FText::Format(FText::FromString("by {0}"), Author));
		}
		break;
	}
	case EStatsPage::OnlineServers: {
		StatsHeaderWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		StatsTextWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		StatsHeaderWidget->SetText(FText::FromString(TEXT("Online Servers")));
		auto Total = FText::AsNumber(StatsMetadata.OnlineServers);
		StatsTextWidget->SetText(FText::Format(FText::FromString("{0}"), Total));
		break;
	}
	case EStatsPage::TotalObjects: {
		StatsHeaderWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		StatsTextWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		StatsHeaderWidget->SetText(FText::FromString(TEXT("Objects Created")));
		auto Total = FText::AsNumber(StatsMetadata.TotalObjects);
		StatsTextWidget->SetText(FText::Format(FText::FromString("{0}"), Total));
		break;
	}
	case EStatsPage::TotalPackages: {
		StatsHeaderWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		StatsTextWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		StatsHeaderWidget->SetText(FText::FromString(TEXT("Packages Published")));
		auto Total = FText::AsNumber(StatsMetadata.TotalPackages);
		StatsTextWidget->SetText(FText::Format(FText::FromString("{0}"), Total));
		break;
	}
	case EStatsPage::TotalPlaceables: {
		StatsHeaderWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		StatsTextWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		StatsHeaderWidget->SetText(FText::FromString(TEXT("Objects Placed")));
		auto Total = FText::AsNumber(StatsMetadata.TotalPlaceables);
		StatsTextWidget->SetText(FText::Format(FText::FromString("{0}"), Total));
		break;
	}
	case EStatsPage::TotalUsers: {
		StatsHeaderWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		StatsTextWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		StatsHeaderWidget->SetText(FText::FromString(TEXT("Users Registered")));
		auto Total = FText::AsNumber(StatsMetadata.TotalUsers);
		StatsTextWidget->SetText(FText::Format(FText::FromString("{0}"), Total));
		break;
	}
	case EStatsPage::TotalWorlds: {
		StatsHeaderWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		StatsTextWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		StatsHeaderWidget->SetText(FText::FromString(TEXT("Worlds Created")));
		auto Total = FText::AsNumber(StatsMetadata.TotalWorlds);
		StatsTextWidget->SetText(FText::Format(FText::FromString("{0}"), Total));
		break;
	}
	case EStatsPage::TotalNFTs: {
		StatsHeaderWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		StatsTextWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		StatsHeaderWidget->SetText(FText::FromString(TEXT("Total NFTs")));
		auto Total = FText::AsNumber(StatsMetadata.TotalNFTs);
		StatsTextWidget->SetText(FText::Format(FText::FromString("{0}"), Total));
		break;
	}
	case EStatsPage::LatestUser:
		break;
	case EStatsPage::OnlineUsers:
		break;
	default:
		StatsPage = EStatsPage::Welcome;
		return;
	}

	StatsPage = static_cast<EStatsPage>(static_cast<uint8>(StatsPage) + 1);

	// if (FadeAnimation) {
	// 	FadeAnimationSequencePlayer = PlayAnimationForward(FadeAnimation);
	// 	FadeAnimationDelegateHandle = FadeAnimationSequencePlayer->OnSequenceFinishedPlaying().AddWeakLambda(this, [=](UUMGSequencePlayer& InSequencePlayer) {
	// 		
	// 	});
	// }
}

void UUIMainMenuStats::NativePreConstruct() {
	Super::NativePreConstruct();
}

void UUIMainMenuStats::NativeConstruct() {
	Super::NativeConstruct();

	GET_MODULE_SUBSYSTEM(StatsSubsystem, Api2, Stats);
	if (StatsSubsystem) {
		TSharedRef<FOnStatsRequestCompleted> Callback = MakeShared<FOnStatsRequestCompleted>();
		Callback->BindWeakLambda(this, [=](const FApiStatsMetadata& InMetadata, const EApi2ResponseCode InResponseCode, const FString& InError) {
			if (InResponseCode == EApi2ResponseCode::Ok) {
				StatsMetadata = InMetadata;
			} else {
				VeLogErrorFunc("failed to get stats");
			}
		});
		StatsSubsystem->Get(Callback);
	}

	if (auto* World = GetWorld()) {
		StatsTimerDelegate = FTimerDelegate::CreateWeakLambda(this, [=] {
			UpdateStats();
		});
		World->GetTimerManager().SetTimer(StatsTimerHandle, StatsTimerDelegate, 5.0f, true);
	}
}

void UUIMainMenuStats::NativeOnInitialized() {
	Super::NativeOnInitialized();

	GET_MODULE_SUBSYSTEM(AuthSubsystem, Api2, Auth);
	if (AuthSubsystem) {
		AuthSubsystem.Get()->GetOnUserChanged().AddWeakLambda(this, [&](const FApiUserMetadata& /* Metadata */) {

			GET_MODULE_SUBSYSTEM(StatsSubsystem, Api2, Stats);
			if (StatsSubsystem) {
				TSharedRef<FOnStatsRequestCompleted> Callback = MakeShared<FOnStatsRequestCompleted>();
				Callback->BindWeakLambda(this, [=](const FApiStatsMetadata& InMetadata, const EApi2ResponseCode InResponseCode, const FString& InError) {
					if (InResponseCode == EApi2ResponseCode::Ok) {
						StatsMetadata = InMetadata;
					} else {
						VeLogErrorFunc("failed to get stats");
					}
				});
				StatsSubsystem->Get(Callback);
			}
		});
	}
}

void UUIMainMenuStats::NativeTick(const FGeometry& MyGeometry, const float InDeltaTime) {
	Super::NativeTick(MyGeometry, InDeltaTime);
}
