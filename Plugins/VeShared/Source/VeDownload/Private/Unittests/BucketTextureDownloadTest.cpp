// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "VeDownload.h"
#include "DownloadChunkSubsystem.h"
#include "Misc/AutomationTest.h"
#include "TextureLoadRequest.h"
#include "DownloadTextureSubsystem.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(BucketTextureDownloadTest, "Game.VeShared.Source.VeDownload.Private.BucketTextureDownloadTest",
								 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)


static bool bLatentCommandComplete = false;
//const FString DownloadPath = TEXT("Test10MB.bin");
static constexpr int64 FileSize = 10485760;

class FMyLatentCommand final : public IAutomationLatentCommand {
public:
	explicit FMyLatentCommand(BucketTextureDownloadTest* InputParam) : Test(InputParam) {
		FString RequestMetadataUrl = "";
		TSharedPtr<FTextureLoadRequest> Request;

		Request = MakeShared<FTextureLoadRequest>(RequestMetadataUrl, TEXTUREGROUP_World, 0);
		Request->GetOnComplete().BindRaw(this, &FMyLatentCommand::OnComplete);

		GET_MODULE_SUBSYSTEM(TextureSubsystem, Download, Texture);
		Test->TestValid("texture subsystem is valid", TextureSubsystem);
		if (TextureSubsystem) {
			TextureSubsystem->LoadTexture(Request);
		}

	}

	virtual ~FMyLatentCommand() override {
	}

	virtual bool Update() override {
		return bLatentCommandComplete;
	}

	//
	// static void OnProgress(const FChunkDownloadProgress& InProgress) {
	// 	VeLog("Test Download Progress. Id: %s, %.2f, total: %lld", *InProgress.Id.ToString(), InProgress.fProgress * 100.0f, InProgress.iTotalSize);
	// }

	void OnComplete(const FTextureLoadResult& InResult) const {
		bLatentCommandComplete = true;
		VeLog("Url file to: %s", *InResult.Url);
		VeLog("Error to: %s", *InResult.Error);
		VeLog("DiskCached to: %hhd", InResult.bDiskCached);
		Test->TestNotNull("Texture is not null", InResult.AsTexture2D());
		Test->TestTrue("Texture is not null", InResult.bDownloaded);

		//Test->TestTrue(TEXT("download successful %s"), InResult.Headers);
		//IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
		//Test->TestTrue(TEXT("file exists"), PlatformFile.FileExists(*DownloadPath));
		//Test->TestEqual(TEXT("file size match"), PlatformFile.FileSize(*DownloadPath), FileSize);
	}

private:
	BucketTextureDownloadTest* Test;
};

bool BucketTextureDownloadTest::RunTest(const FString& Parameters) {

	ADD_LATENT_AUTOMATION_COMMAND(FMyLatentCommand(this));
	// Make the test pass by returning true, or fail by returning false.
	return true;
}
