// Author: Egor A. Pristavka. Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.


#include "VeImageFunctionLibrary.h"

#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Engine/TextureRenderTargetCube.h"
#include "Logging/MessageLog.h"
#include "Serialization/BufferArchive.h"
#include "VeShared.h"

#define LOCTEXT_NAMESPACE "ImageUtils"

static bool GetRawData(UTextureRenderTarget2D* TexRT, TArray64<uint8>& RawData) {
	FRenderTarget* RenderTarget = TexRT->GameThread_GetRenderTargetResource();
	const EPixelFormat Format = TexRT->GetFormat();

	const int32 ImageBytes = CalculateImageBytes(TexRT->SizeX, TexRT->SizeY, 0, Format);
	RawData.AddUninitialized(ImageBytes);
	bool bReadSuccess = false;
	switch (Format) {
	case PF_FloatRGBA:
		{
			TArray<FFloat16Color> FloatColors;
			bReadSuccess = RenderTarget->ReadFloat16Pixels(FloatColors);
			FMemory::Memcpy(RawData.GetData(), FloatColors.GetData(), ImageBytes);
		}
		break;
	case PF_B8G8R8A8:
		bReadSuccess = RenderTarget->ReadPixelsPtr((FColor*)RawData.GetData());
		break;
	default:
		break;
	}
	if (bReadSuccess == false) {
		RawData.Empty();
	}
	return bReadSuccess;
}

static bool GetCubeRawData(UTextureRenderTargetCube* TexRT, TArray64<uint8>& RawData) {
	FRenderTarget* RenderTarget = TexRT->GameThread_GetRenderTargetResource();
	const EPixelFormat Format = TexRT->GetFormat();

	const int32 ImageBytes = CalculateImageBytes(TexRT->SizeX, TexRT->SizeX, 6, Format);
	RawData.AddUninitialized(ImageBytes);
	bool bReadSuccess = false;
	switch (Format) {
	case PF_FloatRGBA:
		{
			TArray<FFloat16Color> FloatColors;
			bReadSuccess = RenderTarget->ReadFloat16Pixels(FloatColors);
			FMemory::Memcpy(RawData.GetData(), FloatColors.GetData(), ImageBytes);
		}
		break;
	case PF_B8G8R8A8:
		bReadSuccess = RenderTarget->ReadPixelsPtr((FColor*)RawData.GetData());
		break;
	default:
		break;
	}
	if (bReadSuccess == false) {
		RawData.Empty();
	}
	return bReadSuccess;
}

bool UVeImageFunctionLibrary::ExportRenderTarget2DAsJPEGRaw(UTextureRenderTarget2D* TexRT, TArray<uint8>& OutBytes) {
	bool bSuccess = false;
	if (TexRT->GetFormat() == PF_B8G8R8A8) {
		check(TexRT != nullptr);
		const FRenderTarget* RenderTarget = TexRT->GameThread_GetRenderTargetResource();
		const FIntPoint Size = RenderTarget->GetSizeXY();

		TArray64<uint8> RawData;
		bSuccess = GetRawData(TexRT, RawData);

		IImageWrapperModule& ImageWrapperModule = FModuleManager::Get().LoadModuleChecked<IImageWrapperModule>(TEXT("ImageWrapper"));

		const TSharedPtr<IImageWrapper> JpegImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::JPEG);

		JpegImageWrapper->SetRaw(RawData.GetData(), RawData.GetAllocatedSize(), Size.X, Size.Y, ERGBFormat::BGRA, 8);

		const TArray64<uint8>& JpegData = JpegImageWrapper->GetCompressed(100);

		const auto JpegDataSize = JpegData.GetAllocatedSize();
		constexpr auto MaxSize = TNumericLimits<int32>::Max();
		if (JpegDataSize >= MaxSize || JpegDataSize < 0) {
			LogErrorF("jpeg size is incorrect: %lld, max is: %d", JpegData.GetAllocatedSize(), MaxSize);
			return false;
		}

		OutBytes.SetNumUninitialized(JpegDataSize);

		FMemory::Memcpy(OutBytes.GetData(), JpegData.GetData(), JpegDataSize);
	}

	return bSuccess;
}


bool UVeImageFunctionLibrary::ExportRenderTarget2DAsJPEG(UTextureRenderTarget2D* TexRT, FArchive& Ar) {
	bool bSuccess = false;
	if (TexRT->GetFormat() == PF_B8G8R8A8) {
		check(TexRT != nullptr);
		const FRenderTarget* RenderTarget = TexRT->GameThread_GetRenderTargetResource();
		const FIntPoint Size = RenderTarget->GetSizeXY();

		TArray64<uint8> RawData;
		bSuccess = GetRawData(TexRT, RawData);

		IImageWrapperModule& ImageWrapperModule = FModuleManager::Get().LoadModuleChecked<IImageWrapperModule>(TEXT("ImageWrapper"));

		const TSharedPtr<IImageWrapper> JpegImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::JPEG);

		JpegImageWrapper->SetRaw(RawData.GetData(), RawData.GetAllocatedSize(), Size.X, Size.Y, ERGBFormat::BGRA, 8);

		const TArray64<uint8>& JpegData = JpegImageWrapper->GetCompressed(100);

		Ar.Serialize((void*)JpegData.GetData(), JpegData.GetAllocatedSize());
	}

	return bSuccess;
}

