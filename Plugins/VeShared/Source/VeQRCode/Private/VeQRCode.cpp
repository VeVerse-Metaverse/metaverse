// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "VeQRCode.h"
#include "qrcodegen.hpp"

#define LOCTEXT_NAMESPACE "FVeQRCodeModule"

void FVeQRCodeModule::StartupModule() {}

void FVeQRCodeModule::ShutdownModule() {}

constexpr uint32 BytesPerPixel = 8;
constexpr uint8 White = 0xFF;

#pragma pack(push, 2)

UTexture2D* FVeQRCodeModule::GenerateQrCode(const FString Source, const FColor& Foreground, const FColor& Background) {
	const qrcodegen::QrCode QrCode = qrcodegen::QrCode::encodeText(TCHAR_TO_UTF8(*Source), qrcodegen::QrCode::Ecc::MEDIUM);

	const uint32 BitmapSize = QrCode.getSize();
	const uint32 BitmapByteSize = BitmapSize * BytesPerPixel * 4; // Number of pixels per dimension (8), 3 bytes per pixel
	const uint32 RawDataSize = BitmapByteSize * BitmapSize * 8;

	// Allocate raw data buffer
	TArray<uint8> RawData = TArray<uint8>();
	RawData.SetNum(RawDataSize);

	// Reset to the white background.
	FMemory::Memset(RawData.GetData(), White, RawDataSize);

	// Convert QrCode bits to bmp pixels
	for (uint32 Y = 0; Y < BitmapSize; Y++) {
		uint8* DestData = RawData.GetData() + BitmapByteSize * Y * BytesPerPixel;
		for (uint32 X = 0; X < BitmapSize; X++) {
			if (QrCode.getModule(X, Y)) {
				for (uint32 XByte = 0; XByte < BytesPerPixel; XByte++) {
					for (uint32 YByte = 0; YByte < BytesPerPixel; YByte++) {
						*(DestData + YByte * 4 + BitmapByteSize * XByte) = Foreground.B;
						*(DestData + 1 + YByte * 4 + BitmapByteSize * XByte) = Foreground.G;
						*(DestData + 2 + YByte * 4 + BitmapByteSize * XByte) = Foreground.R;
						*(DestData + 3 + YByte * 4 + BitmapByteSize * XByte) = Foreground.A;
					}
				}
			} else {
				for (uint32 XByte = 0; XByte < BytesPerPixel; XByte++) {
					for (uint32 YByte = 0; YByte < BytesPerPixel; YByte++) {
						*(DestData + YByte * 4 + BitmapByteSize * XByte) = Background.B;
						*(DestData + 1 + YByte * 4 + BitmapByteSize * XByte) = Background.G;
						*(DestData + 2 + YByte * 4 + BitmapByteSize * XByte) = Background.R;
						*(DestData + 3 + YByte * 4 + BitmapByteSize * XByte) = Background.A;
					}
				}
			}
			DestData += 4 * BytesPerPixel;
		}
	}

	UTexture2D* QrCodeTexture = UTexture2D::CreateTransient(BitmapSize * BytesPerPixel, BitmapSize * BytesPerPixel);
	QrCodeTexture->SRGB = true;
	FTexture2DMipMap& Mip = QrCodeTexture->GetPlatformData()->Mips[0];
	void* Data = Mip.BulkData.Lock(LOCK_READ_WRITE);

	FMemory::Memcpy(Data, RawData.GetData(), RawDataSize);
	Mip.BulkData.Unlock();
	QrCodeTexture->UpdateResource();

	return QrCodeTexture;
}

#pragma pack(pop)

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FVeQRCodeModule, VeQRCode)
