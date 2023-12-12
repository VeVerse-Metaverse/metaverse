// Authors: Egor A. Pristavka, Nikolay Bulatov. 
// Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "UI/DEPRECATED_UIPlaceablePropertiesWidgetOLD.h"

#include "PlaceableObject.h"


// void UUIPlaceablePropertiesPageWidget::SetPlaceableObject(APlaceableObject* InPlaceableObject) {
// 	PlaceableObject = InPlaceableObject;
// }

TArray<FVePropertyMetadata> UDEPRECATED_UIPlaceablePropertiesWidgetOLD::GetProperties_Implementation() const {
	return {};
}

void UDEPRECATED_UIPlaceablePropertiesWidgetOLD::SetProperties_Implementation(const TArray<FVePropertyMetadata>& Properties) {
	
}
