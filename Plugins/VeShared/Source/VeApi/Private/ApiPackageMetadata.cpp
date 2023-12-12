// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "ApiPackageMetadata.h"

#include "ApiCommon.h"
#include "ApiPackageMetadataObject.h"
#include "ApiSpaceMetadata.h"

typedef TSharedPtr<FJsonValue> FJsonValuePtr;
typedef TSharedPtr<FJsonObject> FJsonObjectPtr;
typedef TArray<TSharedPtr<FJsonValue>> FJsonValuePtrArray;

bool FApiPackageMetadata::FromJson(const TSharedPtr<FJsonObject> InJsonObject) {
	bool bOk = FApiEntityMetadata::FromJson(InJsonObject);

	Name = GetJsonStringField(InJsonObject, Api::Fields::Name);
	Title = GetJsonStringField(InJsonObject, Api::Fields::Title);
	Summary = GetJsonStringField(InJsonObject, Api::Fields::Summary);
	Description = GetJsonStringField(InJsonObject, Api::Fields::Description);

	Owner = MakeShared<FApiUserMetadata>();
	const FJsonObjectPtr OwnerJsonObject = GetJsonObjectField(InJsonObject, Api::Fields::Owner);
	if (OwnerJsonObject.IsValid()) {
		bOk &= Owner->FromJson(OwnerJsonObject);
	}

	Version = GetJsonStringField(InJsonObject, Api::Fields::Version);
	ReleaseName = GetJsonStringField(InJsonObject, Api::Fields::ReleaseName);
	Map = GetJsonStringField(InJsonObject, Api::Fields::Map);
	ReleasedAt = GetJsonDateTimeField(InJsonObject, Api::Fields::ReleasedAt);
		
	Files = TArray<FApiFileMetadata>();
	if (InJsonObject->HasTypedField<EJson::Array>(Api::Fields::Files)) {
		const FJsonValuePtrArray FileJsonObjectValues = GetJsonArrayField(InJsonObject, Api::Fields::Files);
		for (const FJsonValuePtr& JsonValue : FileJsonObjectValues) {
			if (const auto FileJsonObject = JsonValue->AsObject(); FileJsonObject.IsValid()) {
				FApiFileMetadata FileMetadata = FApiFileMetadata();
				FileMetadata.FromJson(FileJsonObject);
				Files.Add(FileMetadata);
			}
		}
	} else if (InJsonObject->HasTypedField<EJson::Object>(Api::Fields::Files)) {
		if (const auto FileJsonObject = GetJsonObjectField(InJsonObject, Api::Fields::Files); FileJsonObject->HasTypedField<EJson::Array>(Api::Fields::Entities)) {
			FApiFileBatchMetadata FileBatchMetadata = FApiFileBatchMetadata();
			FileBatchMetadata.FromJson(FileJsonObject);
			for (auto FileBatchItem : FileBatchMetadata.Entities) {
				if (FileBatchItem) {
					Files.Add(*FileBatchItem.Get());
				}
			}
		}
	}
	
	Spaces.Empty();
	const FJsonValuePtrArray SpacesJsonObject = GetJsonArrayField(InJsonObject, Api::Fields::Spaces);
	for (const FJsonValuePtr& JsonValue : SpacesJsonObject) {
		if (const FJsonObjectPtr SpaceJsonObject = JsonValue->AsObject()) {
			auto& SpaceMetadata = Spaces.Emplace_GetRef(MakeShared<FApiSpaceMetadata>());
			SpaceMetadata->FromJson(SpaceJsonObject);
		}
	}

	Tags = TArray<FString>();
	const FJsonValuePtrArray TagsJsonStringValues = GetJsonArrayField(InJsonObject, Api::Fields::Tags);
	for (const FJsonValuePtr& JsonValue : TagsJsonStringValues) {
		const FString Tag = JsonValue->AsString();
		if (!Tag.IsEmpty()) {
			Tags.Add(Tag);
		}
	}

	Views = GetJsonIntField(InJsonObject, Api::Fields::Views);
	Downloads = GetJsonIntField(InJsonObject, Api::Fields::Downloads);
	Likes = GetJsonIntField(InJsonObject, Api::Fields::Likes);
	Dislikes = GetJsonIntField(InJsonObject, Api::Fields::Dislikes);
	Price = GetJsonFloatField(InJsonObject, Api::Fields::Price);
	Discount = GetJsonFloatField(InJsonObject, Api::Fields::Discount);
	bPurchased = GetJsonBoolField(InJsonObject, Api::Fields::Purchased);
	DownloadSize = GetJsonFloatField(InJsonObject, Api::Fields::DownloadSize);

	Links = TArray<FApiLinkMetadata>();

	Platforms.Empty();
	const FJsonValuePtrArray PlatformsJsonObjects = GetJsonArrayField(InJsonObject, Api::Fields::Platforms);
	for (const FJsonValuePtr& JsonValue : PlatformsJsonObjects) {
		const FString Platform = JsonValue->AsString();
		if (Platform == TEXT("Windows")) {
			Platforms.Add(EApiPackagePlatforms::Windows);
		} else if (Platform == TEXT("Mac")) {
			Platforms.Add(EApiPackagePlatforms::Mac);
		} else if (Platform == TEXT("Linux")) {
			Platforms.Add(EApiPackagePlatforms::Linux);
		} else if (Platform == TEXT("SteamVR")) {
			Platforms.Add(EApiPackagePlatforms::SteamVR);
		} else if (Platform == TEXT("OculusVR")) {
			Platforms.Add(EApiPackagePlatforms::OculusVR);
		} else if (Platform == TEXT("OculusQuest")) {
			Platforms.Add(EApiPackagePlatforms::OculusQuest);
		} else if (Platform == TEXT("IOS")) {
			Platforms.Add(EApiPackagePlatforms::IOS);
		} else if (Platform == TEXT("Android")) {
			Platforms.Add(EApiPackagePlatforms::Android);
		}
	}

	TArray<FString> GovernanceList = {
		TEXT("Sun DAO"), TEXT("Moon DAO"), TEXT("Terra DAO"), TEXT("Miami Games LLC"), TEXT("Little Rock Games LLC"), TEXT("Palo Alto Games LLC"), TEXT("Santa Cruz Games LLC")
	};
	GovernedBy = GovernanceList[FMath::RandRange(0, GovernanceList.Num() - 1)];

	if (!GovernedBy.Contains(TEXT("LLC"))) {
		const int32 ProposalNum = FMath::RandRange(0, 5);
		Proposals.AddDefaulted(ProposalNum);
	}

	Staked = FMath::RandRange(0, 100000000);
	CreatorRewards = FMath::RandRange(0.0f, Staked * 0.1f);

	return bOk;
}

