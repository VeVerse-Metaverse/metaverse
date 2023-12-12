// Author: Egor A. Pristavka. 
// Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#include "YoutubeMediaPlayerComponent.h"

#include "ApiEntityMetadata.h"
#include "HttpModule.h"
#include "VeShared.h"
#include "VeUrlHelper.h"
#include "VeYoutube.h"
#include "Interfaces/IHttpResponse.h"

struct FYoutubeRange : public IApiMetadata {
	int32 Start = 0;
	int32 End = 0;

	virtual bool FromJson(const TSharedPtr<FJsonObject> JsonObject) override {
		if (!JsonObject) return false;
		Start = GetJsonIntField(JsonObject, TEXT("start"));
		End = GetJsonIntField(JsonObject, TEXT("end"));
		return false; // todo
	}
};

struct FYoutubeVideoFormat : public IApiMetadata {
	int32 ITag = 0;
	FString MimeType;
	int32 BitRate = 0;
	int32 Width = 0;
	int32 Height = 0;
	uint64 LastModified = 0;
	uint64 ContentLength = 0;
	FString Quality;
	int32 Fps = 0;
	FString QualityLabel;
	FString ProjectionType;
	int32 AverageBitRate = 0;
	FString AudioQuality;
	int64 ApproxDurationMs = 0;
	int32 AudioSampleRate = 0;
	int32 AudioChannels = 0;
	FString SignatureCipher;

	virtual bool FromJson(const TSharedPtr<FJsonObject> JsonObject) override {
		if (!JsonObject) return false;
		ITag = GetJsonIntField(JsonObject, TEXT("itag"));
		MimeType = GetJsonStringField(JsonObject, TEXT("mimetype"));
		BitRate = GetJsonIntField(JsonObject, TEXT("bitrate"));
		Width = GetJsonIntField(JsonObject, TEXT("width"));
		Height = GetJsonIntField(JsonObject, TEXT("height"));
		LastModified = GetJsonInt64Field(JsonObject, TEXT("lastModified"));
		ContentLength = GetJsonInt64Field(JsonObject, TEXT("contentLength"));
		Quality = GetJsonStringField(JsonObject, TEXT("quality"));
		Fps = GetJsonIntField(JsonObject, TEXT("fps"));
		QualityLabel = GetJsonStringField(JsonObject, TEXT("qualityLabel"));
		ProjectionType = GetJsonStringField(JsonObject, TEXT("projectionType"));
		AverageBitRate = GetJsonIntField(JsonObject, TEXT("averageBitrate"));
		AudioQuality = GetJsonStringField(JsonObject, TEXT("audioQuality"));
		ApproxDurationMs = GetJsonInt64Field(JsonObject, TEXT("approxDurationMs"));
		AudioSampleRate = GetJsonIntField(JsonObject, TEXT("audioSampleRate"));
		AudioChannels = GetJsonIntField(JsonObject, TEXT("audioChannels"));
		SignatureCipher = GetJsonStringField(JsonObject, TEXT("signatureCipher"));

		return false;
		// todo
	}
};

struct FYoutubeVideoAdaptiveFormat : public IApiMetadata {
	int32 ITag = 0;
	FString MimeType;
	int32 BitRate = 0;
	int32 Width = 0;
	int32 Height = 0;
	uint64 LastModified = 0;
	uint64 ContentLength = 0;
	FString Quality;
	int32 Fps = 0;
	FString QualityLabel;
	FString ProjectionType;
	int32 AverageBitRate = 0;
	FString AudioQuality;
	int64 ApproxDurationMs = 0;
	int32 AudioSampleRate = 0;
	int32 AudioChannels = 0;
	FString SignatureCipher;

