// Authors: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022-2023 VeVerse AS. All rights reserved.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "VeAiBlueprintFunctionLibrary.h"

#include "Api2AiSubsystem.h"
#include "Api2AiTtsRequest.h"
#include "VeShared.h"
#include "VeApi2.h"
#include "Sound/SoundWaveProcedural.h"

void UVeAiBlueprintFunctionLibrary::GenerateTtsUrl(UObject* WorldContextObject, const FString& OutputFormat, const FString& SampleRate, const FString& InText, const ECogAiVoice& SubjectVoice, const FOnGenerateTtsUrlCompleteNative& OnComplete) {
	// Get the AI API subsystem.
	GET_MODULE_SUBSYSTEM(AiSubsystem, Api2, Ai);
	if (!AiSubsystem) {
		return;
	}

	FApi2AiTtsRequestMetadata RequestMetadata;
	RequestMetadata.Text = InText;
	RequestMetadata.OutputFormat = OutputFormat;
	RequestMetadata.SampleRate = SampleRate;

	switch (SubjectVoice) {
	case ECogAiVoice::FemaleC:
		RequestMetadata.VoiceId = Api2AiTtsVoiceIds::Female::C;
		break;
	case ECogAiVoice::FemaleE:
		RequestMetadata.VoiceId = Api2AiTtsVoiceIds::Female::E;
		break;
	case ECogAiVoice::FemaleF:
		RequestMetadata.VoiceId = Api2AiTtsVoiceIds::Female::F;
		break;
	case ECogAiVoice::FemaleG:
		RequestMetadata.VoiceId = Api2AiTtsVoiceIds::Female::G;
		break;
	case ECogAiVoice::FemaleH:
		RequestMetadata.VoiceId = Api2AiTtsVoiceIds::Female::H;
		break;
	case ECogAiVoice::MaleA:
		RequestMetadata.VoiceId = Api2AiTtsVoiceIds::Male::A;
		break;
	case ECogAiVoice::MaleD:
		RequestMetadata.VoiceId = Api2AiTtsVoiceIds::Male::D;
		break;
	case ECogAiVoice::MaleI:
		RequestMetadata.VoiceId = Api2AiTtsVoiceIds::Male::I;
		break;
	case ECogAiVoice::MaleJ:
		RequestMetadata.VoiceId = Api2AiTtsVoiceIds::Male::J;
		break;
	default:
		RequestMetadata.VoiceId = Api2AiTtsVoiceIds::Male::A;
		break;
	}

	const auto Callback = MakeShared<FOnAiTtsRequestCompleted2>();
	Callback->BindWeakLambda(WorldContextObject, [=](const FApi2AiTtsResponseMetadata& InMetadata, const EApi2ResponseCode InResponseCode, const FString& InError) {
		if (InResponseCode == EApi2ResponseCode::Ok) {
			OnComplete.ExecuteIfBound(InMetadata.Url, InError);
		} else {
			OnComplete.ExecuteIfBound({}, InError);
		}
	});
	AiSubsystem->GetTextToSpeech(RequestMetadata, Callback);
}

