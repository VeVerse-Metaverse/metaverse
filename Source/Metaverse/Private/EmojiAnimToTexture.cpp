// Author: Egor A. Pristavka. 
// Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#include "EmojiAnimToTexture.h"
#include "Engine/TextureRenderTarget2D.h"
#include "UObject/ObjectMacros.h"

// Sets default values for this component's properties
UEmojiAnimToTexture::UEmojiAnimToTexture()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UEmojiAnimToTexture::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UEmojiAnimToTexture::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}


void UEmojiAnimToTexture::CreateTexture(FString TextureName, FString Path, UTextureRenderTarget2D *Target, int32 TextureSize)
{
    Path += TextureName;
    UPackage* Package = CreatePackage(NULL, *Path);
    Package->FullyLoad();

 //       if(Target == nullptr)
 //   {
 //       UE_LOG(LogTemp, Error, TEXT("No RenderTarget set."));
 //       return nullptr;
 //   }

    UTexture2D* NewTexture = NewObject<UTexture2D>(Package, *TextureName, RF_Public | RF_Standalone | RF_MarkAsRootSet);
 //   UTexture2D *NewTexture = UTexture2D::CreateTransient(Target->SizeX, Target->SizeY, PF_B8G8R8A8);

#if WITH_EDITORONLY_DATA
NewTexture->MipGenSettings = TMGS_NoMipmaps;
#endif
NewTexture->SRGB = Target->SRGB;

    NewTexture->AddToRoot();				// This line prevents garbage collection of the texture
    NewTexture->SetPlatformData(new FTexturePlatformData());	// Then we initialize the PlatformData

     // Read the pixels from the RenderTarget and store them in a FColor array
    TArray<FColor> SurfData;
    FRenderTarget *RenderTargetCapture = Target->GameThread_GetRenderTargetResource();
    RenderTargetCapture->ReadPixels(SurfData);
   // NewTexture->AddToRoot();				// This line prevents garbage collection of the texture
  //  NewTexture->PlatformData = new FTexturePlatformData();	// Then we initialize the PlatformData
    NewTexture->GetPlatformData()->SizeX = TextureSize;
    NewTexture->GetPlatformData()->SizeY = TextureSize;
 //   NewTexture->PlatformData->NumSlices = 1;
    NewTexture->GetPlatformData()->PixelFormat = EPixelFormat::PF_B8G8R8A8;

    // Allocate first mipmap.
    FTexture2DMipMap* Mip = new(NewTexture->GetPlatformData()->Mips) FTexture2DMipMap();
    Mip->SizeX = TextureSize;
    Mip->SizeY = TextureSize;

// Lock the texture so it can be modified
    Mip->BulkData.Lock(LOCK_READ_WRITE);
    uint8* TextureData = (uint8*) Mip->BulkData.Realloc(TextureSize * TextureSize * 4);
    FMemory::Memcpy(TextureData, SurfData.GetData(), sizeof(uint8) * TextureSize * TextureSize * 4);
    Mip->BulkData.Unlock();

#if WITH_EDITORONLY_DATA
    NewTexture->Source.Init(TextureSize, TextureSize, 1, 1, ETextureSourceFormat::TSF_BGRA8, (uint8*)SurfData.GetData());
#endif

     // Lock and copies the data between the textures
  //   void* TextureData = NewTexture->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
 //    const int32 TextureDataSize = SurfData.Num() * 4;
  //   FMemory::Memcpy(TextureData, SurfData.GetData(), TextureDataSize);
  //   NewTexture->PlatformData->Mips[0].BulkData.Unlock();
     // Apply Texture changes to GPU memory
     NewTexture->UpdateResource();
     Package->MarkPackageDirty();
     FAssetRegistryModule::AssetCreated(NewTexture);

     FString PackageFileNameString = FPackageName::LongPackageNameToFilename(Path, FPackageName::GetAssetPackageExtension());
//     bool ePkg = ensure(Package);
     bool bSaved = UPackage::SavePackage(Package, NewTexture, EObjectFlags::RF_Public | EObjectFlags::RF_Standalone, *PackageFileNameString, GError, nullptr, true, true, SAVE_NoError);
}
