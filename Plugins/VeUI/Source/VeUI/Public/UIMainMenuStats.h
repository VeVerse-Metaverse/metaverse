// 

#pragma once

#include "CoreMinimal.h"
#include "ApiStatsMetadata.h"
#include "UIWidgetBase.h"
#include "Components/TextBlock.h"
#include "UIMainMenuStats.generated.h"

enum class EStatsPage : uint8 {
	Welcome,
	LatestWorld,
	LatestPackage,
	LatestUser,
	TotalUsers,
	OnlineUsers,
	OnlineServers,
	TotalPackages,
	TotalWorlds,
	TotalObjects,
	TotalNFTs,
	TotalPlaceables,
};

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class VEUI_API UUIMainMenuStats final : public UUIWidgetBase {
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stats", meta=(BindWidgetOptional))
	UTextBlock* StatsHeaderWidget = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stats", meta=(BindWidgetOptional))
	UTextBlock* StatsTextWidget = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stats", meta=(BindWidgetOptional))
	UTextBlock* StatsAuthorWidget = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Transient, Category="Animation", meta=(BindWidgetAnimOptional))
	UWidgetAnimation* FadeAnimation = nullptr;
	/** Cached player for the current fade animation. */
	UPROPERTY()
	UUMGSequencePlayer* FadeAnimationSequencePlayer;
	FDelegateHandle FadeAnimationDelegateHandle;

	FApiStatsMetadata StatsMetadata;
	FTimerHandle StatsTimerHandle;
	FTimerDelegate StatsTimerDelegate;

	EStatsPage StatsPage = EStatsPage::Welcome;
	void UpdateStats();

	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
	virtual void NativeOnInitialized() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
};
