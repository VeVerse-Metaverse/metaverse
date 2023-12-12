// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "IVeBaseLatentCommand.h"
#include "VeDownload.h"
#include "Misc/AutomationTest.h"
#include "Tests/AutomationCommon.h"
#include "TextureLoadRequest.h"
#include "DownloadTextureSubsystem.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(GenericTextureDownloadTest, "Game.VeShared.Source.VeDownload.Private.GenericTextureDownloadTest",
								 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

#if 0
class IVeBaseLatentCommand : public IAutomationLatentCommand {
	double StartedRunning = 0.0;
	double Timeout = 5.0;
	bool bIsProcessing = false;
public:
	explicit IVeBaseLatentCommand(FAutomationTestBase* InTest, IVeBaseLatentCommand* InParentCommand = nullptr, const double InTimeout = 5.0):
		StartedRunning{FPlatformTime::Seconds()}, Timeout{InTimeout}, Test{InTest}, ParentCommand{InParentCommand} {
		check(InTest);
	}

	virtual ~IVeBaseLatentCommand() override {
	}

protected:
	FAutomationTestBase* Test = nullptr;
	IVeBaseLatentCommand* ParentCommand = nullptr;

	bool bComplete = false;

	virtual void Process() = 0;

public:
	bool IsComplete() const {
		return bComplete;
	}

	virtual bool Update() override {
		if (bComplete) {
			return true;
		}

		if (FPlatformTime::Seconds() - StartedRunning >= Timeout) {
			Test->AddError(FString::Printf(TEXT("timed out after %.3f seconds"), this->Timeout));
			bComplete = true;
			return true;
		}

		if (!bIsProcessing) {
			if (this->ParentCommand) {
				if (this->ParentCommand->IsComplete()) {
					bIsProcessing = true;
					Process();
				}
			} else {
				bIsProcessing = true;
				Process();
			}
		}

		return false;
	}
};
#endif

class FTestDownloadRandomTextureLatentCommand final : public IVeBaseLatentCommand {
	FGuid RandomId = {};

public:
	explicit FTestDownloadRandomTextureLatentCommand(FAutomationTestBase* InTest, IVeBaseLatentCommand* InParentCommand = nullptr, const double InTimeout = 10.0,
													 const FGuid& InRandomId = FGuid::NewGuid()) :
		IVeBaseLatentCommand(InTest, InParentCommand, InTimeout), RandomId{InRandomId} {
	}

	virtual void Run() override {
		VeLogFunc("0: start");

		GET_MODULE_SUBSYSTEM(TextureSubsystem, Download, Texture);
		Test->TestValid("texture subsystem is valid", TextureSubsystem);
		if (!TextureSubsystem) {
			VeLogErrorFunc("0: subsystem");
			this->bComplete = true;
			return;
		}

		FString RequestMetadataUrl = FString::Printf(TEXT("https://www.gravatar.com/avatar/%s?s=256&d=identicon&r=PG"), *this->RandomId.ToString(EGuidFormats::Digits).ToLower());
		VeLogFunc("1: url: %s", *RequestMetadataUrl);

		const auto Request = MakeShared<FTextureLoadRequest>(RequestMetadataUrl);
		Request->GetOnComplete().BindLambda([=](const FTextureLoadResult& InResult) {
			VeLogFunc("3: outer lambda");

			Test->TestTrue(TEXT("InResult.bSuccessful"), InResult.bSuccessful);
			Test->TestTrue(TEXT("InResult.Error.IsEmpty()"), InResult.Error.IsEmpty());
			Test->TestTrue(TEXT("InResult.bRemote"), InResult.bDownloaded);
			Test->TestNotNull(TEXT("texture"), InResult.AsTexture2D());

			// VeLogFunc("4: inner request");
			// const auto InnerRequest = MakeShared<FTextureLoadRequest>(RequestMetadataUrl);
			// InnerRequest->GetOnComplete().BindLambda([=](const FTextureLoadResult& InInnerResult) {
			// 	VeLogFunc("6: inner lambda");
			//
			// 	Test->TestTrue(TEXT("InInnerResult.bSuccessful"), InInnerResult.bSuccessful);
			// 	Test->TestTrue(TEXT("InResult.Error.IsEmpty()"), InResult.Error.IsEmpty());
			// 	Test->TestTrue(TEXT("InInnerResult.bMemoryCached"), InInnerResult.bMemoryCached);
			// 	Test->TestNotNull(TEXT("InResult.AsTexture2D()"), InResult.AsTexture2D());
			//
			// 	this->bComplete = true;
			// });

			// VeLogFunc("5: before request");
			// const auto bInnerResult = TextureSubsystem->LoadTexture(InnerRequest);
			// Test->TestTrue(TEXT(""), bInnerResult);
			// if (!bInnerResult) {
			// 	this->bComplete = true;
			// 	VeLogErrorFunc("5: load texture");
			// }
		});


		VeLogFunc("2: before request");
		const auto bResult = TextureSubsystem->LoadTexture(Request);
		Test->TestTrue(TEXT(""), bResult);
		if (!bResult) {
			this->bComplete = true;
			VeLogErrorFunc("2: load texture");
		}
	}

	virtual ~FTestDownloadRandomTextureLatentCommand() override {
	}
};

class FTestGetCachedRandomTextureLatentCommand final : public IVeBaseLatentCommand {
public:
	explicit FTestGetCachedRandomTextureLatentCommand(FAutomationTestBase* InTest, IVeBaseLatentCommand* InParentCommand = nullptr, const double InTimeout = 10.0) :
		IVeBaseLatentCommand(InTest, InParentCommand, InTimeout) {
	}

	virtual ~FTestGetCachedRandomTextureLatentCommand() override {
	}

	virtual void Run() override {
		// do something
		this->bComplete = true;
	}

	virtual bool Update() override {
		IVeBaseLatentCommand::Update();
		
		if (bComplete) {
			return true;
		}

		return false;
	}
};

bool GenericTextureDownloadTest::RunTest(const FString& Parameters) {
	const auto RandomId = FGuid::NewGuid();
	constexpr float LatentCommandTimeout = 10.0f;

	const auto AwaitFor = MakeShared<FTestDownloadRandomTextureLatentCommand>(this, nullptr, LatentCommandTimeout, RandomId);
	FAutomationTestFramework::Get().EnqueueLatentCommand(AwaitFor);

	const auto CommandThatWaits = MakeShared<FTestGetCachedRandomTextureLatentCommand>(this, nullptr, LatentCommandTimeout);
	FAutomationTestFramework::Get().EnqueueLatentCommand(CommandThatWaits);

	// Make the test pass by returning true, or fail by returning false.
	return true;
}
