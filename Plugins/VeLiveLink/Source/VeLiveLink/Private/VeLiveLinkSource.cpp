// Author: Nikolay Bulatov, Egor A. Pristavka. 
// Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#include "VeLiveLinkSource.h"

// #include "VeShared.h"
#include "VeLiveLink.h"
#include "ILiveLinkClient.h"
#include "VeLiveLinkServerConnection.h"
#include "Sockets.h"

#define LOCTEXT_NAMESPACE "VeLiveLinkSource"


FVeLiveLinkSource::FVeLiveLinkSource(FVeLiveLinkConnectionSettings InConnectionSettings)
	: SocketSubsystem(ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM))
	  , ConnectionSettings(MoveTemp(InConnectionSettings)) {

	SourceStatus = EVeLiveLinkSourceStatus::NotListening;

	SourceMachineName = FText::Format(LOCTEXT("VeVerseMachineName", "{0}:{1}"), FText::FromString(ConnectionSettings.BindAddress), FText::AsNumber(ConnectionSettings.ListenPort, &FNumberFormattingOptions::DefaultNoGrouping()));
	SourceType = LOCTEXT("VeVerseSourceType", "VeVerse");
}

FVeLiveLinkSource::~FVeLiveLinkSource() {
	bool bIsReadyToShutdown = false;
	while (!bIsReadyToShutdown) {
		bIsReadyToShutdown = StopListening();
	}
}

void FVeLiveLinkSource::ReceiveClient(ILiveLinkClient* InClient, FGuid InSourceGuid) {
	Client = InClient;
	SourceGuid = InSourceGuid;

	StartListening();
}

void FVeLiveLinkSource::InitializeSettings(ULiveLinkSourceSettings* InSettings) {
	ILiveLinkSource::InitializeSettings(InSettings);

	if (UVeLiveLinkSourceSettings* Settings = Cast<UVeLiveLinkSourceSettings>(InSettings)) {
		SavedSourceSettings = Settings;
	} else {
		// VeLogWarning("Invalid settings class passed to VeLiveLinkSource::InitializeSettings");
	}
}

void FVeLiveLinkSource::OnSettingsChanged(ULiveLinkSourceSettings* Settings, const FPropertyChangedEvent& PropertyChangedEvent) {
	ILiveLinkSource::OnSettingsChanged(Settings, PropertyChangedEvent);

	const FProperty* const MemberProperty = PropertyChangedEvent.MemberProperty;
	const FProperty* const Property = PropertyChangedEvent.Property;
	if (Property && MemberProperty && (PropertyChangedEvent.ChangeType != EPropertyChangeType::Interactive)) {
		if (SavedSourceSettings != nullptr) {
			// todo: check if the settings are different from the current settings and if so, update the connection
		} else {
			// VeLogWarning("VeVerse Live Link Source coming from Preset is outdated. Consider recreating a VeVerse Live Link Source. Configure it and resave as preset");
		}
	}
}

bool FVeLiveLinkSource::IsSourceStillValid() const {
	return SourceStatus == EVeLiveLinkSourceStatus::Listening;
}

bool FVeLiveLinkSource::RequestSourceShutdown() {
	return StopListening();
}

void FVeLiveLinkSource::Update() {
	if (bIsListening) {
		for (FVeLiveLinkConnectionPool::TIterator It = Connections.CreateIterator(); It; ++It) {
			if (It.Value()->IsPendingDestroy()) {
				if (It.Value()->RequestDestroy()) {
					It.RemoveCurrent();
				}
			} else {
				It.Value()->Update();
			}
		}
		AcceptConnections();
	}

	// switch (SourceStatus) {
	// case EVeLiveLinkSourceStatus::NotListening:
	// 	break;
	//
	// case EVeLiveLinkSourceStatus::StartListening:
	// 	break;
	//
	// case EVeLiveLinkSourceStatus::Listening:
	// 	break;
	//
	// case EVeLiveLinkSourceStatus::ShuttingDown:
	// 	break;
	//
	// case EVeLiveLinkSourceStatus::Error:
	// 	break;
	// }
}

FText FVeLiveLinkSource::GetSourceStatus() const {
	switch (SourceStatus) {
	case EVeLiveLinkSourceStatus::NotListening:
		return LOCTEXT("NotConnectedStatus", "Not Listening");
	case EVeLiveLinkSourceStatus::StartListening:
		return LOCTEXT("StartListeningStatus", "Start Listening...");
	case EVeLiveLinkSourceStatus::Listening:
		return LOCTEXT("ListeningStatus", "Listening");
	case EVeLiveLinkSourceStatus::ShuttingDown:
		return LOCTEXT("ShuttingDownStatus", "Shutting Down...");
	case EVeLiveLinkSourceStatus::Error:
		return LOCTEXT("ErrorStatus", "Error");
	default:
		return LOCTEXT("UnknownStatus", "Unknown");
	}
}

