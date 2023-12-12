// Author: Nikolay Bulatov, Egor A. Pristavka. 
// Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#pragma once

#include "CoreMinimal.h"
#include "ILiveLinkSource.h"
#include "VeLiveLinkSourceSettings.h"
#include "VeLiveLinkConnectionSettings.h"
#include "SocketSubsystem.h"


class ISocketSubsystem;
class UVeLiveLinkServerConnection;


enum class EVeLiveLinkSourceStatus : uint8 {
	NotListening,
	StartListening,
	Listening,
	ShuttingDown,
	Error,
};

using FVeLiveLinkConnectionPool = TMap<FGuid, TSharedRef<UVeLiveLinkServerConnection>>;

class VELIVELINK_API FVeLiveLinkSource : public ILiveLinkSource {
	friend class UVeLiveLinkServerConnection;

public:
	FVeLiveLinkSource(FVeLiveLinkConnectionSettings ConnectionSettings);
	~FVeLiveLinkSource();

	// Begin ILiveLinkSource Implementation
	virtual void ReceiveClient(ILiveLinkClient* InClient, FGuid InSourceGuid) override;
	virtual bool IsSourceStillValid() const override;
	virtual bool RequestSourceShutdown() override;
	virtual void Update() override;

	virtual FText GetSourceType() const override { return SourceType; }
	virtual FText GetSourceMachineName() const override { return SourceMachineName; }
	virtual FText GetSourceStatus() const override;

	virtual TSubclassOf<ULiveLinkSourceSettings> GetSettingsClass() const override { return UVeLiveLinkSourceSettings::StaticClass(); }

	virtual void InitializeSettings(ULiveLinkSourceSettings* InSettings) override;
	virtual void OnSettingsChanged(ULiveLinkSourceSettings* Settings, const FPropertyChangedEvent& PropertyChangedEvent) override;
	// End ILiveLinkSourceImplementation

	ILiveLinkClient* GetClient() const { return Client; }
	const FGuid& GetGuid() const { return SourceGuid; }

private:
	/** Starts listening for and accepting incoming connections */
	bool StartListening();

	/** Stops listening for and accepting incoming connections */
	bool StopListening();

	/** Accepts available connection(s) */
	void AcceptConnections();

	ISocketSubsystem* SocketSubsystem = nullptr;
	ILiveLinkClient* Client = nullptr;
	FGuid SourceGuid;

	/** Whether this listeners has begun listening */
	bool bIsListening = false;

	/** The binding socket which accepts incoming connections */
	FUniqueSocket ListenSocket = nullptr;

	/** The collection of unique connections */
	FVeLiveLinkConnectionPool Connections;

	FVeLiveLinkConnectionSettings ConnectionSettings;
	FText SourceType;
	FText SourceMachineName;

	TObjectPtr<UVeLiveLinkSourceSettings> SavedSourceSettings = nullptr;

	EVeLiveLinkSourceStatus SourceStatus = EVeLiveLinkSourceStatus::NotListening;

	FString GetAddressStr(bool bAppendPort) const;
};
