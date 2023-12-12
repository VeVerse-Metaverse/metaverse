// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "VeDownload.h"
#include "DownloadChunkSubsystem.h"
#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS

IMPLEMENT_SIMPLE_AUTOMATION_TEST(ChunkDownloadTest, "Game.VeShared.Source.VeDownload.Private.ChunkDownloadTest",
								 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

FDownloadSubscriptionPtr BoundDownload;

bool bLatentCommandComplete = false;
const FString DownloadPath = TEXT("Test10MB.bin");
constexpr int64 FileSize = 10485760;

class FMyLatentCommand final : public IAutomationLatentCommand {
public:
	explicit FMyLatentCommand(ChunkDownloadTest* InputParam) : Test(InputParam) {
		const FString BucketUrl = "https://veverse-dev.s3.us-west-2.amazonaws.com/";
		const FString EntityId = "9546833c-1b97-4244-87a8-8ef10b8f6c86";
		const FString FileId = "cb0c4bf9-b85d-4d7b-b098-0ed1c6e0fe09";
		const FString FileUrl = BucketUrl + EntityId / FileId;

		GET_MODULE_SUBSYSTEM(ChunkSubsystem, Download, Chunk);
		Test->TestValid("chunk subsystem is valid", ChunkSubsystem);
		if (ChunkSubsystem) {
			BoundDownload = ChunkSubsystem->DownloadFile({FGuid{FileId}, FileUrl, DownloadPath}, 0, true);
			Test->TestValid(TEXT("bound download is valid"), BoundDownload);
			if (!BoundDownload.IsValid()) {
				bLatentCommandComplete = true;
			}
			BoundDownload->OnProgress().BindStatic(&FMyLatentCommand::OnProgress);
			BoundDownload->OnComplete().BindRaw(this, &FMyLatentCommand::OnComplete);
		} else {
			bLatentCommandComplete = true;
		}
	}

	virtual ~FMyLatentCommand() override {
	}

	virtual bool Update() override {
		return bLatentCommandComplete;
	}

	static void OnProgress(const FChunkDownloadProgress& InProgress) {
		VeLog("Test Download Progress. Id: %s, %.2f, total: %lld", *InProgress.Id.ToString(), InProgress.fProgress * 100.0f, InProgress.iTotalSize);
	}

	void OnComplete(const FFileDownloadResult& InResult) const {
		bLatentCommandComplete = true;
		VeLog("Downloaded to: %s", *FPaths::ConvertRelativePathToFull(DownloadPath));
		Test->TestTrue(TEXT("download successful"), InResult.bSuccessful);
		IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
		Test->TestTrue(TEXT("file exists"), PlatformFile.FileExists(*DownloadPath));
		Test->TestEqual(TEXT("file size match"), PlatformFile.FileSize(*DownloadPath), FileSize);
	}

private:
	ChunkDownloadTest* Test;
};

bool ChunkDownloadTest::RunTest(const FString& Parameters) {

	ADD_LATENT_AUTOMATION_COMMAND(FMyLatentCommand(this));

	// Make the test pass by returning true, or fail by returning false.
	return true;
}

#endif
