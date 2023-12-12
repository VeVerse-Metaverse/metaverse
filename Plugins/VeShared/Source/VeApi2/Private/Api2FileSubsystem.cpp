// Authors: Khusan Yadgarov, Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "Api2FileSubsystem.h"

#include "VeApi2.h"
#include "GenericPlatform/GenericPlatformHttp.h"

FName FApi2FileSubsystem::Name = TEXT("Api2FileUploadSubsystem");

FApi2FileUploadRequestMetadata::FApi2FileUploadRequestMetadata(bool SetCurrentPlatform) {
	if (SetCurrentPlatform) {
		Platform = FVeApi2Module::GetPlatform();
		Deployment = FVeApi2Module::GetDeployment();
	}
}

TSharedPtr<FJsonObject> FApi2FileUploadRequestMetadata::ToJson() {
	TSharedPtr<FJsonObject> JsonObject = FApiUploadFileMetadata::ToJson();
	if (!FileType.IsEmpty()) {
		JsonObject->SetStringField(TEXT("type"), FileType);
	}
	if (!Platform.IsEmpty()) {
		JsonObject->SetStringField(TEXT("platform"), Platform);
	}
	if (!Deployment.IsEmpty()) {
		JsonObject->SetStringField(TEXT("deployment"), Deployment);
	}
	if (!FileType.IsEmpty()) {
		JsonObject->SetStringField(TEXT("originalPath"), OriginalPath);
	}
	if (!FileType.IsEmpty()) {
		JsonObject->SetStringField(TEXT("hash"), Hash);
	}
	return JsonObject;
}

FString FApi2FileUploadRequestMetadata::ToUrlParams() {
	const FString Base = FApiUploadFileMetadata::ToUrlParams();

	TArray<FString> Params;
	Params.Emplace(Base);

	if (!FileType.IsEmpty()) {
		Params.Emplace(TEXT("type=") + FGenericPlatformHttp::UrlEncode(FileType));
	}
	if (!Platform.IsEmpty()) {
		Params.Emplace(TEXT("platform=") + FGenericPlatformHttp::UrlEncode(Platform));
	}
	if (!Deployment.IsEmpty()) {
		Params.Emplace(TEXT("deployment=") + FGenericPlatformHttp::UrlEncode(Deployment));
	}
	if (!OriginalPath.IsEmpty()) {
		Params.Emplace(TEXT("original-path=") + FGenericPlatformHttp::UrlEncode(OriginalPath));
	}
	if (!Hash.IsEmpty()) {
		Params.Emplace(TEXT("hash=") + FGenericPlatformHttp::UrlEncode(Hash));
	}

	return FString::Join(Params, TEXT("&"));
}

FApi2FileUploadMemoryRequestMetadata::FApi2FileUploadMemoryRequestMetadata() {}

TSharedPtr<FJsonObject> FApi2FileUploadMemoryRequestMetadata::ToJson() {
	TSharedPtr<FJsonObject> JsonObject = IApiMetadata::ToJson();
	if (!FileType.IsEmpty()) {
		JsonObject->SetStringField(TEXT("platform"), FileType);
	}
	if (!MimeType.IsEmpty()) {
		JsonObject->SetStringField(TEXT("deployment"), MimeType);
	}
	return JsonObject;
}

FString FApi2FileUploadMemoryRequestMetadata::ToUrlParams() {

	return FString::Printf(TEXT("%s=%s&%s=%s"),
		TEXT("type"), *FGenericPlatformHttp::UrlEncode(FileType),
		TEXT("mimetype"), *FGenericPlatformHttp::UrlEncode(MimeType));
}

FApi2FileLinkRequestMetadata::FApi2FileLinkRequestMetadata() {
	Platform = FVeApi2Module::GetPlatform();
	Deployment = FVeApi2Module::GetDeployment();
}


TSharedPtr<FJsonObject> FApi2FileLinkRequestMetadata::ToJson() {
	TSharedPtr<FJsonObject> JsonObject = IApiMetadata::ToJson();

	if (!Type.IsEmpty()) {
		JsonObject->SetStringField(TEXT("type"), Type);
	}
	if (!Url.IsEmpty()) {
		JsonObject->SetStringField(TEXT("url"), Url);
	}
	if (!Mime.IsEmpty()) {
		JsonObject->SetStringField(TEXT("mime"), Mime);
	}
	if (Size != 0) {
		JsonObject->SetNumberField(TEXT("size"), Size);
	}
	if (Version != 0) {
		JsonObject->SetNumberField(TEXT("version"), Version);
	}
	if (!Platform.IsEmpty()) {
		JsonObject->SetStringField(TEXT("platform"), Platform);
	}
	if (!Deployment.IsEmpty()) {
		JsonObject->SetStringField(TEXT("deployment"), Deployment);
	}
	if (Width != 0) {
		JsonObject->SetNumberField(TEXT("width"), Width);
	}
	if (Height != 0) {
		JsonObject->SetNumberField(TEXT("height"), Height);
	}
	if (Variant != 0) {
		JsonObject->SetNumberField(TEXT("variant"), Variant);
	}

	return JsonObject;
}


