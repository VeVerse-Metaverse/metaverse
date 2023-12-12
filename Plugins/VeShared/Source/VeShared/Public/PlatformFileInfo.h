// Author: Egor A. Pristavka

#pragma once

#include "PlatformFileInfo.generated.h"

struct VESHARED_API FPlatformFileInfo {
	/** Parent directory. */
	FString Directory;

	/** Full path. */
	FString Path;

	/** File name. */
	FString Name;

	/** File extension, empty for directories. */
	FString Extension;

	/** File size. */
	int64 Size = 0;

	/** Directory or file flag. */
	bool bIsDirectory;

	/** File modification date and time. */
	FDateTime UpdatedAt;

	FORCEINLINE friend bool operator<(const FPlatformFileInfo& Lhs, const FPlatformFileInfo& Rhs) {
		return Lhs.UpdatedAt < Rhs.UpdatedAt && Lhs.Name < Rhs.Name;
	}
};


UCLASS(BlueprintType)
class VESHARED_API UPlatformFileInfo final : public UObject {
	GENERATED_BODY()

public:
	FPlatformFileInfo Metadata;

	static TArray<UObject*> BatchToUObjectArray(TArray<FPlatformFileInfo> Entities, UObject* Parent = nullptr);
};
