#pragma once

#include "GauntletTestController.h"
#include "MetaverseTestControllerBase.generated.h"

UCLASS()
class UMetaverseTestControllerBase : public UGauntletTestController, public TSharedFromThis<UMetaverseTestControllerBase> {
	GENERATED_BODY()

public:
	virtual void OnInit() override;
	virtual void OnPostMapChange(UWorld* World) override;

protected:
	virtual void OnTick(float TimeDelta) override;
};