bool FApi2FileUploadRequest::Process() {
	HttpRequest->SetTimeout(60.0f * 60.0f * 2.0f);
	HttpRequest->SetVerb(Api::HttpMethods::Put);
	HttpRequest->SetURL(FString::Printf(TEXT("%s?%s"), *(Api::GetEntitiesUrl2() / RequestMetadata.EntityId.ToString(EGuidFormats::DigitsWithHyphensLower) / Api::Files / Api::Upload), *RequestMetadata.ToUrlParams()));

	const FString Boundary = FString::Printf(TEXT("---------------------------%s"), TEXT("ve-boundary"));
	HttpRequest->SetHeader(Api::Headers::ContentType, FString::Printf(TEXT("%s; boundary=%s"), Api::ContentTypes::MultipartFormData, *Boundary));
	HttpRequest->SetHeader(Api::Headers::Accept, Api::ContentTypes::ApplicationJson);

	TArray<uint8> RequestContent;
	TArray<uint8> FileContent;

	const FString BoundaryBegin = "\r\n--" + Boundary + "\r\n";

#pragma region File
	if (RequestMetadata.FilePath.StartsWith("file:///")) {
		constexpr auto PrefixSize = 8;
		RequestMetadata.FilePath.RightChopInline(PrefixSize);
	}

	if (!FPaths::FileExists(RequestMetadata.FilePath)) {
		LogErrorF("file does not exist at %s", *RequestMetadata.FilePath);

		if (OnRequestComplete.IsBound()) {
			OnRequestComplete.Execute(SharedThis(this), false);
		}
		return false;
	}

	if (FFileHelper::LoadFileToArray(FileContent, *RequestMetadata.FilePath)) {
		if (FileContent.Num() <= 0) {
			LogErrorF("failed to load file from: %s", *RequestMetadata.FilePath);
			if (OnRequestComplete.IsBound()) {
				OnRequestComplete.Execute(SharedThis(this), false);
			}
			return false;
		}

		// Append start boundary.
		RequestContent.Append(reinterpret_cast<uint8*>(TCHAR_TO_ANSI(*BoundaryBegin)), BoundaryBegin.Len());

		// Append file header.
		FString FileHeader = "Content-Disposition: form-data;name=\"file\";";
		FileHeader.Append("filename=\"" + FPaths::GetCleanFilename(RequestMetadata.FilePath) + "\"\r\n");
		FileHeader.Append(FString::Printf(TEXT("Content-Type: \"%s\" \r\n\r\n"), RequestMetadata.Mime.IsEmpty() ? TEXT("binary/octet-stream") : *RequestMetadata.Mime));
		RequestContent.Append(reinterpret_cast<uint8*>(TCHAR_TO_ANSI(*FileHeader)), FileHeader.Len());

		// Append file content.
		RequestContent.Append(FileContent);
	} else {
		if (OnRequestComplete.IsBound()) {
			OnRequestComplete.Execute(SharedThis(this), false);
		}
		return false;
	}
#pragma endregion

	// Append end boundary.
	const FString BoundaryEnd = "\r\n--" + Boundary + "--\r\n";
	RequestContent.Append(reinterpret_cast<uint8*>(TCHAR_TO_ANSI(*BoundaryEnd)), BoundaryEnd.Len());

	HttpRequest->SetContent(RequestContent);
	return FApi2Request::Process();
}

bool FApi2FileUploadRequest::OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) {
	if (CheckResponse()) {
		TSharedPtr<FJsonObject> JsonPayload;
		if (GetResponseJsonObjectPayload(Response, JsonPayload)) { if (ResponseMetadata.FromJson(JsonPayload)) { return true; } }
	}
	return false;
}

