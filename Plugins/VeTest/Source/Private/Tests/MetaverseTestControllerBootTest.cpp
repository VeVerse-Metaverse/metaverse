#include "Tests/MetaverseTestControllerBootTest.h"

#include "VeGameMode.h"
#include "VePlayerController.h"

bool UMetaverseTestControllerBootTest::IsBootProcessComplete() const {
	if (const auto* World = GetWorld(); IsValid(World)) {
		const bool bMapNameOk = World->GetMapName() == TEXT("Home");

		const auto* GameMode = Cast<AVeGameMode>(World->GetAuthGameMode());
		const bool bGameModeOk = IsValid(GameMode);

		const auto* PlayerController = Cast<AVePlayerController>(GetFirstPlayerController());
		const bool bPlayerControllerOk = IsValid(PlayerController);

		return bMapNameOk && bGameModeOk && bPlayerControllerOk;
	}

	return false;
}

void UMetaverseTestControllerBootTest::OnTick(const float TimeDelta) {
	if (Timeout > 0) {
		static double StartTime = FPlatformTime::Seconds();

		if (const double TimeSinceStart = FPlatformTime::Seconds() - StartTime; TimeSinceStart >= Timeout) {
			UE_LOG(LogGauntlet, Error, TEXT("Failed to boot in time"));
			EndTest(-1);
		}
	}

	Super::OnTick(TimeDelta);
}
