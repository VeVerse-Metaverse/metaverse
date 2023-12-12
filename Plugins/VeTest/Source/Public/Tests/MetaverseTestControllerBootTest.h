#pragma once

#include "GauntletTestControllerBootTest.h"
#include "MetaverseTestControllerBootTest.generated.h"

/**
 * 
 */
UCLASS()
class UMetaverseTestControllerBootTest : public UGauntletTestControllerBootTest {
	GENERATED_BODY()

protected:
	float Timeout = 10.0f;

	virtual bool IsBootProcessComplete() const override;
	virtual void OnTick(float TimeDelta) override;
};