	virtual bool FromJson(const TSharedPtr<FJsonObject> JsonObject) override {
		if (!JsonObject) return false;
		ITag = GetJsonIntField(JsonObject, TEXT("itag"));
		MimeType = GetJsonStringField(JsonObject, TEXT("mimetype"));
		BitRate = GetJsonIntField(JsonObject, TEXT("bitrate"));
		Width = GetJsonIntField(JsonObject, TEXT("width"));
		Height = GetJsonIntField(JsonObject, TEXT("height"));
		LastModified = GetJsonInt64Field(JsonObject, TEXT("lastModified"));
		ContentLength = GetJsonInt64Field(JsonObject, TEXT("contentLength"));
		Quality = GetJsonStringField(JsonObject, TEXT("quality"));
		Fps = GetJsonIntField(JsonObject, TEXT("fps"));
		QualityLabel = GetJsonStringField(JsonObject, TEXT("qualityLabel"));
		ProjectionType = GetJsonStringField(JsonObject, TEXT("projectionType"));
		AverageBitRate = GetJsonIntField(JsonObject, TEXT("averageBitrate"));
		AudioQuality = GetJsonStringField(JsonObject, TEXT("audioQuality"));
		ApproxDurationMs = GetJsonInt64Field(JsonObject, TEXT("approxDurationMs"));
		AudioSampleRate = GetJsonIntField(JsonObject, TEXT("audioSampleRate"));
		AudioChannels = GetJsonIntField(JsonObject, TEXT("audioChannels"));
		SignatureCipher = GetJsonStringField(JsonObject, TEXT("signatureCipher"));

		return false;
		// todo
	}
};

struct FYoutubeStreamingDataMetadata : public IApiMetadata {
	int32 ExpiresInSeconds = 0;
	TArray<FYoutubeVideoFormat> Formats;
	TArray<FYoutubeVideoAdaptiveFormat> AdaptiveFormats;

	virtual bool FromJson(const TSharedPtr<FJsonObject> JsonObject) override {
		if (!JsonObject) return false;
		ExpiresInSeconds = GetJsonIntField(JsonObject, TEXT("expiresinSeconds"));
		auto Arr = GetJsonArrayField(JsonObject, "formats");
		FYoutubeVideoFormat Format;
		for (const auto& Item : Arr) {
			Format.FromJson(Item->AsObject());
			Formats.Add(Format);
		}
		auto Array = GetJsonArrayField(JsonObject, "adaptiveFormats");
		FYoutubeVideoAdaptiveFormat AdaptiveFormat;
		for (const auto& Item : Array) {
			AdaptiveFormat.FromJson(Item->AsObject());
			AdaptiveFormats.Add(AdaptiveFormat);
		}
		return false; // todo
	}
};

struct FYoutubeThumbnailMetadata : public IApiMetadata {
	FString Url;
	int32 Width;
	int32 Height;

	virtual bool FromJson(const TSharedPtr<FJsonObject> JsonObject) override {
		if (!JsonObject) return false;
		Url = GetJsonStringField(JsonObject, TEXT("url"));
		Width = GetJsonIntField(JsonObject, TEXT("width"));
		Height = GetJsonIntField(JsonObject, TEXT("height"));

		return false; // todo
	}
};

struct FYoutubeVideoDetailsMetadata : public IApiMetadata {
	FString VideoId;
	FString Title;
	int64 LengthSeconds = 0;
	TArray<FString> Keywords = {};
	FString ChannelId;
	FString ShortDescription;
	TArray<FYoutubeThumbnailMetadata> Thumbnails = {};
	bool bAllowRatings = true;
	int64 ViewCount = 0;
	FString Author;
	bool bIsLiveContent = false;

	virtual bool FromJson(const TSharedPtr<FJsonObject> JsonObject) override {
		if (!JsonObject) return false;
		VideoId = GetJsonStringField(JsonObject, "videoId");
		Title = GetJsonStringField(JsonObject, "title");
		LengthSeconds = GetJsonIntField(JsonObject, "title");
		auto Arr = GetJsonArrayField(JsonObject, "keywords");
		for (const auto& Item : Arr) {
			Keywords.Add(Item->AsString());
		}
		ChannelId = GetJsonStringField(JsonObject, "channelId");
		ShortDescription = GetJsonStringField(JsonObject, "shortDescription");
		// parse other fields
		return true;
	}
};

struct FYoutubeVideoMetadata : public IApiMetadata {
	FYoutubeVideoDetailsMetadata VideoDetails;
	FYoutubeStreamingDataMetadata StreamingData;
	
