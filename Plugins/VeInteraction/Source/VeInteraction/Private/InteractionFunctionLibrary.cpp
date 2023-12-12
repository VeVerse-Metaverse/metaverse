// Author: Egor A. Pristavka. 
// Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#include "InteractionFunctionLibrary.h"

#include "Containers/UnrealString.h"
#include "Serialization/BufferWriter.h"


TArray<uint8> UInteractionFunctionLibrary::BooleanToBytes(bool Boolean) {
	return ValueToBytes<bool>(Boolean, EByteTypes::Boolean);
}

void UInteractionFunctionLibrary::BytesToBoolean(const TArray<uint8>& Bytes, bool& Result, bool& IsValid) {
	IsValid = BytesToValue<bool>(Bytes, Result, EByteTypes::Boolean);
}

TArray<uint8> UInteractionFunctionLibrary::IntegerToBytes(int32 Integer) {
	return ValueToBytes<int32>(Integer, EByteTypes::Integer);
}

void UInteractionFunctionLibrary::BytesToInteger(const TArray<uint8>& Bytes, int32& Result, bool& IsValid) {
	IsValid = BytesToValue<int32>(Bytes, Result, EByteTypes::Integer);
}

TArray<uint8> UInteractionFunctionLibrary::FloatToBytes(float Float) {
	return ValueToBytes<float>(Float, EByteTypes::Float);
}

void UInteractionFunctionLibrary::BytesToFloat(const TArray<uint8>& Bytes, float& Result, bool& IsValid) {
	IsValid = BytesToValue<float>(Bytes, Result, EByteTypes::Float);
}

TArray<uint8> UInteractionFunctionLibrary::VectorToBytes(FVector Vector) {
	return ValueToBytes<FVector>(Vector, EByteTypes::Vector);
}

void UInteractionFunctionLibrary::BytesToVector(const TArray<uint8>& Bytes, FVector& Result, bool& IsValid) {
	IsValid = BytesToValue<FVector>(Bytes, Result, EByteTypes::Vector);
}

TArray<uint8> UInteractionFunctionLibrary::RotatorToBytes(FRotator Rotator) {
	return ValueToBytes<FRotator>(Rotator, EByteTypes::Rotator);
}

void UInteractionFunctionLibrary::BytesToRotator(const TArray<uint8>& Bytes, FRotator& Result, bool& IsValid) {
	IsValid = BytesToValue<FRotator>(Bytes, Result, EByteTypes::Rotator);
}

TArray<uint8> UInteractionFunctionLibrary::TransformToBytes(FTransform Transform) {
	return ValueToBytes<FTransform>(Transform, EByteTypes::Transform);
}

void UInteractionFunctionLibrary::BytesToTransform(const TArray<uint8>& Bytes, FTransform& Result, bool& IsValid) {
	IsValid = BytesToValue<FTransform>(Bytes, Result, EByteTypes::Transform);
}

TArray<uint8> UInteractionFunctionLibrary::LinearColorToBytes(FLinearColor LinearColor) {
	return ValueToBytes<FLinearColor>(LinearColor, EByteTypes::LinearColor);
}

void UInteractionFunctionLibrary::BytesToLinearColor(const TArray<uint8>& Bytes, FLinearColor& Result, bool& IsValid) {
	IsValid = BytesToValue<FLinearColor>(Bytes, Result, EByteTypes::LinearColor);
}

TArray<uint8> UInteractionFunctionLibrary::GuidToBytes(FGuid Guid) {
	return ValueToBytes<FGuid>(Guid, EByteTypes::Guid);
}

void UInteractionFunctionLibrary::BytesToGuid(const TArray<uint8>& Bytes, FGuid& Result, bool& IsValid) {
	IsValid = BytesToValue<FGuid>(Bytes, Result, EByteTypes::Guid);
}

TArray<uint8> UInteractionFunctionLibrary::StringToBytes(FString String) {
	return ValueToBytes<FString>(String, EByteTypes::String);
}

void UInteractionFunctionLibrary::BytesToString(const TArray<uint8>& Bytes, FString& Result, bool& IsValid) {
	IsValid = BytesToValue<FString>(Bytes, Result, EByteTypes::String);
}

TArray<uint8> UInteractionFunctionLibrary::ObjectToBytes(UObject* Object) {
	TArray<uint8> Bytes;
	FMemoryWriter Writer(Bytes);
	Object->Serialize(Writer);
	return Bytes;
}

UObject* UInteractionFunctionLibrary::BytesToObject(const FString& ClassName, const TArray<uint8>& Bytes) {
	if (ClassName.IsEmpty() || ClassName == "None") {
		return nullptr;
	}

	UClass* Class = FindObject<UClass>(nullptr, *ClassName);
	if (!Class) {
		Class = LoadObject<UClass>(nullptr, *ClassName);
	}

	UObject* New = NewObject<UObject>(GetTransientPackage(), Class);
	void* Data = const_cast<unsigned char*>(Bytes.GetData());
	FMemoryReader Reader(Bytes);
	New->Serialize(Reader);

	return New;
}

TArray<uint8> UInteractionFunctionLibrary::ArrayBooleanToBytes(TArray<bool>& Booleans) {
	return ValueToBytes<TArray<bool>>(Booleans, EByteTypes::ArrayBoolean);
}

void UInteractionFunctionLibrary::ArrayBytesToBoolean(const TArray<uint8>& Bytes, TArray<bool>& Result, bool& IsValid) {
	IsValid = BytesToValue<TArray<bool>>(Bytes, Result, EByteTypes::ArrayBoolean);
}

TArray<uint8> UInteractionFunctionLibrary::ArrayIntegerToBytes(TArray<int32>& Integers) {
	return ValueToBytes<TArray<int32>>(Integers, EByteTypes::ArrayInteger);
}

void UInteractionFunctionLibrary::ArrayBytesToInteger(const TArray<uint8>& Bytes, TArray<int32>& Result, bool& IsValid) {
	IsValid = BytesToValue<TArray<int32>>(Bytes, Result, EByteTypes::ArrayInteger);
}

TArray<uint8> UInteractionFunctionLibrary::ArrayFloatToBytes(TArray<float>& Floats) {
	return ValueToBytes<TArray<float>>(Floats, EByteTypes::ArrayFloat);
}

void UInteractionFunctionLibrary::ArrayBytesToFloat(const TArray<uint8>& Bytes, TArray<float>& Result, bool& IsValid) {
	IsValid = BytesToValue<TArray<float>>(Bytes, Result, EByteTypes::ArrayFloat);
}

TArray<uint8> UInteractionFunctionLibrary::ArrayStringToBytes(TArray<FString>& Strings) {
	return ValueToBytes<TArray<FString>>(Strings, EByteTypes::ArrayString);
}

void UInteractionFunctionLibrary::ArrayBytesToString(const TArray<uint8>& Bytes, TArray<FString>& Result, bool& IsValid) {
	IsValid = BytesToValue<TArray<FString>>(Bytes, Result, EByteTypes::ArrayString);
}

TArray<uint8> UInteractionFunctionLibrary::ArrayGuidToBytes(TArray<FGuid>& Guid) {
	return ValueToBytes<TArray<FGuid>>(Guid, EByteTypes::ArrayGuid);
}

void UInteractionFunctionLibrary::ArrayBytesToGuid(const TArray<uint8>& Bytes, TArray<FGuid>& Result, bool& IsValid) {
	IsValid = BytesToValue<TArray<FGuid>>(Bytes, Result, EByteTypes::ArrayGuid);
}
