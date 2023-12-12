// Authors: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022-2023 VeVerse AS. All rights reserved.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "ApiFileMetadata.h"

#include "VeShared.h"

FString FileTypeToString(const EApiFileType& Type) {
	switch (Type) {
	case EApiFileType::Model:
		return Api::FileTypes::Model;
	case EApiFileType::Video:
		return Api::FileTypes::Video;
	case EApiFileType::Pak:
		return Api::FileTypes::Pak;
	case EApiFileType::Pdf:
		return Api::FileTypes::Pdf;
	case EApiFileType::PdfImage:
		return Api::FileTypes::PdfImage;
	case EApiFileType::ImageAvatar:
		return Api::FileTypes::ImageAvatar;
	case EApiFileType::ImageFull:
		return Api::FileTypes::ImageFull;
	case EApiFileType::ImagePreview:
		return Api::FileTypes::ImagePreview;
	case EApiFileType::TextureDiffuse:
		return Api::FileTypes::TextureDiffuse;
	case EApiFileType::TextureNormal:
		return Api::FileTypes::TextureNormal;
	case EApiFileType::MeshAvatar:
		return Api::FileTypes::MeshAvatar;
	case EApiFileType::PluginDescriptor:
		return Api::FileTypes::PluginDescriptor;
	case EApiFileType::PluginContent:
		return Api::FileTypes::PluginContent;
	case EApiFileType::CharacterCustomizerSaveGame:
		return Api::FileTypes::CharacterCustomizerSaveGame;
	case EApiFileType::Cubemap:
		return Api::FileTypes::Cubemap;
	case EApiFileType::RenderTargetPreview:
		return Api::FileTypes::RenderTargetPreview;
	case EApiFileType::PakExtraContent:
		return Api::FileTypes::PakExtraContent;
	case EApiFileType::AppLogoImage:
		return Api::FileTypes::AppLogoImage;
	case EApiFileType::AppPlaceholderImage:
		return Api::FileTypes::AppPlaceholderImage;
	default:
		return Api::FileTypes::Unknown;
	}
}

EApiFileType FileTypeFromString(const FString& Source) {
	if (Source == Api::FileTypes::Model) {
		return EApiFileType::Model;
	}

	if (Source == Api::FileTypes::MeshAvatar) {
		return EApiFileType::MeshAvatar;
	}

	if (Source == Api::FileTypes::Video) {
		return EApiFileType::Video;
	}

	if (Source == Api::FileTypes::Pak) {
		return EApiFileType::Pak;
	}

	if (Source == Api::FileTypes::Pdf) {
		return EApiFileType::Pdf;
	}

	if (Source == Api::FileTypes::PdfImage) {
		return EApiFileType::PdfImage;
	}

	if (Source == Api::FileTypes::PluginDescriptor) {
		return EApiFileType::PluginDescriptor;
	}

	if (Source == Api::FileTypes::PluginContent) {
		return EApiFileType::PluginContent;
	}

	if (Source == Api::FileTypes::ImageAvatar) {
		return EApiFileType::ImageAvatar;
	}

	if (Source == Api::FileTypes::ImageFull) {
		return EApiFileType::ImageFull;
	}

	if (Source == Api::FileTypes::ImagePreview) {
		return EApiFileType::ImagePreview;
	}

	if (Source == Api::FileTypes::TextureDiffuse) {
		return EApiFileType::TextureDiffuse;
	}

	if (Source == Api::FileTypes::TextureNormal) {
		return EApiFileType::TextureNormal;
	}

	if (Source == Api::FileTypes::PluginDescriptor) {
		return EApiFileType::PluginDescriptor;
	}

	if (Source == Api::FileTypes::PluginContent) {
		return EApiFileType::PluginContent;
	}

	if (Source == Api::FileTypes::CharacterCustomizerSaveGame) {
		return EApiFileType::CharacterCustomizerSaveGame;
	}

	if (Source == Api::FileTypes::Cubemap) {
		return EApiFileType::Cubemap;
	}

	if (Source == Api::FileTypes::RenderTargetPreview) {
		return EApiFileType::RenderTargetPreview;
	}

	if (Source == Api::FileTypes::PakExtraContent) {
		return EApiFileType::PakExtraContent;
	}

	if (Source == Api::FileTypes::AppLogoImage) {
		return EApiFileType::AppLogoImage;
	}

	if (Source == Api::FileTypes::AppPlaceholderImage) {
		return EApiFileType::AppPlaceholderImage;
	}

	return EApiFileType::Unknown;
}