bool FVeLiveLinkSource::StartListening() {
	check(!bIsListening);

	SourceStatus = EVeLiveLinkSourceStatus::StartListening;

	// Create an IPv4 TCP Socket
	FUniqueSocket NewSocket = SocketSubsystem->CreateUniqueSocket(NAME_Stream, TEXT("VeVerse Live Link Socket"));
	if (!NewSocket) {
		const TCHAR* SocketErr = SocketSubsystem->GetSocketError(SE_GET_LAST_ERROR_CODE);
		// VeLogError("Failed to open socket. Err: %s", SocketErr);

		SourceStatus = EVeLiveLinkSourceStatus::Error;
		return false;
	}

	// Config = FVeLiveLinkServerConfig::GetListenerConfig();

	bool Error = !NewSocket->SetNonBlocking(true);

	TSharedRef<FInternetAddr> BindAddress = SocketSubsystem->CreateInternetAddr();
	if (0 == ConnectionSettings.BindAddress.Compare(TEXT("any"), ESearchCase::IgnoreCase)) {
		BindAddress->SetAnyAddress();
	} else if (0 == ConnectionSettings.BindAddress.Compare(TEXT("localhost"), ESearchCase::IgnoreCase)) {
		BindAddress->SetLoopbackAddress();
	} else {
		bool bIsValidAddress = false;
		BindAddress->SetIp(*(ConnectionSettings.BindAddress), bIsValidAddress);
		if (!bIsValidAddress) {
			// VeLogError("Detected invalid bind address (%s:%u)", *ConnectionSettings.BindAddress, ConnectionSettings.ListenPort);
			Error = true;
		}
	}

	if (!Error) {
		BindAddress->SetPort(ConnectionSettings.ListenPort);
	}

	if (!Error && ConnectionSettings.bReuseAddressAndPort) {
		Error = !NewSocket->SetReuseAddr(true);
	}

	if (!Error) {
		Error = !NewSocket->Bind(*BindAddress);
		if (Error) {
			// VeLogError("Unable to bind to %s", *BindAddress->ToString(true));
		}
	}

	if (!Error) {
		int32 ActualBufferSize;
		NewSocket->SetReceiveBufferSize(ConnectionSettings.BufferSize, ActualBufferSize);
		if (ActualBufferSize < ConnectionSettings.BufferSize) {
			// VeLogWarning("Unable to set desired buffer size (%d): Limited to %d", ConnectionSettings.BufferSize, ActualBufferSize);
		}
	}

	if (!Error) {
		Error = !NewSocket->Listen(ConnectionSettings.ConnectionsBacklogSize);
		if (Error) {
			// VeLogError("Unable to listen on socket");
		}
	}

	if (Error) {
		const TCHAR* SocketErr = SocketSubsystem->GetSocketError(SE_GET_LAST_ERROR_CODE);
		// VeLogError("Failed to create the listen socket as configured. %s", SocketErr);

		SourceStatus = EVeLiveLinkSourceStatus::Error;
		return false;
	}

	ListenSocket = MoveTemp(NewSocket);
	// VeLog("Created new listener on %s", *BindAddress->ToString(true));

	bIsListening = true;
	SourceStatus = EVeLiveLinkSourceStatus::Listening;
	return true;
}

bool FVeLiveLinkSource::StopListening() {
	// check(bIsListening);

	// Tear down our top-level listener first
	if (ListenSocket) {
		// VeLog("Stopping listening on %s", *GetAddressStr(true));

		ListenSocket.Reset();

		bIsListening = false;
		SourceStatus = EVeLiveLinkSourceStatus::ShuttingDown;
	}

	// Destroy connections
	if (Connections.Num()) {
		TArray<FGuid> ConnectionsForRemove;

		for (FVeLiveLinkConnectionPool::TIterator It = Connections.CreateIterator(); It; ++It) {
			if (It.Value()->RequestDestroy()) {
				It.RemoveCurrent();
			}
		}

		if (Connections.Num()) {
			return false;
		}

		// VeLog("All connections closed");
	}

	SourceStatus = EVeLiveLinkSourceStatus::NotListening;
	return true;
}

void FVeLiveLinkSource::AcceptConnections() {
	check(ListenSocket);

	for (int32 i = 0; i < ConnectionSettings.MaxConnectionsAcceptPerFrame; ++i) {
		// Check pending prior to Accept()ing
		bool bHasPendingConnection = false;
		if (!ListenSocket->HasPendingConnection(bHasPendingConnection)) {
			// VeLogError("Listen socket failed to query pending connection");
			return;
		}

		if (bHasPendingConnection) {
			FSocket* IncomingConnection = ListenSocket->Accept(TEXT("HttpRequest"));

			if (nullptr == IncomingConnection) {
				ESocketErrors ErrorCode = ESocketErrors::SE_NO_ERROR;
				FString ErrorStr = TEXT("SocketSubsystem Unavialble");

				if (SocketSubsystem) {
					ErrorCode = SocketSubsystem->GetLastErrorCode();
					ErrorStr = SocketSubsystem->GetSocketError();
				}
				// VeLogError("Error accepting expected connection [%d] %s", static_cast<int32>(ErrorCode), *ErrorStr);
				return;
			}

			IncomingConnection->SetNonBlocking(true);

			auto Connection = MakeShared<UVeLiveLinkServerConnection>(this, IncomingConnection);
			Connections.Emplace(Connection->GetGuid(), Connection);
		}
	}
}

FString FVeLiveLinkSource::GetAddressStr(bool bAppendPort) const {
	TSharedPtr<FInternetAddr> Addr = SocketSubsystem->CreateInternetAddr();
	ListenSocket->GetAddress(*Addr);
	return Addr->ToString(bAppendPort);
}

#undef LOCTEXT_NAMESPACE
