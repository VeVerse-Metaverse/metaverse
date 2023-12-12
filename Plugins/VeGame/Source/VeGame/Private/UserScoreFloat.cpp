// Authors: Egor A. Pristavka, Nikolay Bulatov. 
// Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "UserScoreFloat.h"

TArray<FUserScoreFloat> UUserScoreFloatFunctionLibrary::SortUserScoreFloatArray(TArray<FUserScoreFloat> UserScoreFloatArray, bool bDescending) {
	UserScoreFloatArray.Sort([bDescending](const FUserScoreFloat& A, const FUserScoreFloat& B) {
		if (bDescending) {
			return A.Score > B.Score;
		}
		return A.Score < B.Score;
	});

	return UserScoreFloatArray;
}
