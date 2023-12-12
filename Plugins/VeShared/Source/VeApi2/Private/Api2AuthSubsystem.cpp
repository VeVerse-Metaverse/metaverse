// Authors: Khusan Yadgarov, Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "Api2AuthSubsystem.h"

#include "VeMonitoring/VeMonitoringModule.h"
#include "VeApi.h"
#include "VeApi2.h"
#include "Api2UserSubsystem.h"
#include "ApiAuthSubsystem.h"


FName FApi2AuthSubsystem::Name = TEXT("Api2AuthSubsystem");

/** Config file used to store the encryption key. */
const TCHAR* GameConfigFileName = TEXT("DefaultGame.ini");
/** Name of the session encryption configuration section. */
const TCHAR* GameConfigEncryptionSectionName = TEXT("Encryption");
/** Name of the session encryption configuration key. */
const TCHAR* GameConfigEncryptionKeyName = TEXT("Key");
/** Size of the encrypted data header. Header contains the size of actually encrypted data of variable length. */
constexpr auto EncryptionHeaderSize = FAES::AESBlockSize;

bool FApi2TokenMetadata::FromJson(const TSharedPtr<FJsonObject> JsonObject) {
	const bool bOk = FApiEntityMetadata::FromJson(JsonObject);
	Token = GetJsonStringField(JsonObject, TEXT("token"));
	return bOk;
}

bool FApi2UserLoginRequest::Process() {
	const auto JsonObjectPtr = RequestMetadata.ToJson();

	if (!JsonObjectPtr) {
		return false;
	}

	TArray<uint8> RequestBodyBinary;
	if (!SerializeJsonPayload(JsonObjectPtr, RequestBodyBinary)) {
		return false;
	}

	HttpRequest->SetVerb(TEXT("POST"));
	HttpRequest->SetURL(Api::GetApi2RootUrl() / TEXT("auth/login"));
	HttpRequest->SetContent(RequestBodyBinary);

	return FApi2Request::Process();
}

bool FApi2UserLoginRequest::OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) {
	if (CheckResponse()) {
		ResponseMetadata = FApi2TokenMetadata{};
		if (GetResponseStringPayload(Response, ResponseMetadata.Token)) {
			return true;
		}
	}

	return false;
}

void FApi2AuthSubsystem::Initialize() {
	// DedicatedServer authorization: FGameFrameworkServerSubsystem::Initialize
	if (!IsRunningDedicatedServer()) {
		const auto Callback = MakeShared<FOnGenericRequestCompleted2>();
		Callback->BindLambda([=](const EApi2ResponseCode InResponseCode, const FString& InError) {
			if (InResponseCode == EApi2ResponseCode::Ok) {
				SetAuthorizationState(EAuthorizationState::LoginSuccess, TEXT(""));
			} else {
				SetAuthorizationState(EAuthorizationState::Unauthenticated, InError);
			}
		});

		if (LoadSession(SessionToken)) {
			// Get user metadata
			RequestUserMetadata(Callback);
		} else {
			Callback->ExecuteIfBound(EApi2ResponseCode::Forbidden, TEXT("session token not found"));
		}
	}
}

void FApi2AuthSubsystem::Shutdown() { }

void FApi2AuthSubsystem::SetAuthorizationState(EAuthorizationState InAuthorizationState, const FString& Message) {
	AuthorizationMessage = Message;

	AuthorizationState = InAuthorizationState;
	OnChangeAuthorizationState.Broadcast(AuthorizationState);

	if (InAuthorizationState == EAuthorizationState::LoginSuccess) {
		AuthorizationState = EAuthorizationState::Authorized;
	} else if (InAuthorizationState == EAuthorizationState::LogoutSuccess) {
		AuthorizationState = EAuthorizationState::Unauthorized;
	}
}

void FApi2AuthSubsystem::SetUser(const FApiUserMetadata& InMetadata) {
	if (UserMetadata.Id == InMetadata.Id) {
		return;
	}

	UserMetadata = InMetadata;
	OnUserChanged.Broadcast(UserMetadata);

	// Set UserMetadata for APIv1
	GET_MODULE_SUBSYSTEM(ApiAuthSubsystem, Api, Auth);
	if (ApiAuthSubsystem) {
		ApiAuthSubsystem->SetUser(InMetadata);
	}
}

