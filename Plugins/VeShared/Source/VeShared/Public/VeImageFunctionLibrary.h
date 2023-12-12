// Author: Egor A. Pristavka. Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "VeImageFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class VESHARED_API UVeImageFunctionLibrary final : public UBlueprintFunctionLibrary {
	GENERATED_BODY()
public:
	static bool ExportRenderTarget2DAsJPEGRaw(UTextureRenderTarget2D* TexRT, TArray<uint8>& OutBytes);
	
	static bool ExportCubeRenderTarget2DAsJPEGRaw(UTextureRenderTargetCube* TexRT, TArray<uint8>& OutBytes);

	static bool ExportRenderTarget2DAsJPEG(UTextureRenderTarget2D* TexRT, FArchive& Ar);

	UFUNCTION(BlueprintCallable, Category = "Rendering", meta = (Keywords = "ExportRenderTargetAsJPEG", WorldContext = "WorldContextObject"))
	static void ExportRenderTargetAsJPEG(UObject* WorldContextObject, UTextureRenderTarget2D* TextureRenderTarget, const FString& FilePath, const FString& FileName);
};
