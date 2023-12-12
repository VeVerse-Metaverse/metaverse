#pragma once

class VESHARED_API FVeConfigLibrary {
	static FString GetString(FString Section, FString Key, FString DefaultValue = "", FString ConfigFile = TEXT("DefaultApp.ini"));
	static int32 GetInt(const FString Section, const FString Key, const int32 DefaultValue = 0, const FString ConfigFile = TEXT("DefaultApp.ini"));
	static float GetFloat(const FString Section, const FString Key, const float DefaultValue = 0.0f, const FString ConfigFile = TEXT("DefaultApp.ini"));
	static bool GetBool(const FString Section, const FString Key, const bool DefaultValue = 0.0f, const FString ConfigFile = TEXT("DefaultApp.ini"));

public:
	static FGuid GetAppId();
	static FString GetAppReleaseId();
	static FString GetApiUrl();
	static FString GetApi2Url();
	static FString GetVersion();
	static FString GetWebSocketsUrl();
	static FString GetWebSocketsProtocol();
	static FString GetSessionFileName();
	
	static FString GetBlockchainApiUrl();

	static FString GetStartupMap();
	
	static bool GetLogHttpContent();

	static FGuid GetSeverId();
	static FGuid GetSeverWorldId();
	static FString GetServerApiEmail();
	static FString GetServerApiPassword();
	static FString GetConfiguration();

	static bool GetAllowDefaultVoiceServer();

#pragma region GoogleAuth

	static FString GetOAuthHelperDir();
	static FString GetOAuthHelperName();

#pragma endregion Auth

#pragma region SDK

	static FString GetSdkAutomationToolDir();
	static FString GetSdkAutomationToolName();
	static TArray<FString> GetSdkPublishExcludePaths();

#pragma endregion SDK

#pragma region Project

	static FString GetProjectCopyright();
	static FString GetProjectVersion();
	static FString GetProjectHomepage();
	static FString GetProjectSupportContact();
	static FGuid GetDefaultVoiceServerId();

#pragma endregion Project
	
};
