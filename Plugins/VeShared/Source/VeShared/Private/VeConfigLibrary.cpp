#include "VeConfigLibrary.h"

#include "VeEnvironmentSubsystem.h"
#include "VeShared.h"

/** Name of the file to store the encoded session token. */
const TCHAR* SessionFileName = TEXT(".session.bin");


FString FVeConfigLibrary::GetString(const FString Section, const FString Key, const FString DefaultValue, const FString ConfigFile) {
	FString Value = DefaultValue;

	const FString AppFileName = FPaths::ProjectConfigDir().Append(ConfigFile);
	if (GConfig) {
		GConfig->GetString(*Section, *Key, Value, AppFileName);
	} else {
		VeLogError("GConfig failed. Section: {%s}; Key: {%s}", *Section, *Key);
	}

	return Value;
}

int32 FVeConfigLibrary::GetInt(const FString Section, const FString Key, const int32 DefaultValue, const FString ConfigFile) {
	int32 Value = DefaultValue;

	const FString AppFileName = FPaths::ProjectConfigDir().Append(ConfigFile);
	if (GConfig) {
		GConfig->GetInt(*Section, *Key, Value, AppFileName);
	} else {
		VeLogError("GConfig failed. Section: {%s}; Key: {%s}", *Section, *Key);
	}

	return Value;
}

float FVeConfigLibrary::GetFloat(const FString Section, const FString Key, const float DefaultValue, const FString ConfigFile) {
	float Value = DefaultValue;

	const FString AppFileName = FPaths::ProjectConfigDir().Append(ConfigFile);
	if (GConfig) {
		GConfig->GetFloat(*Section, *Key, Value, AppFileName);
	} else {
		VeLogError("GConfig failed. Section: {%s}; Key: {%s}", *Section, *Key);
	}

	return Value;
}

bool FVeConfigLibrary::GetBool(const FString Section, const FString Key, const bool DefaultValue, const FString ConfigFile) {
	bool Value = DefaultValue;

	const FString AppFileName = FPaths::ProjectConfigDir().Append(ConfigFile);
	if (GConfig) {
		GConfig->GetBool(*Section, *Key, Value, AppFileName);
	} else {
		VeLogError("GConfig failed. Section: {%s}; Key: {%s}", *Section, *Key);
	}

	return Value;
}

FGuid FVeConfigLibrary::GetAppId() {
	return FGuid(GetString("Default", "AppId", "c40daa25-5f5a-4394-9f49-01820204cb70"));
}

FString FVeConfigLibrary::GetAppReleaseId() {
	return GetString("Default", "AppReleaseId", "00000000-0000-0000-0000-000000000000");
}

FString FVeConfigLibrary::GetApiUrl() {
	if (const FString RootUrlEnv = FVeEnvironmentSubsystem::GetEnvironmentVariable("VE_API_ROOT_URL"); !RootUrlEnv.IsEmpty()) {
		return RootUrlEnv;
	}
	return GetString("Default", "ApiUrl", "https://api.veverse.com");
}

FString FVeConfigLibrary::GetApi2Url() {
	if (const FString RootUrlEnv = FVeEnvironmentSubsystem::GetEnvironmentVariable("VE_API2_ROOT_URL"); !RootUrlEnv.IsEmpty()) {
		return RootUrlEnv;
	}
	return GetString("Default", "Api2Url", "https://api2.veverse.com/v2");
}

FString FVeConfigLibrary::GetVersion() {
	return GetString("Default", "/Script/EngineSettings.GeneralProjectSettings", "ProjectVersion", "DefaultGame.ini");
}

FString FVeConfigLibrary::GetWebSocketsUrl() {
	return GetString("Default", "WebSocketsUrl", "ws://rpc.veverse.com/ws");
}

FString FVeConfigLibrary::GetWebSocketsProtocol() {
	auto Url = GetWebSocketsUrl();

	int32 Index = 0;
	if (Url.FindChar(TEXT(':'), Index)) {
		return Url.Left(Index);
	}

	return TEXT("ws");
}

FString FVeConfigLibrary::GetSessionFileName() {
	const FString Value = FVeEnvironmentSubsystem::GetEnvironmentVariable("USERDATA_SESSION");
	if (!Value.IsEmpty() && Value != TEXT("0")) {
		return FPaths::Combine(FPlatformProcess::UserSettingsDir(), TEXT("Metaverse"), TEXT("Saved"), SessionFileName);
	}
	return FPaths::ProjectSavedDir() / SessionFileName;
}

FString FVeConfigLibrary::GetBlockchainApiUrl() {
	if (const FString RootUrlEnv = FVeEnvironmentSubsystem::GetEnvironmentVariable("VE_BLOCKCHAIN_ROOT_URL"); !RootUrlEnv.IsEmpty()) {
		return RootUrlEnv;
	}
	return GetString("Default", "BlockchainUrl", "https://blockchain.veverse.com");
}

static bool DoesMapExist(const FString& MapName) {
	const FString MapPackageName = FString::Printf(TEXT("/Game/Maps/%s.%s"), *MapName, *MapName);
	const FString EngineMapPackageName = FString::Printf(TEXT("/Engine/Maps/%s.%s"), *MapName, *MapName);
	const FString MapPackagePath = FPackageName::LongPackageNameToFilename(MapPackageName);
	const FString EngineMapPackagePath = FPackageName::LongPackageNameToFilename(EngineMapPackageName);
	return (FPaths::FileExists(MapPackagePath) || FPaths::FileExists(EngineMapPackagePath));
}

