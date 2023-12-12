// Author: Egor A. Pristavka. Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#pragma once

#include "VeShared.h"
#include "FileDownloadTask.h"
#include "FileHeaderRequest.h"


struct FFileDownloadInfo {
	/** Unique id of mod */
	FGuid Id;
	/** Url to download from. */
	FString Url;
	/** Path on the disk to download to. */
	FString Path;
};

typedef FFileDownloadTaskProgress FChunkDownloadProgress;
typedef FFileDownloadTaskResult FFileDownloadResult;

DECLARE_DELEGATE_OneParam(FDownloadChunkSubsystemOnRequestCallback, const FFileDownloadInfo& /* Id */);
DECLARE_DELEGATE_OneParam(FDownloadChunkSubsystemOnProgressCallback, const FChunkDownloadProgress& /* Progress */);
DECLARE_DELEGATE_OneParam(FDownloadChunkSubsystemOnCompleteCallback, const FFileDownloadResult& /* Result */);

class IDownloadSubscription {
public:
	virtual FDownloadChunkSubsystemOnCompleteCallback& OnComplete() = 0;
	virtual FDownloadChunkSubsystemOnProgressCallback& OnProgress() = 0;

	virtual ~IDownloadSubscription() {}
};

typedef TSharedPtr<IDownloadSubscription> FDownloadSubscriptionPtr;

class VEDOWNLOAD_API FDownloadChunkSubsystem final : public IModuleSubsystem {
public:
	static constexpr uint32 HighestPriority = 0xffffffff;

	DECLARE_EVENT_OneParam(FDownloadChunkSubsystem, FDownloadChunkSubsystemOnRequestEvent, const FFileDownloadInfo&)
	FDownloadChunkSubsystemOnRequestEvent& OnDownloadRequest() { return OnRequest; }
	DECLARE_EVENT_OneParam(FDownloadChunkSubsystem, FDownloadChunkSubsystemOnCompleteEvent, const FFileDownloadResult&)
	FDownloadChunkSubsystemOnCompleteEvent& OnDownloadComplete() { return OnComplete; }
	DECLARE_EVENT_OneParam(FDownloadChunkSubsystem, FDownloadChunkSubsystemOnProgressEvent, const FChunkDownloadProgress&)
	FDownloadChunkSubsystemOnProgressEvent& OnDownloadProgress() { return OnProgress; }
private:
	FDownloadChunkSubsystemOnRequestEvent OnRequest;
	FDownloadChunkSubsystemOnCompleteEvent OnComplete;
	FDownloadChunkSubsystemOnProgressEvent OnProgress;

#pragma region VersionInfo
	class VersionInfo {
		TMap<FGuid, FString> Versions;
		FRWLock VersionLock;
		FCriticalSection FileRWLock;
		volatile bool IsUpdated = false;

		FString FileLocation() const {
			return FPaths::ConvertRelativePathToFull(FPaths::ProjectPersistentDownloadDir() / TEXT("pak-versions.json"));
		}
		// reads existing paks versions and updates file
		void Update();
	public:
		/** Tries to set new version value for identified mod, returns true only if version is changed */
		bool TrySetVersion(const FGuid& Id, const FString& Version);

		/** Removes version controll */
		void RemoveVersion(const FGuid& Id);
	};
	VersionInfo Versions;
#pragma endregion

#pragma region Subscriptions
	/** Subscription artifact */
	class FSubscription {
		FDownloadChunkSubsystem* Subsystem;
	public:
		FGuid Id;
		TSharedPtr<FDownloadChunkSubsystemOnCompleteCallback> OnComplete;
		TSharedPtr<FDownloadChunkSubsystemOnProgressCallback> OnProgress;

		FSubscription(const FGuid& Id, FDownloadChunkSubsystem* Subsystem);

		FSubscription(FSubscription&& Sub)
			: Subsystem{Sub.Subsystem}, Id{Sub.Id}, OnComplete{Sub.OnComplete}, OnProgress{Sub.OnProgress} {
			Sub.Subsystem = nullptr;
		}

		FSubscription& operator=(FSubscription&& Sub) noexcept {
			Subsystem = Sub.Subsystem;
			Id = Sub.Id;
			OnComplete = Sub.OnComplete;
			OnProgress = Sub.OnProgress;
			Sub.Subsystem = nullptr;
			Sub.OnComplete.Reset();
			Sub.OnProgress.Reset();

			return *this;
		}

		bool operator ==(const FSubscription& other) const;

		/** Unbinds delegates (Not thread safe) */
		void Destroy();
		bool IsDestroyed() const { return Subsystem == nullptr; }

		~FSubscription();
	};

	// subscription reference, destruction cancells subscription
	class FDownloadSubscription final : public IDownloadSubscription {
		TSharedPtr<FSubscription> Subscription;
	public:
		FDownloadSubscription(TSharedPtr<FSubscription> Subscription)
			: Subscription{Subscription} {}

