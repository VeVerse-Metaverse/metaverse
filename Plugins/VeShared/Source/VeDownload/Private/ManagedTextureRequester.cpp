// Author: Egor A. Pristavka. Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#include "ManagedTextureRequester.h"

#include "DownloadTextureSubsystem.h"
#include "VeDownload.h"


IManagedTextureRequester::IManagedTextureRequester() {}

IManagedTextureRequester::~IManagedTextureRequester() {
	if (IsRunningDedicatedServer()) {
		return;
	}

	ClearRequest();
	ReleaseAllTextures();
}

void IManagedTextureRequester::ClearRequest() {
	if (IsRunningDedicatedServer()) {
		return;
	}

	if (Request) {
		if (Request->GetOnComplete().IsBound()) {
			Request->GetOnComplete().Unbind();
		}
		Request.Reset();
	}
}

void IManagedTextureRequester::RequestTexture(UObject* Context, const FString& InUrl, TEnumAsByte<enum TextureGroup> InTextureGroup) {
	if (IsRunningDedicatedServer()) {
		return;
	}

	if (CurrentTextureUrl != InUrl) {
		ClearRequest();
	}

	CurrentTextureUrl = InUrl;

	uint32 MipCount = 8;
	if (InTextureGroup == TEXTUREGROUP_UI) {
		MipCount = 0;
	}

	Request = MakeShared<FTextureLoadRequest>(InUrl, InTextureGroup, MipCount);
	if (IsValid(Context)) {
		Request->GetOnComplete().BindWeakLambda(Context, [this](const FTextureLoadResult& Result) {
			OnTextureLoadCompleted(Result);
		});
	} else {
		Request->GetOnComplete().BindRaw(this, &IManagedTextureRequester::OnTextureLoadCompleted);
	}

	GET_MODULE_SUBSYSTEM(TextureSubsystem, Download, Texture);
	if (TextureSubsystem) {
		TextureSubsystem->LoadTexture(Request);
	}
}

void IManagedTextureRequester::RequestTexture(UObject* Context, const TSharedPtr<FTextureLoadRequest> InRequest) {
	if (IsRunningDedicatedServer()) {
		return;
	}

	if (CurrentTextureUrl != InRequest->Url) {
		ClearRequest();
	}

	CurrentTextureUrl = InRequest->Url;

	Request = InRequest;
	if (IsValid(Context)) {
		Request->GetOnComplete().BindWeakLambda(Context, [this](const FTextureLoadResult& Result) {
			OnTextureLoadCompleted(Result);
		});
	} else {
		Request->GetOnComplete().BindRaw(this, &IManagedTextureRequester::OnTextureLoadCompleted);
	}

	GET_MODULE_SUBSYSTEM(TextureSubsystem, Download, Texture);
	if (TextureSubsystem) {
		TextureSubsystem->LoadTexture(Request);
	}
}

void IManagedTextureRequester::ReleaseTexture(const FString& InUrl) {
	if (IsRunningDedicatedServer()) {
		return;
	}

	GET_MODULE_SUBSYSTEM(TextureSubsystem, Download, Texture);
	if (TextureSubsystem) {
		TextureSubsystem->RemoveObjectReference(InUrl, this);
	}
}

void IManagedTextureRequester::ReleaseAllTextures() {
	if (IsRunningDedicatedServer()) {
		return;
	}

	if (!bUsingAnyTextures) {
		return;
	}

	GET_MODULE_SUBSYSTEM(TextureSubsystem, Download, Texture);
	if (TextureSubsystem) {
		TextureSubsystem->RemoveAllObjectReferences(this);
	}
}

void IManagedTextureRequester::OnTextureLoadCompleted(const FTextureLoadResult& Result) {
	if (IsRunningDedicatedServer()) {
		return;
	}

	GET_MODULE_SUBSYSTEM(TextureSubsystem, Download, Texture);
	if (TextureSubsystem) {
		TextureSubsystem->AddObjectReference(Result.Url, this);
		if (!PreviousTextureUrl.IsEmpty() && Result.Url != PreviousTextureUrl) {
			TextureSubsystem->RemoveObjectReference(PreviousTextureUrl, this);
		}
		PreviousTextureUrl = Result.Url;
	}

	if (!Result.bSuccessful || !Result.Texture.IsValid()) {
		LogErrorF("failed to load texture %s: %s", *Result.Url, *Result.Error);
	}

	NativeOnTextureLoaded(Result);
}

void FManagedTextureRequester::NativeOnTextureLoaded(const FTextureLoadResult& InResult) {
	if (IsRunningDedicatedServer()) {
		return;
	}

	Result = InResult;
	bBusy = false;

	if (OnTextureLoaded.IsBound()) {
		OnTextureLoaded.Broadcast(InResult);
	}
}

void FManagedTextureRequester::NativeOnTextureRequested(TSharedPtr<FTextureLoadRequest> InRequest) {
	bBusy = true;
}