bool FApi2FileUploadMemoryRequest::Process() {
	HttpRequest->SetTimeout(60.0f * 60.0f * 2.0f);
	HttpRequest->SetVerb(Api::HttpMethods::Put);
	HttpRequest->SetURL(FString::Printf(TEXT("%s?%s"), *(Api::GetEntitiesUrl2() / RequestMetadata.Id.ToString(EGuidFormats::DigitsWithHyphensLower) / Api::Files / Api::Upload), *RequestMetadata.ToUrlParams()));

	const FString Boundary = FString::Printf(TEXT("---------------------------%s"), TEXT("ve-boundary"));
	HttpRequest->SetHeader(Api::Headers::ContentType, FString::Printf(TEXT("%s; boundary=%s"), Api::ContentTypes::MultipartFormData, *Boundary));
	HttpRequest->SetHeader(Api::Headers::Accept, Api::ContentTypes::ApplicationJson);

	TArray<uint8> RequestContent;

	const FString BoundaryBegin = "\r\n--" + Boundary + "\r\n";

#pragma region File

	if (RequestMetadata.FileBytes.Num() <= 0) {
		LogErrorF("memory file invalid");
		if (OnRequestComplete.IsBound()) {
			OnRequestComplete.Execute(SharedThis(this), false);
		}
		return false;
	}

	// Append start boundary.
	RequestContent.Append(reinterpret_cast<uint8*>(TCHAR_TO_ANSI(*BoundaryBegin)), BoundaryBegin.Len());

	// Append file header.
	FString FileHeader = "Content-Disposition: form-data;name=\"file\";";
	FileHeader.Append("filename=\"" + RequestMetadata.FileName + "\"\r\n");
	FileHeader.Append("Content-Type: \"binary/octet-stream\" \r\n\r\n");
	RequestContent.Append(reinterpret_cast<uint8*>(TCHAR_TO_ANSI(*FileHeader)), FileHeader.Len());

	// Append file content.
	RequestContent.Append(RequestMetadata.FileBytes);

#pragma endregion

	// Append end boundary.
	const FString BoundaryEnd = "\r\n--" + Boundary + "--\r\n";
	RequestContent.Append(reinterpret_cast<uint8*>(TCHAR_TO_ANSI(*BoundaryEnd)), BoundaryEnd.Len());

	HttpRequest->SetContent(RequestContent);
	return FApi2Request::Process();
}

bool FApi2FileUploadMemoryRequest::OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) {
	if (CheckResponse()) {
		TSharedPtr<FJsonObject> JsonPayload;
		if (GetResponseJsonObjectPayload(Response, JsonPayload)) { if (ResponseMetadata.FromJson(JsonPayload)) { return true; } }
	}
	return false;
}

bool FApi2FileLinkRequest::Process() {
	if (RequestMetadata.Id.IsEmpty()) {
		return false;
	}

	const TSharedPtr<FJsonObject> JsonObject = RequestMetadata.ToJson();

	TArray<uint8> RequestBodyBinary;
	if (!SerializeJsonPayload(JsonObject, RequestBodyBinary)) {
		return false;
	}

	HttpRequest->SetVerb(Api::HttpMethods::Put);
	HttpRequest->SetURL(FString::Printf(TEXT("%s"), *(Api::GetEntitiesUrl2() / RequestMetadata.Id / Api::Files / Api::Link)));
	HttpRequest->SetContent(RequestBodyBinary);

	return FApi2Request::Process();
}

bool FApi2FileLinkRequest::OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) {
	bool bOk = false;

	if (CheckResponse()) {
		TSharedPtr<FJsonObject> JsonPayload;
		if (GetResponseJsonObjectPayload(Response, JsonPayload)) {
			if (ResponseMetadata.FromJson(JsonPayload)) {
				bOk = true;
			}
		}
	}

	return bOk;
}

bool FApi2FileDeleteRequest::Process() {
	if (!Id.IsValid()) {
		return false;
	}

	HttpRequest->SetVerb(Api::HttpMethods::Delete);
	HttpRequest->SetURL(Api::GetEntitiesUrl2() / Api::Files / Id.ToString(EGuidFormats::DigitsWithHyphensLower));

	return FApi2Request::Process();
}

bool FApi2FileDeleteRequest::OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) {
	return bSuccessful && CheckResponse();
}

void FApi2FileSubsystem::Shutdown() {}

void FApi2FileSubsystem::Initialize() {
	// Example:
	// TSharedPtr<FOnFileBatchRequestCompleted> Callback = MakeShared<FOnFileBatchRequestCompleted>();
	// Callback->BindLambda([=](const FApiFileBatchMetadata& InMetadata, const bool bInSuccessful, const FString& Error) {
	// 	if (bInSuccessful) {
	// 		for (const auto EntityPtr : InMetadata.Entities) {
	// 			VeLogFunc("%s", *EntityPtr->Name);
	// 		}
	// 	}
	// });
	// if (Index({}, Callback)) {}
}

