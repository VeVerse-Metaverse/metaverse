#include "Tests/MetaverseTestControllerDedicatedServerTest.h"

void UMetaverseTestControllerDedicatedServerTest::OnTick(float TimeDelta) {
	if (GetTimeInCurrentState() > 300)
	{
		UE_LOG(LogGauntlet, Error, TEXT("Failing boot test after 300 secs!"));
		EndTest(-1);
	}
	Super::OnTick(TimeDelta);
}