void UVeAiBlueprintFunctionLibrary::GenerateTtsUrl(UObject* WorldContextObject, const FString& OutputFormat, const FString& SampleRate, const FString& InText, const
	ECogAiVoice& SubjectVoice, const FOnGenerateTtsUrlComplete& OnComplete) {
	// Get the AI API subsystem.
	GET_MODULE_SUBSYSTEM(AiSubsystem, Api2, Ai);
	if (!AiSubsystem) {
		return;
	}

	FApi2AiTtsRequestMetadata RequestMetadata;
	RequestMetadata.Text = InText;
	RequestMetadata.OutputFormat = OutputFormat;
	RequestMetadata.SampleRate = SampleRate;

	switch (SubjectVoice) {
	case ECogAiVoice::FemaleC:
		RequestMetadata.VoiceId = Api2AiTtsVoiceIds::Female::C;
		break;
	case ECogAiVoice::FemaleE:
		RequestMetadata.VoiceId = Api2AiTtsVoiceIds::Female::E;
		break;
	case ECogAiVoice::FemaleF:
		RequestMetadata.VoiceId = Api2AiTtsVoiceIds::Female::F;
		break;
	case ECogAiVoice::FemaleG:
		RequestMetadata.VoiceId = Api2AiTtsVoiceIds::Female::G;
		break;
	case ECogAiVoice::FemaleH:
		RequestMetadata.VoiceId = Api2AiTtsVoiceIds::Female::H;
		break;
	case ECogAiVoice::MaleA:
		RequestMetadata.VoiceId = Api2AiTtsVoiceIds::Male::A;
		break;
	case ECogAiVoice::MaleD:
		RequestMetadata.VoiceId = Api2AiTtsVoiceIds::Male::D;
		break;
	case ECogAiVoice::MaleI:
		RequestMetadata.VoiceId = Api2AiTtsVoiceIds::Male::I;
		break;
	case ECogAiVoice::MaleJ:
		RequestMetadata.VoiceId = Api2AiTtsVoiceIds::Male::J;
		break;
	default:
		RequestMetadata.VoiceId = Api2AiTtsVoiceIds::Male::A;
		break;
	}

	const auto Callback = MakeShared<FOnAiTtsRequestCompleted2>();
	Callback->BindWeakLambda(WorldContextObject, [=](const FApi2AiTtsResponseMetadata& InMetadata, const EApi2ResponseCode InResponseCode, const FString& InError) {
		if (InResponseCode == EApi2ResponseCode::Ok) {
			OnComplete.ExecuteIfBound(InMetadata.Url, InError);
		} else {
			OnComplete.ExecuteIfBound({}, InError);
		}
	});
	AiSubsystem->GetTextToSpeech(RequestMetadata, Callback);
}

void UVeAiBlueprintFunctionLibrary::StreamTts(UObject* WorldContextObject, const FString& OutputFormat, const FString& SampleRate, const FString& InText, const ECogAiVoice& SubjectVoice, const FOnGenerateTtsUrlCompleteNative& OnComplete) {
	// Get the AI API subsystem.
	GET_MODULE_SUBSYSTEM(AiSubsystem, Api2, Ai);
	if (!AiSubsystem) {
		return;
	}

	FApi2AiTtsRequestMetadata RequestMetadata;
	RequestMetadata.Text = InText;
	RequestMetadata.OutputFormat = OutputFormat;
	RequestMetadata.SampleRate = SampleRate;

	switch (SubjectVoice) {
	case ECogAiVoice::FemaleC:
		RequestMetadata.VoiceId = Api2AiTtsVoiceIds::Female::C;
		break;
	case ECogAiVoice::FemaleE:
		RequestMetadata.VoiceId = Api2AiTtsVoiceIds::Female::E;
		break;
	case ECogAiVoice::FemaleF:
		RequestMetadata.VoiceId = Api2AiTtsVoiceIds::Female::F;
		break;
	case ECogAiVoice::FemaleG:
		RequestMetadata.VoiceId = Api2AiTtsVoiceIds::Female::G;
		break;
	case ECogAiVoice::FemaleH:
		RequestMetadata.VoiceId = Api2AiTtsVoiceIds::Female::H;
		break;
	case ECogAiVoice::MaleA:
		RequestMetadata.VoiceId = Api2AiTtsVoiceIds::Male::A;
		break;
	case ECogAiVoice::MaleD:
		RequestMetadata.VoiceId = Api2AiTtsVoiceIds::Male::D;
		break;
	case ECogAiVoice::MaleI:
		RequestMetadata.VoiceId = Api2AiTtsVoiceIds::Male::I;
		break;
	case ECogAiVoice::MaleJ:
		RequestMetadata.VoiceId = Api2AiTtsVoiceIds::Male::J;
		break;
	default:
		RequestMetadata.VoiceId = Api2AiTtsVoiceIds::Male::A;
		break;
	}

	const auto Callback = MakeShared<FOnAiTtsRequestCompleted2>();
	Callback->BindWeakLambda(WorldContextObject, [=](const FApi2AiTtsResponseMetadata& InMetadata, const EApi2ResponseCode InResponseCode, const FString& InError) {
		if (InResponseCode == EApi2ResponseCode::Ok) {
			// Convert local file path to absolute URL
			const FString Url = FPaths::ConvertRelativePathToFull(InMetadata.Url);
			OnComplete.ExecuteIfBound(Url, InError);
		} else {
			OnComplete.ExecuteIfBound({}, InError);
		}
	});
	AiSubsystem->StreamTextToSpeech(RequestMetadata, Callback);
}

