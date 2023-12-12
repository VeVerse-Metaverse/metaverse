// Authors: Egor A. Pristavka, Nikolay Bulatov. 
// Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once

#include "InspectableActor.h"
#include "DEPRECATED_PlaceablePdf.generated.h"

class UWidgetComponent;

UCLASS(Blueprintable, NotBlueprintType, Deprecated)
class VEGAME_API ADEPRECATED_PlaceablePdf : public AInspectableActor {
	GENERATED_BODY()

public:
	ADEPRECATED_PlaceablePdf();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/** Widget component ot render the PDF pages. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UWidgetComponent* WidgetComponent = nullptr;

	/** Reload current page image. */
	UFUNCTION(BlueprintCallable, BlueprintCosmetic)
	void Client_Refresh();

#pragma region Navigation
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Placeable|Pdf")
	int32 CurrentPage = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Placeable|Pdf", meta=(DisplayPriority=0))
	TArray<FString> PageUrls;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing="OnRep_CurrentPageUrl", Category="Placeable|Pdf")
	FString CurrentPageUrl;

	UFUNCTION()
	void OnRep_CurrentPageUrl();

	/** Go to the first page. */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void Authority_FirstPage();

	/** Go to the next page. */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void Authority_NextPage();

	/** Go to the previous page. */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void Authority_PreviousPage();

	/** Process navigation inputs at the client. */
	virtual void ClientInputBegin_Implementation(UInteractionSubjectComponent* Subject, UInteractionObjectComponent* Object, const FName& Name,
		const FVector& Origin, const FVector& Location, const FVector& Normal);

	/** Server  */
	virtual void ServerEvent_Implementation(UInteractionSubjectComponent* Subject, UInteractionObjectComponent* Object, const FName& Name, const TArray<uint8>& Args);

#pragma endregion

	/** Save callback */
	virtual void OnSave(const FString& Url);

protected:
	/** Class to use when creating a editor properties widget instance. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Placeable", meta=(DisplayPriority=0))
	TSubclassOf<class UUIEditorUrlPropertiesWidget> PlaceablePropertiesClass;

	virtual UUIPlaceablePropertiesWidget* GetPlaceablePropertiesWidget_Implementation(APlayerController* PlayerController) override;
};
