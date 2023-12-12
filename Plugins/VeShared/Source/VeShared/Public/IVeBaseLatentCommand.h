#pragma once

#include "Misc/AutomationTest.h"

class IVeBaseLatentCommand : public IAutomationLatentCommand {
	double StartedRunning = 0.0;
	double Timeout = 30.0;
	bool bIsProcessing = false;
public:
	explicit IVeBaseLatentCommand(FAutomationTestBase* InTest, IVeBaseLatentCommand* InParentCommand = nullptr, const double InTimeout = 5.0)
		: StartedRunning{FPlatformTime::Seconds()}, Timeout{InTimeout}, Test{InTest}, ParentCommand{InParentCommand} {
		check(InTest);
	}

	virtual ~IVeBaseLatentCommand() override { }

protected:
	FAutomationTestBase* Test = nullptr;
	IVeBaseLatentCommand* ParentCommand = nullptr;
	bool bComplete = false;

	virtual void Run() = 0;

public:
	bool IsComplete() const {
		return bComplete;
	}

	void SetComplete() {
		bComplete = true;
	}

	virtual bool Update() override {
		if (bComplete) {
			return true;
		}

		// Timeout.
		if (FPlatformTime::Seconds() - StartedRunning >= Timeout) {
			Test->AddError(FString::Printf(TEXT("timed out after %.3f seconds"), this->Timeout));
			bComplete = true;
			return true;
		}

		// Processing status.
		if (!bIsProcessing) {
			if (this->ParentCommand) {
				if (this->ParentCommand->IsComplete()) {
					bIsProcessing = true;
					Run();
				}
			} else {
				bIsProcessing = true;
				Run();
			}
		}

		return false;
	}
};
