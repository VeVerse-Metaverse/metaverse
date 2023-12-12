// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "HttpDownloadSubsystem.h"

#include "VeShared.h"
#include "VeDownload.h"
#include "HttpDownloadItem.h"
#include "StorageItemId.h"

#undef VE_LOG_MODULE
#define VE_LOG_MODULE HttpDownloadSubsystem


const FName FDownloadHttpDownloadSubsystem::Name = FName("FDownloadDownload2Subsystem");

void FDownloadHttpDownloadSubsystem::Initialize() { }

void FDownloadHttpDownloadSubsystem::Shutdown() {
	for (const auto& Element : ScheduledSharedItemMap) {
		Element.Value->GetOnComplete().RemoveAll(this);
	}
	ScheduledSharedItemMap.Empty();

	for (const auto& Element : ActiveSharedItemMap) {
		Element.Value->GetOnComplete().RemoveAll(this);
	}
	ActiveSharedItemMap.Empty();

	for (const auto& Element : WeakItemMap) {
		if (Element.Value.IsValid()) {
			Element.Value.Pin()->GetOnComplete().RemoveAll(this);
		}
	}
	WeakItemMap.Empty();
}

TSharedRef<FHttpDownloadItem> FDownloadHttpDownloadSubsystem::GetDownloadItem(const FStorageItemId& InStorageItemId) {
	const auto& ItemKey = InStorageItemId.GetId();

	// Find existing item

	if (auto* ItemPtr = ActiveSharedItemMap.Find(ItemKey)) {
		return *ItemPtr;
	}

	if (auto* ItemPtr = ScheduledSharedItemMap.Find(ItemKey)) {
		return *ItemPtr;
	}

	if (auto* ItemPtr = WeakItemMap.Find(ItemKey)) {
		if ((*ItemPtr).IsValid()) {
			return (*ItemPtr).Pin().ToSharedRef();
		}
	}

	// Create item

	auto Item = MakeShared<FHttpDownloadItem>(this, InStorageItemId);
	WeakItemMap.Emplace(ItemKey, Item);

	OnItemCreate.Broadcast(&Item.Get());
	return Item;
}

void FDownloadHttpDownloadSubsystem::UpdateActiveDownloads(FHttpDownloadItem* NewDownloadItem) {
	// Limit active downloads number.
	if (!ScheduledSharedItemMap.Num() || ActiveSharedItemMap.Num() >= MaxActiveDownloadNum) {
		return;
	}

	VeLogTestFunc("Selecting next scheduled download");

	FString SelectedDownloadKey = "";
	TSharedPtr<FHttpDownloadItem> SelectedDownload = nullptr;

	if (ScheduledSharedItemMap.Num() == 1) {
		VeLogTestFunc("Only one scheduled task, no need to iterate");
		auto It = ScheduledSharedItemMap.CreateConstIterator();
		SelectedDownloadKey = It.Key();
		SelectedDownload = It.Value();
	} else {
		VeLogTestFunc("Iterate through scheduled downloads (%d) to select one with the highest priority", ScheduledSharedItemMap.Num());
		for (const auto& DownloadItem : ScheduledSharedItemMap) {
			if (DownloadItem.Value->IsProcessing()) {
				VeLogTestFunc("task is busy, already active: %s", *DownloadItem.Key);
				continue;
			}

			if (DownloadItem.Value->GetOptions().Priority >= HighestPriority) {
				VeLogTestFunc("download has the top priority, selecting it: %s", *DownloadItem.Key);
				SelectedDownloadKey = DownloadItem.Key;
				SelectedDownload = DownloadItem.Value;
				break;
			}

			if (!SelectedDownload) {
				VeLogTestFunc("initial download: %s", *DownloadItem.Key);
				SelectedDownloadKey = DownloadItem.Key;
				SelectedDownload = DownloadItem.Value;
				continue;
			}

			if (SelectedDownload->GetOptions().Priority < DownloadItem.Value->GetOptions().Priority) {
				VeLogTestFunc("re-selecting a higher priority task");
				SelectedDownloadKey = DownloadItem.Key;
				SelectedDownload = DownloadItem.Value;
			}
		}
	}

	if (SelectedDownload.IsValid()) {
		// Move to the active download map.
		VeLogTestFunc("move download schedule");
		ActiveSharedItemMap.Emplace(SelectedDownloadKey, SelectedDownload.ToSharedRef());
		ScheduledSharedItemMap.Remove(SelectedDownloadKey);

		// Start downloading process
		SelectedDownload->ActivateProcess();
		OnItemProcessStart.Broadcast(SelectedDownload.Get());
	}
}

void FDownloadHttpDownloadSubsystem::Item_OnComplete(FHttpDownloadItem* InItem) {
	const auto& ItemKey = InItem->GetId();

	// Move Item from SharedMap into WeakMap
	if (auto* SharedItemPtr = ActiveSharedItemMap.Find(ItemKey)) {
		if ((*SharedItemPtr).GetSharedReferenceCount() > 1) {
			WeakItemMap.Emplace(ItemKey, *SharedItemPtr);
		}
		ActiveSharedItemMap.Remove(ItemKey);
		OnItemProcessStop.Broadcast(&(*SharedItemPtr).Get());
	}

	UpdateActiveDownloads();
}

void FDownloadHttpDownloadSubsystem::Item_OnDestroy(FHttpDownloadItem* InItem) {
	const auto& ItemKey = InItem->GetId();

	WeakItemMap.Remove(ItemKey);
	OnItemCreate.Broadcast(InItem);
}

void FDownloadHttpDownloadSubsystem::Item_OnProcess(FHttpDownloadItem* InItem) {
	const auto& ItemKey = InItem->GetId();

	// Move Item from WeakMap into SharedMap
	if (auto* ItemPtr = WeakItemMap.Find(ItemKey)) {
		auto Item = (*ItemPtr).Pin().ToSharedRef();
		ScheduledSharedItemMap.Emplace(ItemKey, Item);
		WeakItemMap.Remove(ItemKey);

		UpdateActiveDownloads(InItem);
	}
}
