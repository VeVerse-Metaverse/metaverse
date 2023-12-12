// 

#include "VeSharedLibrary.h"

#include "VeShared.h"

DEFINE_FUNCTION(UVeSharedLibrary::execValueToJson) {
	Stack.Step(Stack.Object, NULL);

	FProperty* Property = Stack.MostRecentProperty;

	void* Value = Stack.MostRecentPropertyAddress;

	P_FINISH;
	P_NATIVE_BEGIN;
		FString& ResultStr = *(FString*)RESULT_PARAM;
		ResultStr = Generic_ValueToJson(Property, Value);
	P_NATIVE_END;
}


FString UVeSharedLibrary::Generic_ValueToJson(FProperty* Property, const void* Value) {
	if (FStructProperty* StructProperty = CastField<FStructProperty>(Property)) {
		return ValueToJson_StructProperty(StructProperty, Value);
	} else {
		return TEXT("{\"") + Property->GetAuthoredName() + TEXT("\": ") + ValueToJson_Property(Property, Value) + TEXT("}");
	}
}

FString UVeSharedLibrary::ValueToJson(FProperty* Property, const void* Value) {
	return Generic_ValueToJson(Property, Value);
}

FString UVeSharedLibrary::PtrToString(UObject* Object) {
	return FString::Printf(TEXT("%p"), Object);
}

FString UVeSharedLibrary::ValueToJson_StructProperty(FStructProperty* InStructProperty, const void* InValue) {
	TArray<FString> Strings;

	// Walk the structs' properties
	UScriptStruct* Struct = InStructProperty->Struct;
	for (TFieldIterator<FProperty> It(Struct); It; ++It) {
		FProperty* Property = *It;

		// This is the variable name if you need it
		FString VariableName = Property->GetName();

		// Never assume ArrayDim is always 1
		for (int32 ArrayIndex = 0; ArrayIndex < Property->ArrayDim; ArrayIndex++) {
			// This grabs the pointer to where the property value is stored
			const void* Value = Property->ContainerPtrToValuePtr<void>(InValue, ArrayIndex);

			// Parse this property
			Strings.Emplace(TEXT("\"") + Property->GetAuthoredName() + ("\": ") + ValueToJson_Property(Property, Value));
		}
	}

	return TEXT("{") + FString::Join(Strings, TEXT(", ")) + "}";
}

FString UVeSharedLibrary::ValueToJson_Property(FProperty* InProperty, const void* InValue) {

	// Here's how to read integer and float properties
	if (auto* NumericProperty = CastField<FNumericProperty>(InProperty)) {
		if (NumericProperty->IsFloatingPoint()) {
			return FString::SanitizeFloat(NumericProperty->GetFloatingPointPropertyValue(InValue));
		}
		if (NumericProperty->IsInteger()) {
			return FString::FromInt(NumericProperty->GetSignedIntPropertyValue(InValue));
		}
	}

	// How to read booleans
	if (auto* BoolProperty = CastField<FBoolProperty>(InProperty)) {
		const bool BoolValue = BoolProperty->GetPropertyValue(InValue);
		return BoolValue ? TEXT("true") : TEXT("false");
	}

	// Reading names
	if (auto* NameProperty = CastField<FNameProperty>(InProperty)) {
		return TEXT("\"") + NameProperty->GetPropertyValue(InValue).ToString() + TEXT("\"");
	}

	// Reading strings
	if (auto* StringProperty = CastField<FStrProperty>(InProperty)) {
		return TEXT("\"") + StringProperty->GetPropertyValue(InValue) + TEXT("\"");
	}

	// Reading texts
	if (auto* TextProperty = CastField<FTextProperty>(InProperty)) {
		return TEXT("\"") + TextProperty->GetPropertyValue(InValue).ToString() + TEXT("\"");
	}

	// Reading an array
	if (auto* ArrayProperty = CastField<FArrayProperty>(InProperty)) {
		// We need the helper to get to the items of the array            
		FScriptArrayHelper Helper(ArrayProperty, InValue);
		TArray<FString> Strings;
		Strings.Reserve(Helper.Num());
		for (int32 i = 0, n = Helper.Num(); i < n; ++i) {
			Strings.Emplace(ValueToJson_Property(ArrayProperty->Inner, Helper.GetRawPtr(i)));
		}
		return TEXT("[") + FString::Join(Strings, TEXT(", ")) + TEXT("]");
	}

	// Reading a nested struct
	if (FStructProperty* StructProperty = CastField<FStructProperty>(InProperty)) {
		return ValueToJson_StructProperty(StructProperty, InValue);
	}

	return TEXT("\"\"");
}
