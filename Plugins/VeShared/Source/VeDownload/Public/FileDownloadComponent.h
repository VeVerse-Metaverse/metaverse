// 

#pragma once

#include "Components/ActorComponent.h"
#include "Download1Metadata.h"
#include "FileDownloadComponent.generated.h"

DECLARE_DELEGATE_OneParam(FOnDownloadProgress, const FDownload1ProgressMetadata& Metadata);
DECLARE_DELEGATE_OneParam(FOnDownloadComplete, const FDownload1ResultMetadata& Metadata);

DECLARE_DYNAMIC_DELEGATE_OneParam(FOnDownloadProgressBP, const FDownload1ProgressMetadata&, Metadata);

DECLARE_DYNAMIC_DELEGATE_OneParam(FOnDownloadCompleteBP, const FDownload1ResultMetadata&, Metadata);

class FVeDownload1;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class VEDOWNLOAD_API UFileDownloadComponent : public UActorComponent {
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UFileDownloadComponent();

	TSharedPtr<FVeDownload1> Download;

	void StartDownload(const FString& Url, const FOnDownloadProgress& OnDownloadProgress, const FOnDownloadComplete& OnDownloadComplete);

	UFUNCTION(BlueprintCallable)
	void StartDownload(const FString& Url, const FOnDownloadProgressBP& OnDownloadProgressBP, const FOnDownloadCompleteBP& OnDownloadCompleteBP);
};