bool FApiUpdatePackageMetadata::operator==(const FApiUpdatePackageMetadata& rhsMetadata) const {
	bool Result = rhsMetadata.Id == Id &&
		rhsMetadata.Title == Title &&
		rhsMetadata.Description == Description &&
		rhsMetadata.Summary == Summary &&
		rhsMetadata.Map == Map &&
		rhsMetadata.Version == Version &&
		rhsMetadata.ReleaseName == ReleaseName &&
		rhsMetadata.bPublic == bPublic;

	if (!Result) {
		return false;
	}

	// Platforms
	if (Platforms.Num() != rhsMetadata.Platforms.Num()) {
		return false;
	}
	for (auto Platform : Platforms) {
		if (!rhsMetadata.Platforms.Contains(Platform)) {
			return false;
		}
	}

	return true;
}

bool FApiUpdatePackageMetadata::operator!=(const FApiUpdatePackageMetadata& rhsMetadata) const {
	return !(operator==(rhsMetadata));
}

FApiUpdatePackageMetadata::FApiUpdatePackageMetadata(const FApiPackageMetadata& InMetadata) {
	if (InMetadata.Id.IsValid()) {
		Id = InMetadata.Id;
	} else {
		Id = FGuid();
	}

	Title = InMetadata.Title;
	Description = InMetadata.Description;
	Summary = InMetadata.Summary;
	Map = InMetadata.Map;
	Version = InMetadata.Version;
	ReleaseName = InMetadata.ReleaseName;
	bPublic = InMetadata.bPublic;

	Platforms = InMetadata.Platforms;
	// Price = InMetadata.Price;
	// Discount = InMetadata.Discount;
}

TSharedPtr<FJsonObject> FApiUpdatePackageMetadata::ToJson() {
	const FJsonObjectPtr JsonObject = IApiMetadata::ToJson();
	JsonObject->SetStringField(Api::Fields::Title, Title);
	JsonObject->SetStringField(Api::Fields::Description, Description);
	JsonObject->SetStringField(Api::Fields::Summary, Summary);
	JsonObject->SetStringField(Api::Fields::Map, Map);
	JsonObject->SetStringField(Api::Fields::Version, Version);
	JsonObject->SetStringField(Api::Fields::ReleaseName, ReleaseName);
	JsonObject->SetBoolField(Api::Fields::Public, bPublic);

	// JsonObject->SetNumberField(Api::Fields::Price, Price);
	// JsonObject->SetNumberField(Api::Fields::Discount, Discount);

	// Platforms
	TArray<TSharedPtr<FJsonValue>> Array;
	for (const auto& Platform : Platforms) {
		FString PlatformStr;
		if (Platform == EApiPackagePlatforms::Windows) {
			PlatformStr = TEXT("Windows");
		} else if (Platform == EApiPackagePlatforms::Mac) {
			PlatformStr = TEXT("Mac");
		} else if (Platform == EApiPackagePlatforms::Linux) {
			PlatformStr = TEXT("Linux");
		} else if (Platform == EApiPackagePlatforms::SteamVR) {
			PlatformStr = TEXT("SteamVR");
		} else if (Platform == EApiPackagePlatforms::OculusVR) {
			PlatformStr = TEXT("OculusVR");
		} else if (Platform == EApiPackagePlatforms::OculusQuest) {
			PlatformStr = TEXT("OculusQuest");
		} else if (Platform == EApiPackagePlatforms::IOS) {
			PlatformStr = TEXT("IOS");
		} else if (Platform == EApiPackagePlatforms::Android) {
			PlatformStr = TEXT("Android");
		} else {
			continue;
		}
		Array.Emplace(MakeShared<FJsonValueString>(PlatformStr));
	}
	JsonObject->SetArrayField(Api::Fields::Platforms, Array);

	return JsonObject;
}
