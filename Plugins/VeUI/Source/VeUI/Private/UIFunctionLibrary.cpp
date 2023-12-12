// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.


#include "UIFunctionLibrary.h"

TArray<FUIFriendInfo> UUIFunctionLibrary::SortUIFriendList(const TArray<FUIFriendInfo> ObjectArray, const bool Reversed) {
	TArray<FUIFriendInfo> Array = ObjectArray;

	if (!Reversed) {
		Array.Sort([](const FUIFriendInfo& A, const FUIFriendInfo& B) {
			if (A.Status == B.Status) {
				return A.Name.CompareTo(B.Name) < 0;
			}
			return A.Status < B.Status;
		});
	} else {
		Array.Sort([](const FUIFriendInfo& A, const FUIFriendInfo& B) {
			if (A.Status == B.Status) {
				return A.Name.CompareTo(B.Name) < 0;
			}
			return !(A.Status < B.Status);
		});
	}

	return Array;
}

TArray<FUIFriendInfo> UUIFunctionLibrary::FilterUIFriendList(const TArray<FUIFriendInfo> ObjectArray, const FText& Text, const EUIUserStatus Status) {
	const FString SearchString = Text.ToString();

	if (Status == EUIUserStatus::Any) {
		return ObjectArray.FilterByPredicate([SearchString](const FUIFriendInfo& A) {
			if (SearchString.IsEmpty()) {
				return true;
			}
			return A.Name.ToString().Contains(SearchString);
		});
	}

	return ObjectArray.FilterByPredicate([SearchString, Status](const FUIFriendInfo& A) {
		if (SearchString.IsEmpty()) {
			return A.Status == Status;
		}
		return A.Name.ToString().Contains(SearchString) && A.Status == Status ? true : false;
	});
}