		FDownloadSubscription(FDownloadSubscription&& other)
			: Subscription{other.Subscription} {
			other.Subscription.Reset();
		}

		FDownloadSubscription& operator=(FDownloadSubscription&& other) {
			Subscription = other.Subscription;
			other.Subscription.Reset();

			return *this;
		}

		virtual FDownloadChunkSubsystemOnCompleteCallback& OnComplete() { return *Subscription->OnComplete; }
		virtual FDownloadChunkSubsystemOnProgressCallback& OnProgress() { return *Subscription->OnProgress; }

		~FDownloadSubscription() {
			if (Subscription.IsValid()) Subscription->Destroy();
		}
	};

	/** Collection of all subscriptions
		order of subscriptions execution is not guaranteed */
	TMap<FGuid, TArray<TSharedPtr<FSubscription>>> Subscriptions;
	FRWLock SubscriptionsLock;

public:
	const TMap<FGuid, TArray<TSharedPtr<FSubscription>>>& GetSubscriptions() const { return Subscriptions; }
private:
	bool TryRemoveSubscription(const FSubscription& sub);

	/** Erases all subscriptions so they no longer emit */
	void CleanSubscriptions();

	/** Creates a subscription to @Id file events */
	FDownloadSubscriptionPtr TryAddSubscription(FGuid Id);
#pragma endregion

#pragma region Tasks
	/** Download request record */
	struct FChunkDownloadRequestWithTask {
		/** File download info */
		FFileDownloadInfo Info;
		/** Task priority, tasks with higher priority are scheduled prior to lower priority tasks. */
		uint32 Priority = 0;
		/** Overwrite or not. */
		bool bOverwrite = false;

		/** Async task for chunked downloading */
		FFileDownloadTask DownloadTask;

		/** Determines if the task is available to iterate */
		bool bIsAvailable;

		FChunkDownloadRequestWithTask(const FFileDownloadInfo& Info, uint32 Priority, bool bOverwrite, FFileDownloadTask&& Task)
			: Info{Info}, Priority{Priority}, bOverwrite{bOverwrite}, DownloadTask{MoveTempIfPossible(Task)}, bIsAvailable{true} {}
	};

	/** Collection of all downloads */
	TMap<FGuid, TSharedRef<FChunkDownloadRequestWithTask>> Downloads;

	/** Lock is used to prevent picking the same task by multiple threads */
	FCriticalSection ActiveDownloadsLock;
	FRWLock DownloadsLock;

	/** Number of currently active download tasks */
	volatile int32 iActiveDownloads = 0;

	/** How many parallel download tasks are allowed. */
	const int32 MaxParallelTasks = 10;
public:
	int32 GetMaxParallelTasks() const { return MaxParallelTasks; }
private:
	/** Tick interval. */
	const float TickInterval = 1.0f;

	void SetAvailable(const FGuid& Id);
#pragma endregion

protected:
	virtual void Initialize() override;
	virtual void Shutdown() override;
public:
	class FDownloadAsyncResult {
	public:
		FDownloadSubscriptionPtr Subscription;

	};

	const static FName Name;

	/** Requests to download file by id and url, returns owned subscription */
	FDownloadSubscriptionPtr DownloadFile(const FFileDownloadInfo& Info, uint32 Priority, bool bOverwrite = false);

	/** Returns result of download attempt */
	TSharedPtr<FFileDownloadResult> Result(const FGuid& Id);
	TSharedPtr<FFileDownloadTaskProgress> Progress(const FGuid& Id);

	/** Remove file tracking */
	bool TryRemove(const FGuid& Id);
	/** Remove file */
	bool TryRemove(const FFileDownloadInfo& Info);

	/** Sets priority of the download if possible */
	bool TrySetPriority(const FGuid& Id, int32 Priority);

	/** Stops download of file by id, */
	bool TryStop(const FGuid& id);

	/** Requests file headers */
	void GetFileHeaders(FString InUrl, TSharedRef<FOnFileHeaderRequestComplete> InCallback);

	virtual ~FDownloadChunkSubsystem();

private:
#pragma region Callbacks
	void DownloadProgressReceived(const FFileDownloadTaskProgress& Progress);

	void DownloadChunkReceived(const FFileDownloadTaskProgress& Progress);

	void DownloadCompleteReceived(const FFileDownloadTaskResult& Result);

	void DownloadLinkReceived(const FFileDownloadLink& Link);

	bool DownloadHeaderReceived(const FFileDownloadTaskHeader& Header);
#pragma endregion

#pragma region Tick
	/** Ticker delegate. */
	FTSTicker::FDelegateHandle TickDelegateHandle;

	/** Updates asynchronous tasks. */
	bool Tick(float _DeltaTime);

	/** Start ticking to update async jobs. */
	void StartTick();

	/** Stop ticking when no active or pending async jobs left. */
	void StopTick();
#pragma endregion
};
