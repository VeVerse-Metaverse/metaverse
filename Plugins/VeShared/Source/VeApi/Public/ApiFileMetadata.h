// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once
#include "ApiEntityMetadata.h"

UENUM()
enum class EApiFileType : uint8 {
	ImagePreview,
	ImageFull,
	ImageAvatar,
	TextureDiffuse,
	TextureNormal,
	Model,
	Video,
	Pak,
	Pdf,
	PdfImage,
	PluginDescriptor,
	PluginContent,
	MeshAvatar,
	CharacterCustomizerSaveGame,
	Cubemap,
	RenderTargetPreview,
	PakExtraContent,
	AppLogoImage,
	AppPlaceholderImage,
	Unknown
};

UENUM()
enum class EApiFileDeploymentType : uint8 {
	None,
	Server,
	Client
};

FString VEAPI_API FileTypeToString(const EApiFileType& Type);
EApiFileType VEAPI_API FileTypeFromString(const FString& Source);

class VEAPI_API FApiFileMetadata final : public IApiMetadata {
public:
	/** Unique ID of the entity. */
	FGuid Id;

	/** Current version of the file. */
	int Version;

	/** file variation */
	int Variation;

	/** ID of uploader. */
	FGuid UploadedBy;

	/** ID of the parent entity. */
	FGuid EntityId;

	/** File cloud URL. */
	FString Url;

	/** Mime type of the file. */
	FString Mime;

	/** File create date and time. */
	FDateTime CreatedAt = FDateTime();

	/** File update date and time. */
	FDateTime UpdatedAt = FDateTime();

	/** File type. */
	EApiFileType Type;

	/** File platform. */
	FString Platform;

	/** File size. */
	int64 Size;

	/** Media file pixel width. */
	int32 Width;

	/** Media file pixel height. */
	int32 Height;

	/** Type of file "Server" or "Client" */
	EApiFileDeploymentType DeploymentType = EApiFileDeploymentType::None;

	FString OriginalPath;

	FString Hash;

	FString GetCachedFilePath() const;

	virtual bool FromJson(const TSharedPtr<FJsonObject> JsonObject) override;
};

class VEAPI_API FApiUploadFileMetadata : public IApiMetadata {
public:
	/** Id of the entity to attach the file to. */
	FGuid EntityId;

	/** Type of the file. */
	FString FileType;

	/** Path of the source file. */
	FString FilePath;

	void SetFileType(const FString& MimeType, const FString& Url);
};

class VEAPI_API FApiUploadMemoryFileMetadata final : public IApiMetadata {
public:
	/** Id of the entity to attach the file to. */
	FGuid Id;

	/** Name of the file. */
	FString FileName;
	
	/** Type of the file. */
	FString FileType;

	/** Source data of the source file. */
	TArray<uint8> FileBytes;

	/** File mime type. */
	FString MimeType;
};

class VEAPI_API FApiEntityFileLinkMetadata final : public FApiUploadFileMetadata {
public:
	/** Url to file. */
	FString File;
	
	/** File mime type. */
	FString MimeType;
};

VEAPI_API const FApiFileMetadata* FindFileMetadata(const TArray<FApiFileMetadata>& Files, EApiFileType Type);
VEAPI_API const FApiFileMetadata* FindFileMetadata(const TArray<FApiFileMetadata>& Files, const TArray<EApiFileType>& Types);

typedef TApiBatchMetadata<class FApiFileMetadata/*, class UApiFileMetadataObject*/> FApiFileBatchMetadata;
