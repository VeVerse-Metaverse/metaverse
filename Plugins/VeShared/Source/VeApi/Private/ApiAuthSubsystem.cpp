// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "ApiAuthSubsystem.h"

#include "ApiUserSubsystem.h"
#include "VeApi.h"
#include "VeEnvironmentSubsystem.h"
#include "VeMonitoring/VeMonitoringModule.h"

const FName FApiAuthSubsystem::Name = FName("AuthSubsystem");

void FApiAuthSubsystem::Initialize() {
	FString ApiKey = TEXT("");

#if UE_SERVER || WITH_SERVER_CODE
	if (IsRunningDedicatedServer()) {
		ApiKey = FVeEnvironmentSubsystem::GetEnvironmentVariable(VeEnvironment::ServerApiKey);
		if (ApiKey.IsEmpty()) {
			const FString ServerConfig = FPaths::ProjectConfigDir().Append(TEXT("DefaultServer.ini"));
			if (GConfig) {
				GConfig->GetString(TEXT("Defaults"), TEXT("ApiKey"), ApiKey, ServerConfig);
			}
		}
	}
#endif

	if (ApiKey.IsEmpty()) {
		ApiKey = LoadApiKeyFromDisk();
	}

	if (!ApiKey.IsEmpty()) {
		UserMetadata.ApiKey = ApiKey;
#if 1
		// SetUser(UserMetadata);
		// BroadcastApiUserLogin(true, FString());
#else
		// todo: REMOVE DEPRECATED CODE
		if (FVeApiModule* ApiModule = FVeApiModule::Get()) {
			if (const TSharedPtr<FApiUserSubsystem> UserSubsystem = ApiModule->GetUserSubsystem()) {
				const TSharedRef<FOnUserRequestCompleted> CallbackPtr = MakeShareable(new FOnUserRequestCompleted());
		
				CallbackPtr->BindLambda([this](const FApiUserMetadata& InMetadata, const bool bInSuccessful, const FString InError) {
					if (bInSuccessful) {
						if (FVeMonitoringModule* MonitoringModule = FVeMonitoringModule::Get()) {
							if (const TSharedPtr<FMonitoringMonitoringSubsystem> MonitoringSubsystem = MonitoringModule->GetMonitoringSubsystem()) {
								MonitoringSubsystem->SetUser({
									InMetadata.Id.ToString(EGuidFormats::DigitsWithHyphens),
									InMetadata.Email,
									InMetadata.Name
								});
							} else { LogErrorF("failed to get subsystem F%s%sSubsystem", STRINGIFY(Monitoring), STRINGIFY(Monitoring)); }
						} else { LogErrorF("failed to get module FVe%sModule", STRINGIFY(Monitoring)); }
		
						SetUser(InMetadata);
						BroadcastApiUserLogin(bInSuccessful, InError);
					} else {
						LogErrorF("failed to request: %s", *InError);
					}
				});
		
				if (!UserSubsystem->GetMe(CallbackPtr)) {
					LogErrorF("failed to request");
				}
			} else { LogErrorF("failed to get subsystem F%s%sSubsystem", STRINGIFY(Api), STRINGIFY(User)); }
		} else { LogErrorF("failed to get module FVe%sModule", STRINGIFY(Api)); };
#endif
	}
}

void FApiAuthSubsystem::Shutdown() {
}

void FApiAuthSubsystem::SetUser(const FApiUserMetadata& Metadata) {
	UserMetadata = Metadata;
	BroadcastApiUserChange();
}

void FApiAuthSubsystem::BroadcastApiUserLogin(const bool bSuccessful, const FString& Error) const {
	if (OnUserLogin.IsBound()) {
		OnUserLogin.Broadcast(UserMetadata, bSuccessful, Error);
	}
}

void FApiAuthSubsystem::BroadcastApiUserRegister(const bool bSuccessful, const FString& Error) const {
	if (OnUserRegister.IsBound()) {
		OnUserRegister.Broadcast(UserMetadata, bSuccessful, Error);
	}
}

void FApiAuthSubsystem::BroadcastApiUserLogout() const {
	if (OnUserLogout.IsBound()) {
		OnUserLogout.Broadcast();
	}
}

void FApiAuthSubsystem::BroadcastApiUserChange() const {
	if (OnUserChange.IsBound()) {
		OnUserChange.Broadcast(UserMetadata);
	}
}

void FApiAuthSubsystem::SaveApiKeyToDisk(const FString& InApiKey) {
	const FString Path = FPaths::ProjectSavedDir() / "ApiToken.dat";
	if (FPaths::FileExists(Path)) {
		IFileManager::Get().Delete(*Path);
	}

	FFileHelper::SaveStringToFile(InApiKey, *Path);
}

void FApiAuthSubsystem::DeleteApiKeyFromDisk() {
	const FString Path = FPaths::ProjectSavedDir() / "ApiToken.dat";
	if (FPaths::FileExists(Path)) {
		IFileManager::Get().Delete(*Path);
	}
}

