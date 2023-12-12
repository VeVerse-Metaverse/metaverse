// 

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "VeSharedLibrary.generated.h"

/**
 * 
 */
UCLASS()
class VESHARED_API UVeSharedLibrary : public UBlueprintFunctionLibrary {
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category="JSON", CustomThunk, meta=(CustomStructureParam="Payload"))
	static FString ValueToJson(UProperty* Payload);
	
	static FString ValueToJson(FProperty* Property, const void* Value);

	UFUNCTION(BlueprintPure, Category="VeShared")
	static FString PtrToString(UObject* Object);
	
private:

	DECLARE_FUNCTION(execValueToJson);
	static FString Generic_ValueToJson(FProperty* Property, const void* Value);

	/*
	 * Function for iterating through all properties of a struct
	 * @param StructProperty    The struct property reflection data
	 * @param StructPtr        The pointer to the struct value
	 */
	static FString ValueToJson_StructProperty(FStructProperty* InStructProperty, const void* InValue);


	/* Function for parsing a single property
	 * @param Property    the property reflection data
	 * @param ValuePtr    the pointer to the property value
	 */
	static FString ValueToJson_Property(FProperty* InProperty, const void* InValue);
};