FString FApiFileMetadata::GetCachedFilePath() const {
	// Texture file name. 
	FString FileName;
	// Texture file extension.
	FString FileExt;
	// Texture file path.
	FString Path;

	// Extract parts from the URL.
	FPaths::Split(Url, Path, FileName, FileExt);

	FString FullFileName = FileExt.IsEmpty() ? FileName : FileName + "." + FileExt;

	const FRegexPattern RegexPattern(TEXT("^(?:https?:\\/\\/[a-z.\\-\\d]+\\/)(.+)$"));
	FRegexMatcher RegexMatcher(RegexPattern, Path);
	if (RegexMatcher.FindNext()) {
		Path = FString(TEXT("Cache/")) + RegexMatcher.GetCaptureGroup(1);
	} else {
		Path = FString(TEXT("Cache/"));
	}

	// Disk cache file path.
	const FString CachedFilePath = FPaths::Combine(FPaths::ProjectDir(), Path, FullFileName);

	return CachedFilePath;
}

bool FApiFileMetadata::FromJson(const TSharedPtr<FJsonObject> JsonObject) {
	const bool bOk = IApiMetadata::FromJson(JsonObject);

	Id = GetJsonGuidField(JsonObject, Api::Fields::Id);
	UploadedBy = GetJsonGuidField(JsonObject, Api::Fields::UploadedBy);
	EntityId = GetJsonGuidField(JsonObject, Api::Fields::EntityId);
	Url = GetJsonStringField(JsonObject, Api::Fields::Url);
	Mime = GetJsonStringField(JsonObject, Api::Fields::Mime);
	Version = GetJsonIntField(JsonObject, Api::Fields::Version);
	Variation = GetJsonIntField(JsonObject, Api::Fields::Variation);
	Platform = GetJsonStringField(JsonObject, Api::Fields::Platform);
	CreatedAt = GetJsonDateTimeField(JsonObject, Api::Fields::CreatedAt);
	UpdatedAt = GetJsonDateTimeField(JsonObject, Api::Fields::UpdatedAt);
	Type = FileTypeFromString(GetJsonStringField(JsonObject, Api::Fields::Type));
	Size = GetJsonInt64Field(JsonObject, Api::Fields::Size);
	Width = GetJsonIntField(JsonObject, Api::Fields::Width);
	Height = GetJsonIntField(JsonObject, Api::Fields::Height);
	OriginalPath = GetJsonStringField(JsonObject, Api::Fields::OriginalPath);
	Hash = GetJsonStringField(JsonObject, Api::Fields::Hash);

	const FString DeploymentTypeStr = GetJsonStringField(JsonObject, Api::Fields::DeploymentType);
	if (DeploymentTypeStr == Api::DeploymentType::Server) {
		DeploymentType = EApiFileDeploymentType::Server;
	} else if (DeploymentTypeStr == Api::DeploymentType::Client) {
		DeploymentType = EApiFileDeploymentType::Client;
	} else {
		DeploymentType = EApiFileDeploymentType::None;
	}

	return bOk;
}

void FApiUploadFileMetadata::SetFileType(const FString& MimeType, const FString& Url) {
	const auto MediaType = GetMediaType(MimeType, Url);
	if (MediaType == Api::PlaceableTypes::Video2D) {
		FileType = Api::FileTypes::Video;
	} else if (MediaType == Api::PlaceableTypes::Audio) {
		FileType = Api::FileTypes::Audio;
	} else if (MediaType == Api::PlaceableTypes::Mesh3D) {
		FileType = Api::FileTypes::Model;
	} else if (MediaType == Api::PlaceableTypes::Pdf) {
		FileType = Api::FileTypes::PdfImage;
	} else if (MediaType == Api::PlaceableTypes::Image2D) {
		FileType = Api::FileTypes::ImageFullInitial;
	}
}

const FApiFileMetadata* FindFileMetadata(const TArray<FApiFileMetadata>& Files, EApiFileType Type) {
	const FApiFileMetadata* FileMetadata = Files.FindByPredicate([Type](const FApiFileMetadata& File) {
		return File.Type == Type;
	});
	return FileMetadata;
}

const FApiFileMetadata* FindFileMetadata(const TArray<FApiFileMetadata>& Files, const TArray<EApiFileType>& Types) {
	const FApiFileMetadata* FileMetadata = nullptr;
	for (auto Type : Types) {
		FileMetadata = Files.FindByPredicate([Type](const FApiFileMetadata& File) {
			return File.Type == Type;
		});
		if (FileMetadata) {
			return FileMetadata;
		}
	}
	return nullptr;
}
