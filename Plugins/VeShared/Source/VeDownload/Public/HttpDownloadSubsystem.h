// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once

#include "HttpDownloadTypes.h"
#include "VeShared.h"

class FHttpDownloadItem;
class FHttpDownloadTask;
class FStorageItemId;


/** Download subsystem manages parallel file downloads */
class VEDOWNLOAD_API FDownloadHttpDownloadSubsystem final : public IModuleSubsystem {
	friend FHttpDownloadItem;

	typedef TMulticastDelegate<void(FHttpDownloadItem* DownloadItem)> FOnHttpDownloadItem;

public:
	static const FName Name;
	virtual void Initialize() override;
	virtual void Shutdown() override;

public:
	/** Schedule file download */
	TSharedRef<FHttpDownloadItem> GetDownloadItem(const FStorageItemId& InStorageItemId);

private:
	/** Updates asynchronous tasks */
	void UpdateActiveDownloads(FHttpDownloadItem* NewDownloadItem = nullptr);

	/** Highest download priority. */
	static constexpr uint32 HighestPriority = static_cast<uint32>(-1);

	/** How many parallel download tasks are allowed. */
	const int32 MaxActiveDownloadNum = 10;

	/** Scheduled downloads. */
	TMap<FString, TSharedRef<FHttpDownloadItem>> ScheduledSharedItemMap;

	/** Active downloads. */
	TMap<FString, TSharedRef<FHttpDownloadItem>> ActiveSharedItemMap;

	/** Inactive downloads. */
	TMap<FString, TWeakPtr<FHttpDownloadItem>> WeakItemMap;

	/** Call from FHttpDownloadItem. Callback for the download complete events, removes the completed download from the container. */
	void Item_OnComplete(FHttpDownloadItem* InItem);

	/** Call from FHttpDownloadItem. */
	void Item_OnDestroy(FHttpDownloadItem* InItem);

	/** Call from FHttpDownloadItem. */
	void Item_OnProcess(FHttpDownloadItem* InItem);

#pragma region Events

public:
	FOnHttpDownloadItem& GetOnItemCreate() { return OnItemCreate; }
	FOnHttpDownloadItem& GetOnItemDestroy() { return OnItemDestroy; }
	FOnHttpDownloadItem& GetOnItemProcessStart() { return OnItemProcessStart; }
	FOnHttpDownloadItem& GetOnItemProcessEnd() { return OnItemProcessStop; }

private:
	FOnHttpDownloadItem OnItemCreate;
	FOnHttpDownloadItem OnItemDestroy;
	FOnHttpDownloadItem OnItemProcessStart;
	FOnHttpDownloadItem OnItemProcessStop;
#pragma endregion Events
};
