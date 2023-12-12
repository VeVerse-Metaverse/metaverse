// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved. 

#include "GameFrameworkWorldSubsystem.h"

#include "GameFrameworkWorldItem.h"


const FName FGameFrameworkWorldSubsystem::Name = FName("GameFrameworkWorldSubsystem");

void FGameFrameworkWorldSubsystem::Initialize() { }

void FGameFrameworkWorldSubsystem::Shutdown() { }

TSharedRef<FGameFrameworkWorldItem> FGameFrameworkWorldSubsystem::GetItem(const FGuid& ItemId, const FApiSpaceMetadata& InMetadata) {
	if (auto* Item = ItemList.Find(ItemId)) {
		(*Item)->SetMetadata(InMetadata);
		return *Item;
	}
	auto Item = ItemList.Emplace(ItemId, MakeShared<FGameFrameworkWorldItem>(ItemId));
	Item->SetMetadata(InMetadata);
	return Item;
}

void FGameFrameworkWorldSubsystem::GetItem(const FGuid& ItemId, TSharedRef<FOnGameFrameworkWorldItemCreated> InCallback) {
	if (auto* Item = ItemList.Find(ItemId)) {
		auto& WorldItem = *Item;

		if (WorldItem->GetMetadata().Id.IsValid()) {
			InCallback->ExecuteIfBound(WorldItem, true, TEXT(""));
		} else {
			const auto RefreshCallback = MakeShared<FOnGenericRequestCompleted>();
			RefreshCallback->BindLambda([=](const bool bInSuccessful, const FString& InError) {
				InCallback->ExecuteIfBound(WorldItem, bInSuccessful, InError);
			});
			WorldItem->RefreshMetadata(RefreshCallback);
		}
		return;
	}

	auto& WorldItem = ItemList.Emplace(ItemId, MakeShared<FGameFrameworkWorldItem>(ItemId));

	const auto RefreshCallback = MakeShared<FOnGenericRequestCompleted>();
	RefreshCallback->BindLambda([=](const bool bInSuccessful, const FString& InError) {
		InCallback->ExecuteIfBound(WorldItem, bInSuccessful, InError);
	});
	WorldItem->RefreshMetadata(RefreshCallback);
}