	virtual bool FromJson(const TSharedPtr<FJsonObject> JsonObject) override {
		if (!JsonObject) return false;
		const auto Object = GetJsonObjectField(JsonObject, "videoDetails");
		VideoDetails.FromJson(Object);
		const auto StreamingDataObject = GetJsonObjectField(JsonObject, "streamingData");
		StreamingData.FromJson(StreamingDataObject);
		return true;
	}
};

UYoutubeMediaPlayerComponent::UYoutubeMediaPlayerComponent() {
	PrimaryComponentTick.bCanEverTick = true;
}

void UYoutubeMediaPlayerComponent::BeginPlay() {
	Super::BeginPlay();
}

void UYoutubeMediaPlayerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UYoutubeMediaPlayerComponent::OpenUrl(const FString& Url) {
	RequestYoutubeVideoMetadata(Url);
}

void UYoutubeMediaPlayerComponent::RequestYoutubeVideoMetadata(const FString& Url) {
	const auto VideoId = GetVideoIdFromUrl(Url);
	if (VideoId.IsEmpty()) {
		VeLogErrorFunc("invalid url: %s", *Url);
		return;
	}

	const auto Request = FHttpModule::Get().CreateRequest();
	Request->OnProcessRequestComplete().BindWeakLambda(this, [=](FHttpRequestPtr RequestPtr, FHttpResponsePtr ResponsePtr, bool bSuccessful) {
		if (!(bSuccessful && ResponsePtr.IsValid())) {
			VeLogErrorFunc("failed to fetch %s", *Url);
		}
		if (EHttpResponseCodes::IsOk(ResponsePtr->GetResponseCode())) {
			const auto ResponseStr = ResponsePtr->GetContentAsString();
			const auto OpenToken = FString(TEXT("var ytInitialPlayerResponse = {"));
			const auto CloseToken = FString(TEXT("};"));
			// Calculate index of the JSON start.
			auto OpenIndex = ResponseStr.Find(OpenToken);
			if (OpenIndex == INDEX_NONE) {
				VeLogErrorFunc("failed to find open token: %s", *ResponseStr);
				return;
			}
			OpenIndex += OpenToken.Len() - 1;
			// Remove heading symbols until the open index.
			const auto RightStr = ResponseStr.RightChop(OpenIndex);
			// Get the JSON end index.
			auto CloseIndex = RightStr.Find(CloseToken);
			if (CloseIndex == INDEX_NONE) {
				VeLogErrorFunc("failed to find close token: %s", *ResponseStr);
				return;
			}
			CloseIndex += 1;
			// Extract the resulting JSON string.
			const auto ResultStr = RightStr.Left(CloseIndex);

			TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
			TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResultStr);

			if (!FJsonSerializer::Deserialize(Reader, JsonObject)) {
				// The deserialization failed, handle this case
			} else {
				FYoutubeVideoMetadata VideoMetadata;
				VideoMetadata.FromJson(JsonObject);
			}

		} else {
			VeLogErrorFunc("failed to fetch %s: %d %s", *Url, ResponsePtr->GetResponseCode(), *ResponsePtr->GetContentAsString());
		}
	});

	Request->SetVerb("GET");
	Request->SetURL("https://www.youtube.com/watch?v=" + VideoId);
	Request->SetHeader(TEXT("Accept"), TEXT("text/html"));
	Request->SetHeader(TEXT("Content-Type"), TEXT("text/html; charset=utf-8"));
	Request->ProcessRequest();
}

FString UYoutubeMediaPlayerComponent::GetVideoIdFromUrl(const FString& Url) {
	if (Url.Contains("youtu.be")) {
		FString Left, Right;
		if (Url.Split(".be/", &Left, &Right)) {
			return Right;
		}
		VeLogErrorFunc("failed to split string: %s", *Url);
	}

	auto Tokens = FVeUrlHelper::ParseUrlQuery(Url);
	if (const FString* VideoId = Tokens.Find("v"); VideoId) {
		return *VideoId;
	}
	return {};
}
