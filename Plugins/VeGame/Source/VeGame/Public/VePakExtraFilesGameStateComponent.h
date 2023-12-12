// Author: Egor A. Pristavka. 
// Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#pragma once

#include "Components/ActorComponent.h"
#include "VeWorldMetadata.h"
#include "VePakExtraFilesGameStateComponent.generated.h"


UCLASS(NotBlueprintable)
class VEGAME_API UVePakExtraFilesGameStateComponent : public UActorComponent {
	GENERATED_BODY()

	DECLARE_EVENT(UVePakExtraFilesGameStateComponent, FPakFileEvent);

public:
	UVePakExtraFilesGameStateComponent();

	/** Called before BeginPlay. */
	virtual void  BeginInitialization();

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	bool IsComplete() const { return bComplete; }

	FPakFileEvent OnComplete;

protected:
	void Complete(bool InForceComplete);
	virtual void NativeOnComplete();

private:
	TArray<FApiFileMetadata> PakExtraContentFiles;
	int32 PakExtraContentFilesComplete = 0;
	bool bComplete = false;
	
	void CopyPakExtraContentFiles(const FGuid& PackageId);
	void CopyPakExtraContent(const FApiFileMetadata& InFileMetadata);
	void DeletePakExtraContentFiles();
	void DeletePakExtraContent(const FApiFileMetadata& InFileMetadata);
};
