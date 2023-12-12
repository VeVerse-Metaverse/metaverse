// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "Download1.h"
#include "IVeBaseLatentCommand.h"
#include "VeDownload.h"
#include "Misc/AutomationTest.h"
#include "Download1Subsystem.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(F1GiByteDownloadTest, "Game.VeDownload.1GiByteDownloadTest", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

class FTest1GiByteDownloadLatentCommand final : public IVeBaseLatentCommand {
	TArray<FString> TestUrls = {
		"http://127.0.0.1:3000/v2/tests/static/1GiB.bin"
	};

	TArray<TSharedPtr<FVeDownload1>> Downloads;
	int32 ProcessedUrls = 0;
	int32 LastReported = -1;

public:
	explicit FTest1GiByteDownloadLatentCommand(FAutomationTestBase* InTest, IVeBaseLatentCommand* InParentCommand = nullptr, const double InTimeout = 30.0)
		: IVeBaseLatentCommand(InTest, InParentCommand, InTimeout) { }

	virtual ~FTest1GiByteDownloadLatentCommand() override { }

	virtual bool Update() override {
		if (const int32 Current = TestUrls.Num() - ProcessedUrls; LastReported != Current) {
			Test->AddInfo(FString::Printf(TEXT("processed %d of %d, remaining %d"), ProcessedUrls, TestUrls.Num(), Current));
			LastReported = Current;
		}
		return IVeBaseLatentCommand::Update();
	}

	virtual void Run() override {
		VeLogFunc("start");

		GET_MODULE_SUBSYSTEM(DownloadSubsystem, Download, Download1);
		Test->TestValid("download subsystem", DownloadSubsystem);
		if (!DownloadSubsystem) {
			VeLogErrorFunc("no download subsystem");
			SetComplete();
			return;
		}

		for (const auto& RequestUrl : TestUrls) {
			VeLogFunc("test download url: %s", *RequestUrl);
			FDownload1RequestMetadata RequestMetadata;
			RequestMetadata.Url = RequestUrl;
			RequestMetadata.Mode = EDownload1Mode::Overwrite;
			RequestMetadata.bUseRange = false;
			RequestMetadata.Priority = 0;

			const auto Download = DownloadSubsystem->Download(RequestMetadata);

			Download->GetOnDownloadProgress().AddLambda([=](const FDownload1ProgressMetadata& Metadata) {
				VeLogFunc("on progress: %s, %.3f", *RequestUrl, Metadata.Progress);
				Test->TestTrue(TEXT("Progress.TotalSize > 0"), Metadata.ContentLength > 0);
			});

			Download->GetOnDownloadComplete().AddLambda([=](const FDownload1ResultMetadata& Metadata) {
				VeLogFunc("on complete: %s", *RequestUrl);
				Test->TestTrue(TEXT("Result.Code == Completed"), Metadata.DownloadCode == EDownload1Code::Completed);

				IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
				const bool bFileExists = PlatformFile.FileExists(*Metadata.RequestMetadata.Path);
				Test->TestTrue(FString::Printf(TEXT("FileExists at %s"), *Metadata.RequestMetadata.Path), bFileExists);

				Test->TestTrue(FString::Printf(TEXT("FileSize is %lld"), Metadata.ContentLength), PlatformFile.FileSize(*Metadata.RequestMetadata.Path) == Metadata.ContentLength);
				Test->TestTrue(TEXT("Requested ContentLength matches actual ContentLength"), Metadata.RequestMetadata.ContentLength == Metadata.ContentLength);

				ProcessedUrls++;
				if (ProcessedUrls >= TestUrls.Num()) {
					SetComplete();
				}
			});
		}
	}
};

bool F1GiByteDownloadTest::RunTest(const FString& Parameters) {
	constexpr float LatentCommandTimeout = 3600.0f;

	FAutomationTestFramework::Get().EnqueueLatentCommand(MakeShared<FTest1GiByteDownloadLatentCommand>(this, nullptr, LatentCommandTimeout));

	// Make the test pass by returning true, or fail by returning false.
	return true;
}