namespace {
	constexpr uint32 SessionFileHeaderSize = sizeof(uint32);
	constexpr uint8 SessionFileHeader[3] = {'M', 'S', 'V'};
	constexpr uint8 SessionFileCurrentVersion = 0x00;
}

bool FApi2AuthSubsystem::SaveSession(const FString& InToken) {
	if (!GConfig) {
		return false;
	}

	const auto Path = FVeConfigLibrary::GetSessionFileName();
	if (FPaths::FileExists(Path)) {
		IFileManager::Get().Delete(*Path);
	}

	const auto GameConfigPath = FPaths::ProjectConfigDir().Append(GameConfigFileName);

	FString EncryptionKey;
	if (!GConfig->GetString(GameConfigEncryptionSectionName, GameConfigEncryptionKeyName, EncryptionKey, GameConfigPath)) {
		return false;
	}

	if (EncryptionKey.Len() <= 0) {
		return false;
	}

	TArray<uint8> KeyBytes;
	FBase64::Decode(EncryptionKey, KeyBytes);
	ensure(KeyBytes.Num() == sizeof(FAES::FAESKey::Key));
	FAES::FAESKey Key;
	FMemory::Memcpy(Key.Key, &KeyBytes[0], sizeof(FAES::FAESKey::Key));

	TArray<uint8> Buffer;
	const auto TokenSize = InToken.Len();
	const auto EncodedBufferSize = EncryptionHeaderSize + TokenSize + (FAES::AESBlockSize - TokenSize % FAES::AESBlockSize);
	Buffer.SetNum(EncodedBufferSize);

	// Write buffer length as a header as a little-endian integer
	for (int i = 0; i < 4; i++) {
		Buffer[i] = TokenSize >> i * 8;
	}

	StringToBytes(InToken, Buffer.GetData() + EncryptionHeaderSize, TokenSize);
	FAES::EncryptData(Buffer.GetData(), EncodedBufferSize, Key);

	const TArray Header = {SessionFileHeader[0], SessionFileHeader[1], SessionFileHeader[2], SessionFileCurrentVersion};
	Buffer.Insert(Header, 0);

	return FFileHelper::SaveArrayToFile(Buffer, *Path);
}

bool FApi2AuthSubsystem::LoadSession(FString& OutToken) {
	if (!GConfig) {
		return false;
	}

	const auto GameConfig = FPaths::ProjectConfigDir().Append(GameConfigFileName);

	FString EncryptionKey;
	if (!GConfig->GetString(GameConfigEncryptionSectionName, GameConfigEncryptionKeyName, EncryptionKey, GameConfig)) {
		return false;
	}

	if (EncryptionKey.Len() <= 0) {
		return false;
	}

	const auto Path = FVeConfigLibrary::GetSessionFileName();
	TArray<uint8> Buffer;
	if (!FFileHelper::LoadFileToArray(Buffer, *Path)) {
		return false;
	}

	// Validate header.
	if (Buffer.Num() <= SessionFileHeaderSize) {
		VeLogWarning("invalid session file size");
		return false;
	}

	if (const uint8* Header = Buffer.GetData(); Header != 0) {
		if (!(Header[0] == SessionFileHeader[0] && Header[1] == SessionFileHeader[1] && Header[2] == SessionFileHeader[2])) {
			VeLogWarning("invalid session file");
			return false;
		}

		if (const uint8 Version = Header[3]; Version != SessionFileCurrentVersion) {
			VeLogWarning("invalid session version");
			return false;
		}
	}

	const auto EncodedString = FString::FromHexBlob(Buffer.GetData() + SessionFileHeaderSize, Buffer.Num() - SessionFileHeaderSize);
	const uint32 EncodedStringSize = EncodedString.Len();
	if (EncodedStringSize <= 0) {
		return false;
	}

	const uint32 EncodedBufferSize = EncryptionHeaderSize + EncodedStringSize - (FAES::AESBlockSize - EncodedStringSize % FAES::AESBlockSize);

	TArray<uint8> DecodedBuffer;
	DecodedBuffer.SetNum(EncodedBufferSize);
	if (!FString::ToHexBlob(EncodedString, DecodedBuffer.GetData(), EncodedBufferSize)) {
		return false;
	}

	TArray<uint8> KeyBytes;
	FBase64::Decode(EncryptionKey, KeyBytes);
	ensure(KeyBytes.Num() == sizeof(FAES::FAESKey::Key));
	FAES::FAESKey Key;
	FMemory::Memcpy(Key.Key, &KeyBytes[0], sizeof(FAES::FAESKey::Key));

	FAES::DecryptData(DecodedBuffer.GetData(), EncodedBufferSize, Key);
	const uint32 TokenSize = *static_cast<uint32*>(static_cast<void*>(DecodedBuffer.GetData()));
	OutToken = BytesToString(DecodedBuffer.GetData() + EncryptionHeaderSize, TokenSize);
	return true;
}

