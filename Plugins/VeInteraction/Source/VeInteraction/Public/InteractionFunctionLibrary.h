// Author: Egor A. Pristavka. 
// Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "InteractionFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class VEINTERACTION_API UInteractionFunctionLibrary : public UBlueprintFunctionLibrary {
	GENERATED_BODY()

	enum class EByteTypes : uint8 {
		None,
		Boolean,
		Integer,
		Float,
		Vector,
		Rotator,
		Transform,
		LinearColor,
		Guid,
		String,

		ArrayBoolean,
		ArrayInteger,
		ArrayFloat,
		ArrayString,
		ArrayGuid,
	};
	
public:
	UFUNCTION(BlueprintPure, Category="Interaction|Bytes")
	static TArray<uint8> BooleanToBytes(bool Boolean);

	UFUNCTION(BlueprintCallable, Category="Interaction|Bytes")
	static void BytesToBoolean(const TArray<uint8>& Bytes, bool& Result, bool& IsValid);

	UFUNCTION(BlueprintPure, Category="Interaction|Bytes")
	static TArray<uint8> IntegerToBytes(int32 Integer);

	UFUNCTION(BlueprintCallable, Category="Interaction|Bytes")
	static void BytesToInteger(const TArray<uint8>& Bytes, int32& Result, bool& IsValid);

	UFUNCTION(BlueprintPure, Category="Interaction|Bytes")
	static TArray<uint8> FloatToBytes(float Float);

	UFUNCTION(BlueprintCallable, Category="Interaction|Bytes")
	static void BytesToFloat(const TArray<uint8>& Bytes, float& Result, bool& IsValid);

	UFUNCTION(BlueprintPure, Category="Interaction|Bytes")
	static TArray<uint8> VectorToBytes(FVector Vector);

	UFUNCTION(BlueprintCallable, Category="Interaction|Bytes")
	static void BytesToVector(const TArray<uint8>& Bytes, FVector& Result, bool& IsValid);

	UFUNCTION(BlueprintPure, Category="Interaction|Bytes")
	static TArray<uint8> RotatorToBytes(FRotator Rotator);

	UFUNCTION(BlueprintCallable, Category="Interaction|Bytes")
	static void BytesToRotator(const TArray<uint8>& Bytes, FRotator& Result, bool& IsValid);

	UFUNCTION(BlueprintPure, Category="Interaction|Bytes")
	static TArray<uint8> TransformToBytes(FTransform Transform);

	UFUNCTION(BlueprintCallable, Category="Interaction|Bytes")
	static void BytesToTransform(const TArray<uint8>& Bytes, FTransform& Result, bool& IsValid);

	UFUNCTION(BlueprintPure, Category="Interaction|Bytes")
	static TArray<uint8> LinearColorToBytes(FLinearColor LinearColor);

	UFUNCTION(BlueprintCallable, Category="Interaction|Bytes")
	static void BytesToLinearColor(const TArray<uint8>& Bytes, FLinearColor& Result, bool& IsValid);

	UFUNCTION(BlueprintPure, Category="Interaction|Bytes")
	static TArray<uint8> GuidToBytes(FGuid Guid);

	UFUNCTION(BlueprintCallable, Category="Interaction|Bytes")
	static void BytesToGuid(const TArray<uint8>& Bytes, FGuid& Result, bool& IsValid);

	UFUNCTION(BlueprintPure, Category="Interaction|Bytes")
	static TArray<uint8> StringToBytes(FString String);

	UFUNCTION(BlueprintCallable, Category="Interaction|Bytes")
	static void BytesToString(const TArray<uint8>& Bytes, FString& Result, bool& IsValid);

	// UObject
	
	static TArray<uint8> ObjectToBytes(UObject* Object);
	static UObject* BytesToObject(const FString& ClassName, const TArray<uint8>& Bytes);

	// Arrays
	
	UFUNCTION(BlueprintPure, Category="Interaction|Bytes")
	static TArray<uint8> ArrayBooleanToBytes(UPARAM(ref) TArray<bool>& Booleans);
	
	UFUNCTION(BlueprintCallable, Category="Interaction|Bytes")
	static void ArrayBytesToBoolean(const TArray<uint8>& Bytes, TArray<bool>& Result, bool& IsValid);
	
	UFUNCTION(BlueprintPure, Category="Interaction|Bytes")
	static TArray<uint8> ArrayIntegerToBytes(UPARAM(ref) TArray<int32>& Integers);
	
	UFUNCTION(BlueprintCallable, Category="Interaction|Bytes")
	static void ArrayBytesToInteger(const TArray<uint8>& Bytes, TArray<int32>& Result, bool& IsValid);
	
	UFUNCTION(BlueprintPure, Category="Interaction|Bytes")
	static TArray<uint8> ArrayFloatToBytes(UPARAM(ref) TArray<float>& Floats);
	
	UFUNCTION(BlueprintCallable, Category="Interaction|Bytes")
	static void ArrayBytesToFloat(const TArray<uint8>& Bytes, TArray<float>& Result, bool& IsValid);
	
	UFUNCTION(BlueprintPure, Category="Interaction|Bytes")
	static TArray<uint8> ArrayStringToBytes(UPARAM(ref) TArray<FString>& Strings);
	
	UFUNCTION(BlueprintCallable, Category="Interaction|Bytes")
	static void ArrayBytesToString(const TArray<uint8>& Bytes, TArray<FString>& Result, bool& IsValid);
	
	UFUNCTION(BlueprintPure, Category="Interaction|Bytes")
	static TArray<uint8> ArrayGuidToBytes(UPARAM(ref) TArray<FGuid>& Guid);
	
	UFUNCTION(BlueprintCallable, Category="Interaction|Bytes")
	static void ArrayBytesToGuid(const TArray<uint8>& Bytes, TArray<FGuid>& Result, bool& IsValid);

protected:
	template <class T>
	static TArray<uint8> ValueToBytes(T& Value, EByteTypes Type) {
		TArray<uint8> Bytes;
		FMemoryWriter Writer(Bytes);
		Writer << Type;
		Writer << Value;
		return Bytes;
	}

	template <class T>
	static bool BytesToValue(const TArray<uint8>& Bytes, T& Value, EByteTypes Type) {
		FMemoryReader Reader(Bytes);
		EByteTypes BytesType;
		Reader << BytesType;
		if (BytesType != Type) {
			return false;
		}
		Reader << Value;
		return true;
	}

};
