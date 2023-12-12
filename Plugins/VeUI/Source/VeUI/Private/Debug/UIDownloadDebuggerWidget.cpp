// Copyright (c) 2022 LE7EL AS. All right reserved.

#include "Debug/UIDownloadDebuggerWidget.h"

#include "VeDownload.h"
#include "VeShared.h"
#include "Components/UIButtonWidget.h"
#include "Debug/UIDownloadDebuggerItemWidget.h"

void UUIDownloadDebuggerWidget::NativeOnInitialized() {
	Super::NativeOnInitialized();

	GET_MODULE_SUBSYSTEM(DownloadSubsystem, Download, Chunk);
	if (DownloadSubsystem) {
		auto Num = DownloadSubsystem->GetMaxParallelTasks();
		for (auto i = 0; i < Num; i++) {
			auto* Widget = CreateWidget<UUIDownloadDebuggerItemWidget>(this, ItemWidgetClass);
			ContainerWidget->AddChildToVerticalBox(reinterpret_cast<UWidget*>(Widget));
		}
	}
}

void UUIDownloadDebuggerWidget::NativeTick(const FGeometry& MyGeometry, const float InDeltaTime) {
	Super::NativeTick(MyGeometry, InDeltaTime);

	GET_MODULE_SUBSYSTEM(DownloadSubsystem, Download, Chunk);
	if (DownloadSubsystem) {
		auto Subs = DownloadSubsystem->GetSubscriptions();
		int32 i = 0;
		for (auto Sub : Subs) {
			i++;
			FGuid Key = Sub.Key;
			auto Progress = DownloadSubsystem->Progress(Key);
			if (Progress) {
				// ContainerWidget->GetChildIndex()
			}
			auto Result = DownloadSubsystem->Result(Key);
			if (Result) { }
		}
	}
}