void FApi2AuthSubsystem::ClearSession() {
	SessionToken = {};
	const auto Path = FVeConfigLibrary::GetSessionFileName();
	if (FPaths::FileExists(Path)) {
		IFileManager::Get().Delete(*Path);
	}
}

void FApi2AuthSubsystem::Login(const FApiUserLoginMetadata& InMetadata) {
	const auto Request = MakeShared<FApi2UserLoginRequest>();
	Request->RequestMetadata = InMetadata;

	Request->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest>& InRequest, const bool bSuccessful) {
		if (!bSuccessful) {
			VeLogErrorFunc("User login request failed: %s", *InRequest->GetErrorString());
			SetUser({});
			const FString ErrStr = (InRequest->GetErrorString().IsEmpty()) ? TEXT("User login request failed") : InRequest->GetErrorString();
			SetAuthorizationState(EAuthorizationState::LoginFail, ErrStr);
			return;
		}

		if (Request->ResponseMetadata.Token.IsEmpty()) {
			VeLogErrorFunc("User login request failed: %d, no token", InRequest->GetResponseCode());
			SetUser({});
			SetAuthorizationState(EAuthorizationState::LoginFail, TEXT("Failed to get token"));
			return;
		}

		SessionToken = Request->ResponseMetadata.Token;
		SaveSession(SessionToken);

		// Get user metadata
		const auto Callback = MakeShared<FOnGenericRequestCompleted2>();
		Callback->BindLambda([=](const EApi2ResponseCode InResponseCode, const FString& InError) {
			if (InResponseCode == EApi2ResponseCode::Ok) {
				SetAuthorizationState(EAuthorizationState::LoginSuccess, TEXT(""));
			} else {
				SetAuthorizationState(EAuthorizationState::LoginFail, InError);
			}
		});
		RequestUserMetadata(Callback);
	});

	Request->Process();
}

void FApi2AuthSubsystem::LoginWithOAuthToken(const FString& InToken) {
	SessionToken = InToken;
	SaveSession(InToken);
	const auto Callback = MakeShared<FOnGenericRequestCompleted2>();
	Callback->BindLambda([=](const EApi2ResponseCode InResponseCode, const FString& InError) {
		if (InResponseCode == EApi2ResponseCode::Ok) {
			SetAuthorizationState(EAuthorizationState::LoginSuccess, TEXT(""));
		} else {
			SetAuthorizationState(EAuthorizationState::LoginFail, InError);
		}
	});
	RequestUserMetadata(Callback);
}

void FApi2AuthSubsystem::Logout() {
	ClearSession();
	SetUser({});
	SetAuthorizationState(EAuthorizationState::LogoutSuccess, TEXT(""));
}

void FApi2AuthSubsystem::RequestUserMetadata(TSharedRef<FOnGenericRequestCompleted2> InCallback) {
	GET_MODULE_SUBSYSTEM(UserSubsystem, Api2, User);
	if (!UserSubsystem) {
		SetUser({});
		InCallback->ExecuteIfBound(EApi2ResponseCode::Failed, TEXT("user susbsystem error"));
		return;
	}

	const auto Callback = MakeShared<FOnUserRequestCompleted2>();
	Callback->BindLambda([=](const FApiUserMetadata& InMetadata, const EApi2ResponseCode InResponseCode, const FString& InError) {
		if (InResponseCode == EApi2ResponseCode::Ok) {
			SetUser(InMetadata);

			// Initialize the monitoring subsystem with user information.
			GET_MODULE_SUBSYSTEM(MonitoringSubsystem, Monitoring, Monitoring);
			if (MonitoringSubsystem) {
				MonitoringSubsystem->SetUser({
					InMetadata.Id.ToString(EGuidFormats::DigitsWithHyphensLower),
					InMetadata.Email,
					InMetadata.Name
				});
			}
		} else {
			VeLogError("Failed to get user metadata: %s", *InError);
			SetUser({});
		}

		InCallback->ExecuteIfBound(InResponseCode, InError);
	});

	UserSubsystem->GetMe(Callback);
}
