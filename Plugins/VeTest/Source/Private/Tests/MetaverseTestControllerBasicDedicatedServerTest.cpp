#include "Tests/MetaverseTestControllerBasicDedicatedServerTest.h"
#include "Engine/World.h"

void UMetaverseTestControllerBasicDedicatedServerTest::OnTick(float TimeDelta) {
	if (GetTimeInCurrentState() > 300) {
		UE_LOG(LogGauntlet, Error, TEXT("Failing boot test after 300 secs!"));
		EndTest(-1);
	}
}

void UMetaverseTestControllerBasicDedicatedServerTest::OnPostMapChange(UWorld* World) {
	// if (IsInGame())
	// {
	EndTest(0);
	// }
}