void FApi2FileSubsystem::Upload(const FApi2FileUploadRequestMetadata& InMetadata, const TSharedRef<FOnFileRequestCompleted> InCallback) const {
	// Create request object.
	const auto Request = MakeShared<FApi2FileUploadRequest>();

	// Set request metadata.
	Request->RequestMetadata = InMetadata;

	// Bind request complete lambda.
	Request->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest>& InRequest, const bool bInSuccessful) {
		InCallback->ExecuteIfBound(Request->ResponseMetadata, bInSuccessful, InRequest->GetErrorString());
	});

	// Process request.
	if (!Request->Process()) {
		InCallback->ExecuteIfBound({}, false, TEXT("not processed"));
	}
}

void FApi2FileSubsystem::Upload2(const FApi2FileUploadRequestMetadata& InMetadata, const TSharedRef<FOnFileRequestCompleted2> InCallback) const {
	// Create request object.
	const auto Request = MakeShared<FApi2FileUploadRequest>();

	// Set request metadata.
	Request->RequestMetadata = InMetadata;

	Request->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest>& InRequest, const bool bInSuccessful) {
		InCallback->ExecuteIfBound(Request->ResponseMetadata, Request->GetApiResponseCode(), InRequest->GetErrorString());
	});

	if (!Request->Process()) {
		InCallback->ExecuteIfBound({}, EApi2ResponseCode::Failed, TEXT("not processed"));
	}
}

void FApi2FileSubsystem::UploadMemory(const FApi2FileUploadMemoryRequestMetadata& InMetadata, const TSharedRef<FOnFileRequestCompleted> InCallback) const {
	// Create request object.
	const auto Request = MakeShared<FApi2FileUploadMemoryRequest>();

	// Set request metadata.
	Request->RequestMetadata = InMetadata;

	// Bind request complete lambda.
	Request->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest>& InRequest, const bool bInSuccessful) {
		InCallback->ExecuteIfBound(Request->ResponseMetadata, bInSuccessful, InRequest->GetErrorString());
	});

	// Process request.
	if (!Request->Process()) {
		InCallback->ExecuteIfBound({}, false, TEXT("not processed"));
	}
}

void FApi2FileSubsystem::Link(const FApi2FileLinkRequestMetadata& InMetadata, const TSharedRef<FOnFileRequestCompleted> InCallback) const {
	// Create request object.
	const auto Request = MakeShared<FApi2FileLinkRequest>();

	// Set request metadata.
	Request->RequestMetadata = InMetadata;

	// Bind request complete lambda.
	Request->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest>& InRequest, const bool bInSuccessful) {
		InCallback->ExecuteIfBound(Request->ResponseMetadata, bInSuccessful, InRequest->GetErrorString());
	});

	// Process request.
	if (!Request->Process()) {
		InCallback->ExecuteIfBound({}, false, TEXT("not processed"));
	}
}

void FApi2FileSubsystem::Delete(const FGuid& Id, const TSharedRef<FOnGenericRequestCompleted2> InCallback) const {
	// Create request object.
	const auto Request = MakeShared<FApi2FileDeleteRequest>();

	// Set request metadata.
	Request->Id = Id;

	Request->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest>& InRequest, const bool bInSuccessful) {
		InCallback->ExecuteIfBound(Request->GetApiResponseCode(), InRequest->GetErrorString());
	});

	if (!Request->Process()) {
		InCallback->ExecuteIfBound(EApi2ResponseCode::Failed, TEXT("not processed"));
	}
}

void FApi2FileLinkRequestMetadata::SetFileType(const FString& InMimeType, const FString& InUrl) {
	const auto MediaType = GetMediaType(InMimeType, InUrl);
	if (MediaType == Api::PlaceableTypes::Video2D) {
		Type = Api::FileTypes::Video;
	} else if (MediaType == Api::PlaceableTypes::Audio) {
		Type = Api::FileTypes::Audio;
	} else if (MediaType == Api::PlaceableTypes::Mesh3D) {
		Type = Api::FileTypes::Model;
	} else if (MediaType == Api::PlaceableTypes::Pdf) {
		Type = Api::FileTypes::PdfImage;
	} else if (MediaType == Api::PlaceableTypes::Image2D) {
		Type = Api::FileTypes::ImageFullInitial;
	}
}
