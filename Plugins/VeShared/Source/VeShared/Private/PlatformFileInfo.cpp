// Author: Egor A. Pristavka

#include "PlatformFileInfo.h"

TArray<UObject*> UPlatformFileInfo::BatchToUObjectArray(TArray<FPlatformFileInfo> Entities, UObject* Parent) {
	TArray<UObject*> Objects = TArray<UObject*>();
	for (const FPlatformFileInfo& Entity : Entities) {
		UPlatformFileInfo* Object = NewObject<UPlatformFileInfo>(Parent);
		Object->Metadata = Entity;
		Objects.Add(Object);
	}
	return Objects;
}