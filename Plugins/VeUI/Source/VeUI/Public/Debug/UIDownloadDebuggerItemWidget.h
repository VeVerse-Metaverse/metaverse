// Copyright (c) 2022 LE7EL AS. All right reserved.

#pragma once

#include "DownloadChunkSubsystem.h"
#include "Components/SizeBox.h"
#include "Components/VerticalBox.h"
#include "UIDownloadDebuggerItemWidget.generated.h"

class UTextBlock;
class UBorder;

/**  */
UCLASS(HideDropdown)
class VEUI_API UUIDownloadDebuggerItemWidget final : public UUserWidget {
	GENERATED_BODY()

public:
#pragma region Widgets

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	TObjectPtr<USizeBox> ProgressWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	TObjectPtr<UTextBlock> ProgressTextWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	TObjectPtr<UTextBlock> IdTextWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	TObjectPtr<UTextBlock> UrlTextWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	TObjectPtr<UTextBlock> ResultTextWidget;

	void SetInfo(const FGuid& Id, const FString& Url) const;
	void SetProgress(float Progress, int64 TotalSize) const;
	void SetResult(const FFileDownloadResult& Result) const;

#pragma endregion

	FGuid DownloadId;
	FDownloadSubscriptionPtr BoundDownload;

};