bool UVeImageFunctionLibrary::ExportCubeRenderTarget2DAsJPEGRaw(UTextureRenderTargetCube* TexRT, TArray<uint8>& OutBytes) {
	bool bSuccess = false;
	if (TexRT->GetFormat() == PF_B8G8R8A8) {
		check(TexRT != nullptr);
		const FRenderTarget* RenderTarget = TexRT->GameThread_GetRenderTargetResource();
		const FIntPoint Size = RenderTarget->GetSizeXY();

		TArray64<uint8> RawData;
		bSuccess = GetCubeRawData(TexRT, RawData);

		IImageWrapperModule& ImageWrapperModule = FModuleManager::Get().LoadModuleChecked<IImageWrapperModule>(TEXT("ImageWrapper"));

		const TSharedPtr<IImageWrapper> JpegImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::EXR);

		JpegImageWrapper->SetRaw(RawData.GetData(), RawData.GetAllocatedSize(), Size.X, Size.Y, ERGBFormat::BGRA, 8);

		const TArray64<uint8>& JpegData = JpegImageWrapper->GetCompressed(100);

		const auto JpegDataSize = JpegData.GetAllocatedSize();
		constexpr auto MaxSize = TNumericLimits<int32>::Max();
		if (JpegDataSize >= MaxSize || JpegDataSize < 0) {
			LogErrorF("jpeg size is incorrect: %lld, max is: %d", JpegData.GetAllocatedSize(), MaxSize);
			return false;
		}

		OutBytes.SetNumUninitialized(JpegDataSize);

		FMemory::Memcpy(OutBytes.GetData(), JpegData.GetData(), JpegDataSize);
	}

	return bSuccess;
}

void UVeImageFunctionLibrary::ExportRenderTargetAsJPEG(UObject* WorldContextObject, UTextureRenderTarget2D* TextureRenderTarget, const FString& FilePath, const FString& FileName) {
	const FString TotalFileName = FPaths::Combine(*FilePath, *FileName);
	FText PathError;
	FPaths::ValidatePath(TotalFileName, &PathError);

	if (!TextureRenderTarget) {
		FMessageLog("Blueprint").Warning(FText::Format(
			LOCTEXT("ExportRenderTarget_InvalidTextureRenderTarget", "ExportRenderTarget[{0}]: TextureRenderTarget must be non-null."),
			FText::FromString(GetPathNameSafe(WorldContextObject))));
	} else if (!TextureRenderTarget->GetResource()) {
		FMessageLog("Blueprint").Warning(FText::Format(
			LOCTEXT("ExportRenderTarget_ReleasedTextureRenderTarget", "ExportRenderTarget[{0}]: render target has been released."),
			FText::FromString(GetPathNameSafe(WorldContextObject))));
	} else if (!PathError.IsEmpty()) {
		FMessageLog("Blueprint").Warning(FText::Format(
			LOCTEXT("ExportRenderTarget_InvalidFilePath", "ExportRenderTarget[{0}]: Invalid file path provided: '{1}'"), FText::FromString(GetPathNameSafe(WorldContextObject)),
			PathError));
	} else if (FileName.IsEmpty()) {
		FMessageLog("Blueprint").Warning(FText::Format(
			LOCTEXT("ExportRenderTarget_InvalidFileName", "ExportRenderTarget[{0}]: FileName must be non-empty."), FText::FromString(GetPathNameSafe(WorldContextObject))));
	} else {
		FArchive* Ar = IFileManager::Get().CreateFileWriter(*TotalFileName);

		if (Ar) {
			FBufferArchive Buffer;

			bool bSuccess = false;
			if (TextureRenderTarget->RenderTargetFormat == RTF_RGBA16f) {
				FMessageLog("Blueprint").Warning(LOCTEXT("ExportRenderTarget_FileWriterFailedToCreate", "ExportRenderTarget: Unsupported render target format for JPEG."));
				bSuccess = false;
			} else {
				bSuccess = ExportRenderTarget2DAsJPEG(TextureRenderTarget, Buffer);
			}

			if (bSuccess) {
				Ar->Serialize(Buffer.GetData(), Buffer.Num());
			}

			delete Ar;
		} else {
			FMessageLog("Blueprint").Warning(LOCTEXT("ExportRenderTarget_FileWriterFailedToCreate", "ExportRenderTarget: FileWrite failed to create."));
		}
	}
}

#undef LOCTEXT_NAMESPACE