FString FVeConfigLibrary::GetStartupMap() {
	const FString FallbackMapName = "Home";
	const FString FallbackEngineMapName = "Entry";

	if (const FString RootUrlEnv = FVeEnvironmentSubsystem::GetEnvironmentVariable("VE_STARTUP_MAP"); !RootUrlEnv.IsEmpty()) {
		return RootUrlEnv;
	}

	if (const FString MapName = GetString("Default", "StartupMap", FallbackMapName); DoesMapExist(MapName)) {
		return MapName;
	}

	return FallbackEngineMapName;
}

bool FVeConfigLibrary::GetLogHttpContent() {
	if (const FString Value = FVeEnvironmentSubsystem::GetEnvironmentVariable("LOG_HTTP_CONTENT"); !Value.IsEmpty()) {
		return Value != TEXT("0");
	}
	return false;
}

FGuid FVeConfigLibrary::GetSeverId() {
	return FGuid(FVeEnvironmentSubsystem::GetEnvironmentVariable(VeEnvironment::ServerId));
}

FGuid FVeConfigLibrary::GetSeverWorldId() {
	return FGuid(FVeEnvironmentSubsystem::GetEnvironmentVariable(VeEnvironment::ServerWorldId));
}

FString FVeConfigLibrary::GetServerApiEmail() {
	if (const FString Value = FVeEnvironmentSubsystem::GetEnvironmentVariable("VE_SERVER_API_EMAIL"); !Value.IsEmpty()) {
		return Value;
	}
	return TEXT("");
}

FString FVeConfigLibrary::GetServerApiPassword() {
	if (const FString Value = FVeEnvironmentSubsystem::GetEnvironmentVariable("VE_SERVER_API_PASSWORD"); !Value.IsEmpty()) {
		return Value;
	}
	return TEXT("");
}

FString FVeConfigLibrary::GetConfiguration() {
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
	return "Development";
#elif UE_BUILD_TEST
	return "Test";
#elif UE_BUILD_SHIPPING
	return "Shipping";
#else
	return "";
#endif
}

bool FVeConfigLibrary::GetAllowDefaultVoiceServer() {
	return GetInt("Default", "AllowDefaultVoiceServer", 1) != 0;
}

FString FVeConfigLibrary::GetOAuthHelperDir() {
	return GetString("Default", "WorkDir", "Binaries/Extras", TEXT("DefaultOAuth.ini"));
}

FString FVeConfigLibrary::GetOAuthHelperName() {
	return GetString("Default", "Path", "oauth-helper.exe", TEXT("DefaultOAuth.ini"));
}

#pragma region SDK

FString FVeConfigLibrary::GetSdkAutomationToolDir() {
	return GetString("Default", "AutomationToolDir", "Binaries/SDK", TEXT("DefaultSDK.ini"));
}

FString FVeConfigLibrary::GetSdkAutomationToolName() {
	return GetString("Default", "AutomationToolName", "sdk-automation.exe", TEXT("DefaultSDK.ini"));
}

TArray<FString> FVeConfigLibrary::GetSdkPublishExcludePaths() {
	FString ExcludePathsStr = GetString("Default", "PublishExcludePaths", "", TEXT("DefaultSDK.ini"));

	TArray<FString> List;
	ExcludePathsStr.ParseIntoArray(List, TEXT(","));
	return List;
}

#pragma endregion SDK

#pragma region Project

FString FVeConfigLibrary::GetProjectCopyright() {
	FString Result;
	GConfig->GetString(
		TEXT("/Script/EngineSettings.GeneralProjectSettings"),
		TEXT("CopyrightNotice"),
		Result,
		GGameIni
		);
	return Result;
}

FString FVeConfigLibrary::GetProjectVersion() {
	FString Result;
	GConfig->GetString(
		TEXT("/Script/EngineSettings.GeneralProjectSettings"),
		TEXT("ProjectVersion"),
		Result,
		GGameIni
		);
	return Result;
}

FString FVeConfigLibrary::GetProjectHomepage() {
	FString Result;
	GConfig->GetString(
		TEXT("/Script/EngineSettings.GeneralProjectSettings"),
		TEXT("Homepage"),
		Result,
		GGameIni
		);
	return Result;
}

FString FVeConfigLibrary::GetProjectSupportContact() {
	FString Result;
	GConfig->GetString(
		TEXT("/Script/EngineSettings.GeneralProjectSettings"),
		TEXT("SupportContact"),
		Result,
		GGameIni
		);
	return Result;
}

FGuid FVeConfigLibrary::GetDefaultVoiceServerId() {
	const FString DefaultServerIdString = GetString("Default", "DefaultVoiceServerId", "00000000-0000-0000-0000-000000000000", TEXT("DefaultApp.ini"));
	FGuid DefaultVoiceServerId;
	if (!FGuid::Parse(DefaultServerIdString, DefaultVoiceServerId)) {
		DefaultVoiceServerId = {};
	}
	return DefaultVoiceServerId;
}

#pragma endregion Project