FString FApiAuthSubsystem::LoadApiKeyFromDisk() {
	const FString Path = FPaths::ProjectSavedDir() / "ApiToken.dat";
	if (FPaths::FileExists(Path)) {
		FString Buffer;
		FFileHelper::LoadFileToString(Buffer, *Path);
		return Buffer;
	}

	return TEXT("");
}

bool FApiAuthSubsystem::Register(const FApiRegisterUserMetadata& InMetadata) {
	// Allow only one register request at a time.
	if (ApiUserRegisterRequest) {
		return false;
	}

	ApiUserRegisterRequest = MakeShared<FApiRegisterUserRequest>();

	ApiUserRegisterRequest->RequestMetadata = InMetadata;

	ApiUserRegisterRequest->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest>& InRequest, const bool bSuccessful)-> void {
		bool bOk = false;

		if (bSuccessful) {
			const auto Request = StaticCastSharedPtr<FApiRegisterUserRequest>(InRequest);
			if (Request) {
				SetUser(Request->ResponseMetadata);
				SaveApiKeyToDisk(Request->ResponseMetadata.ApiKey);
				bOk = true;
			}
		}

		BroadcastApiUserLogin(bOk, InRequest->GetErrorString());
		BroadcastApiUserRegister(bOk, InRequest->GetErrorString());

		ApiUserRegisterRequest.Reset();
	});

	return ApiUserRegisterRequest->Process();
}

// bool FApiAuthSubsystem::Login(const FApiUserLoginMetadata& InMetadata) {
// 	// Allow only one login request at a time.
// 	if (ApiUserLoginRequest) {
// 		return false;
// 	}
//
// 	ApiUserLoginRequest = MakeShared<FApiUserLoginRequest>();
//
// 	ApiUserLoginRequest->RequestMetadata = InMetadata;
//
// 	ApiUserLoginRequest->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest>& InRequest, const bool bSuccessful) -> void {
// 		bool bOk = false;
//
// 		if (bSuccessful) {
// 			const auto Request = StaticCastSharedPtr<FApiUserLoginRequest>(InRequest);
// 			if (Request) {
// 				SetUser(Request->ResponseMetadata);
// 				SaveApiKeyToDisk(Request->ResponseMetadata.ApiKey);
//
// 				if (FVeMonitoringModule* MonitoringModule = FVeMonitoringModule::Get()) {
// 					if (const TSharedPtr<FMonitoringMonitoringSubsystem> MonitoringSubsystem = MonitoringModule->GetMonitoringSubsystem()) {
// 						MonitoringSubsystem->SetUser({
// 							Request->ResponseMetadata.Id.ToString(EGuidFormats::DigitsWithHyphens),
// 							Request->ResponseMetadata.Email,
// 							Request->ResponseMetadata.Name
// 						});
// 					} else { LogErrorF("failed to get subsystem F%s%sSubsystem", STRINGIFY(Monitoring), STRINGIFY(Monitoring)); }
// 				} else { LogErrorF("failed to get module FVe%sModule", STRINGIFY(Monitoring)); }
//
// 				bOk = true;
// 			}
// 		}
//
// 		BroadcastApiUserLogin(bOk, InRequest->GetErrorString());
// 		ApiUserLoginRequest.Reset();
// 	});
//
// 	return ApiUserLoginRequest->Process();
// }

// void FApiAuthSubsystem::Logout() {
// 	SetUser(FApiUserMetadata());
// 	DeleteApiKeyFromDisk();
// 	BroadcastApiUserLogout();
// }

#if 0
void FApiAuthSubsystem::RefreshUserMetadata() {
	if (FVeApiModule* ApiModule = FVeApiModule::Get()) {
		if (const TSharedPtr<FApiUserSubsystem> UserSubsystem = ApiModule->GetUserSubsystem()) {
			const TSharedPtr<FOnUserRequestCompleted> Callback = MakeShareable(new FOnUserRequestCompleted);
			Callback->BindLambda([=](const FApiUserMetadata& InMetadata, const bool bInSuccessful, const FString& Error) {
				if (bInSuccessful) {
					SetUser(InMetadata);
				} else {
					LogErrorF("failed to refresh user metadata: %s", *Error);
				}
			});

			if (!UserSubsystem->GetMe(Callback)) {
				LogErrorF("failed to get me");
			}
		} else { LogErrorF("failed to get subsystem F%s%sSubsystem", STRINGIFY(Api), STRINGIFY(User)); }
	} else { LogErrorF("failed to get module FVe%sModule", STRINGIFY(Api)); }
}
#endif

FString FApiAuthSubsystem::GetUserApiKey() const {
	return UserMetadata.ApiKey;
}

FString FApiAuthSubsystem::GetUserEthAddress() const {
	return UserMetadata.EthAddress;
}

bool FApiAuthSubsystem::IsLoggedIn() const {
	return !UserMetadata.ApiKey.IsEmpty();
}