void UVeAiBlueprintFunctionLibrary::StreamTts(UObject* WorldContextObject, const FString& OutputFormat, const FString& SampleRate, const FString& InText, const ECogAiVoice& SubjectVoice, const FOnGenerateTtsUrlComplete& OnComplete) {
	// Get the AI API subsystem.
	GET_MODULE_SUBSYSTEM(AiSubsystem, Api2, Ai);
	if (!AiSubsystem) {
		return;
	}

	FApi2AiTtsRequestMetadata RequestMetadata;
	RequestMetadata.Text = InText;
	RequestMetadata.OutputFormat = OutputFormat;
	RequestMetadata.SampleRate = SampleRate;

	switch (SubjectVoice) {
	case ECogAiVoice::FemaleC:
		RequestMetadata.VoiceId = Api2AiTtsVoiceIds::Female::C;
		break;
	case ECogAiVoice::FemaleE:
		RequestMetadata.VoiceId = Api2AiTtsVoiceIds::Female::E;
		break;
	case ECogAiVoice::FemaleF:
		RequestMetadata.VoiceId = Api2AiTtsVoiceIds::Female::F;
		break;
	case ECogAiVoice::FemaleG:
		RequestMetadata.VoiceId = Api2AiTtsVoiceIds::Female::G;
		break;
	case ECogAiVoice::FemaleH:
		RequestMetadata.VoiceId = Api2AiTtsVoiceIds::Female::H;
		break;
	case ECogAiVoice::MaleA:
		RequestMetadata.VoiceId = Api2AiTtsVoiceIds::Male::A;
		break;
	case ECogAiVoice::MaleD:
		RequestMetadata.VoiceId = Api2AiTtsVoiceIds::Male::D;
		break;
	case ECogAiVoice::MaleI:
		RequestMetadata.VoiceId = Api2AiTtsVoiceIds::Male::I;
		break;
	case ECogAiVoice::MaleJ:
		RequestMetadata.VoiceId = Api2AiTtsVoiceIds::Male::J;
		break;
	default:
		RequestMetadata.VoiceId = Api2AiTtsVoiceIds::Male::A;
		break;
	}

	const auto Callback = MakeShared<FOnAiTtsRequestCompleted2>();
	Callback->BindWeakLambda(WorldContextObject, [=](const FApi2AiTtsResponseMetadata& InMetadata, const EApi2ResponseCode InResponseCode, const FString& InError) {
		if (InResponseCode == EApi2ResponseCode::Ok) {
			OnComplete.ExecuteIfBound(InMetadata.Url, InError);
		} else {
			OnComplete.ExecuteIfBound({}, InError);
		}
	});
	AiSubsystem->StreamTextToSpeech(RequestMetadata, Callback);
}

FString UVeAiBlueprintFunctionLibrary::SanitizeTtsText(const FString& InText) {
	FString OutString;
	bool bInTag = false;
	for (const TCHAR Char : InText) {
		if (Char == TEXT('<')) {
			bInTag = true;
		} else if (Char == TEXT('>')) {
			bInTag = false;
		} else if (!bInTag) {
			OutString.AppendChar(Char);
		}
	}
	return OutString;
}
