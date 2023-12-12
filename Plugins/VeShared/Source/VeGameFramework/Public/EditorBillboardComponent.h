// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.



#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EditorBillboardComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class VEGAMEFRAMEWORK_API UEditorBillboardComponent : public UPrimitiveComponent {
	GENERATED_BODY()

public:
	UEditorBillboardComponent();

	virtual void BeginPlay() override;

	/** Use this flag to force change component visibility (e.g. when it is no longer needed having another visible primitive). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Sprite)
	bool bForceInvisibility = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Sprite)
	TObjectPtr<UTexture2D> Sprite;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Sprite)
	uint32 bIsScreenSizeScaled:1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Sprite)
	float ScreenSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Sprite)
	float U;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Sprite)
	float UL;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Sprite)
	float V;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Sprite)
	float VL;

	/** The billboard is not rendered where texture opacity < OpacityMaskRefVal */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, AdvancedDisplay, Category=Sprite)
	float OpacityMaskRefVal;

	/** Change the sprite texture used by this component */
	UFUNCTION(BlueprintCallable, Category="Rendering|Components|Sprite")
	virtual void SetForceInvisible(bool bInInvisible);

	/** Change the sprite texture used by this component */
	UFUNCTION(BlueprintCallable, Category="Rendering|Components|Sprite")
	virtual void SetSprite(UTexture2D* NewSprite);

	/** Change the sprite's UVs */
	UFUNCTION(BlueprintCallable, Category="Rendering|Components|Sprite")
	virtual void SetUV(int32 NewU, int32 NewUL, int32 NewV, int32 NewVL);

	/** Change the sprite texture and the UV's used by this component */
	UFUNCTION(BlueprintCallable, Category="Rendering|Components|Sprite")
	virtual void SetSpriteAndUV(UTexture2D* NewSprite, int32 NewU, int32 NewUL, int32 NewV, int32 NewVL);

	/** Changed the opacity masked used by this component */
	UFUNCTION(BlueprintCallable, Category = "Rendering|Components|Sprite")
	void SetOpacityMaskRefVal(float RefVal);

	//~ Begin UPrimitiveComponent Interface
	virtual FPrimitiveSceneProxy* CreateSceneProxy() override;
	virtual FBoxSphereBounds CalcBounds(const FTransform& LocalToWorld) const override;
	//~ End UPrimitiveComponent Interface

};
