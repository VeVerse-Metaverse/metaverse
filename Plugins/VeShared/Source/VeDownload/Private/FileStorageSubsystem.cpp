// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.


#include "FileStorageSubsystem.h"


const FName FDownloadFileStorageSubsystem::Name = FName("FileStorageSubsystem");

void FDownloadFileStorageSubsystem::Initialize() { }

void FDownloadFileStorageSubsystem::Shutdown() {
	for (const auto& Element : SharedItemMap) {
		Element.Value->GetOnComplete().RemoveAll(this);
	}
	SharedItemMap.Empty();

	for (const auto& Element : WeakItemMap) {
		if (Element.Value.IsValid()) {
			Element.Value.Pin()->GetOnComplete().RemoveAll(this);
		}
	}
	WeakItemMap.Empty();
}

TSharedRef<FBaseStorageItem> FDownloadFileStorageSubsystem::GetFileStorageItem(const FStorageItemId& StorageItemId) {
	return GetStorageItem<FFileStorageItem>(StorageItemId);
}

TSharedRef<FBaseStorageItem> FDownloadFileStorageSubsystem::GetPakStorageItem(const FStorageItemId& StorageItemId) {
	return GetStorageItem<FPakStorageItem>(StorageItemId);
}

TSharedRef<FBaseStorageItem> FDownloadFileStorageSubsystem::GetTextureStorageItem(const FStorageItemId& StorageItemId) {
	return GetStorageItem<FTextureStorageItem>(StorageItemId);
}

TSharedPtr<FBaseStorageItem> FDownloadFileStorageSubsystem::FindItem(const FString& Key) const {
	if (auto* SharedItemPtr = SharedItemMap.Find(Key)) {
		return *SharedItemPtr;
	}

	if (auto* WeakItemPtr = WeakItemMap.Find(Key)) {
		if ((*WeakItemPtr).IsValid()) {
			return (*WeakItemPtr).Pin();
		}
	}

	return nullptr;
}

void FDownloadFileStorageSubsystem::Item_OnComplete(FBaseStorageItem* InItem) {
	const auto& ItemKey = InItem->GetId();

	// Move Item from SharedMap into WeakMap
	if (auto* SharedItemPtr = SharedItemMap.Find(ItemKey)) {
		if ((*SharedItemPtr).GetSharedReferenceCount() > 1) {
			WeakItemMap.Emplace(ItemKey, *SharedItemPtr);
		}
		SharedItemMap.Remove(ItemKey);
	}
}

void FDownloadFileStorageSubsystem::Item_OnDestroy(FBaseStorageItem* InItem) {
	const auto& ItemKey = InItem->GetId();

	WeakItemMap.Remove(ItemKey);
}

void FDownloadFileStorageSubsystem::Item_OnProcess(FBaseStorageItem* InItem) {
	const auto& ItemKey = InItem->GetId();

	// Move Item from WeakMap into SharedMap
	if (auto* ItemPtr = WeakItemMap.Find(ItemKey)) {
		auto Item = (*ItemPtr).Pin().ToSharedRef();
		SharedItemMap.Emplace(ItemKey, Item);
		WeakItemMap.Remove(ItemKey);
		InItem->ActivateProcess();
	}
}
