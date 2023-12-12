// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.


#pragma once

#include "VeShared.h"
#include "BaseStorageItem.h"
#include "VeDownload.h"

class FBaseStorageItem;
class FStorageItemId;

/**
 * 
 */
class VEDOWNLOAD_API FDownloadFileStorageSubsystem : public IModuleSubsystem {
	friend FBaseStorageItem;

public:
	static const FName Name;
	virtual void Initialize() override;
	virtual void Shutdown() override;

	TSharedRef<FBaseStorageItem> GetFileStorageItem(const FStorageItemId& StorageItemId);
	TSharedRef<FBaseStorageItem> GetPakStorageItem(const FStorageItemId& StorageItemId);
	TSharedRef<FBaseStorageItem> GetTextureStorageItem(const FStorageItemId& StorageItemId);

protected:
	TMap<FString, TSharedRef<FBaseStorageItem>> SharedItemMap;
	TMap<FString, TWeakPtr<FBaseStorageItem>> WeakItemMap;
	TSharedPtr<FBaseStorageItem> FindItem(const FString& Key) const;

	template <class T>
	TSharedRef<FBaseStorageItem> GetStorageItem(const FStorageItemId& StorageItemId);

private:
	/** Call from FBaseStorageItem. */
	void Item_OnComplete(FBaseStorageItem* InItem);

	/** Call from FBaseStorageItem. */
	void Item_OnDestroy(FBaseStorageItem* InItem);

	/** Call from FBaseStorageItem. */
	void Item_OnProcess(FBaseStorageItem* InItem);
};


template <class T>
TSharedRef<FBaseStorageItem> FDownloadFileStorageSubsystem::GetStorageItem(const FStorageItemId& StorageItemId) {
	const auto& ItemKey = StorageItemId.GetId();

	// Find existing item

	if (const auto Item = FindItem(ItemKey); Item.IsValid()) {
		return Item.ToSharedRef();
	}

	// Create item

	auto Item = MakeShared<T>(this, StorageItemId);
	WeakItemMap.Emplace(ItemKey, Item);

	return Item;
}
