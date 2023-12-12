// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "EditorBillboardComponent.h"

#include "GameFrameworkEditorSubsystem.h"
#include "LevelUtils.h"
#include "VeGameFramework.h"
#include "VeShared.h"
#include "Components/LightComponent.h"
#include "Engine/LevelStreaming.h"

namespace BillboardConstants {
	static constexpr float DefaultScreenSize = 0.0025f;
};

/** Represents a billboard sprite to the scene manager. */
class FSpriteSceneProxy final : public FPrimitiveSceneProxy {
	FVector Origin;
	const float ScreenSize;
	const UTexture2D* Texture;
	float Scale;
	const float U;
	float ComponentUL;
	const float V;
	float ComponentVL;
	float OpacityMaskRefVal;
	FLinearColor Color;
	const uint32 bIsScreenSizeScaled : 1;
	uint32 bIsActorLocked : 1;

public:
	virtual SIZE_T GetTypeHash() const override {
		static size_t UniquePointer;
		return reinterpret_cast<size_t>(&UniquePointer);
	}

	FSpriteSceneProxy(const UEditorBillboardComponent* InComponent, float SpriteScale)
		: FPrimitiveSceneProxy(InComponent), ScreenSize(InComponent->ScreenSize),
		  U(InComponent->U), V(InComponent->V),
		  Color(FLinearColor::White), bIsScreenSizeScaled(InComponent->bIsScreenSizeScaled) {

		bWillEverBeLit = 0;

		if (IsValid(InComponent)) {
			// Calculate the scale factor for the sprite.
			Scale = InComponent->GetComponentTransform().GetMaximumAxisScale() * SpriteScale * 0.25f;

			OpacityMaskRefVal = InComponent->OpacityMaskRefVal;

			if (InComponent->Sprite) {
				Texture = InComponent->Sprite;
				// Set UL and VL to the size of the texture if they are set to 0.0, otherwise use the given value.
				ComponentUL = InComponent->UL;
				ComponentVL = InComponent->VL;
			} else {
				Texture = nullptr;
				ComponentUL = ComponentVL = 0;
			}

			if (const auto* Owner = InComponent->GetOwner()) {
				if (const auto* LightComponent = Owner->FindComponentByClass<ULightComponent>()) {
					Color = LightComponent->GetLightColor();
					Color.A = 0xFF;
				} else {
					Color = FLinearColor::White;
				}

				bIsActorLocked = false;

				// Level colorization
				const ULevel* Level = Owner->GetLevel();
				if (const auto* LevelStreaming = FLevelUtils::FindStreamingLevel(Level)) {
					SetLevelColor(LevelStreaming->LevelColor);
				}
			}

			FColor NewPropertyColor;
			if (GEngine->GetPropertyColorationColor((UObject*)(InComponent), NewPropertyColor)) {
				SetPropertyColor(NewPropertyColor);
			}
		}

	}

	virtual void GetDynamicMeshElements(const TArray<const FSceneView*>& Views, const FSceneViewFamily& ViewFamily, uint32 VisibilityMap, FMeshElementCollector& Collector) const override {
		if (auto* TextureResource = Texture ? Texture->GetResource() : nullptr) {
			const float UL = ComponentUL == 0.0f ? TextureResource->GetSizeX() : ComponentUL;
			const float VL = ComponentVL == 0.0f ? TextureResource->GetSizeY() : ComponentVL;

			for (int32 ViewIndex = 0; ViewIndex < Views.Num(); ViewIndex++) {
				if (VisibilityMap & 1 << ViewIndex) {
					const auto* View = Views[ViewIndex];
					// Calculate the view-dependent scaling factor.
					float ViewedSizeX = Scale * UL;
					float ViewedSizeY = Scale * VL;

					if (bIsScreenSizeScaled && View->ViewMatrices.GetProjectionMatrix().M[3][3] != 1.0f) {
						const float ZoomFactor = FMath::Min<float>(View->ViewMatrices.GetProjectionMatrix().M[0][0], View->ViewMatrices.GetProjectionMatrix().M[1][1]);
						if (ZoomFactor != 0.0f) {
							const float Radius = View->WorldToScreen(Origin).W * (ScreenSize / ZoomFactor);

							if (Radius < 1.0f) {
								ViewedSizeX *= Radius;
								ViewedSizeY *= Radius;
							}
						}
					}
					FLinearColor ColorToUse = Color;
					if (IsSelected()) {
						ColorToUse = FLinearColor::White + (GEngine->GetSelectedMaterialColor() * GEngine->SelectionHighlightIntensityBillboards * 10);
					} else if (IsHovered()) {
						ColorToUse = FLinearColor::White + (GEngine->GetHoveredMaterialColor() * GEngine->SelectionHighlightIntensityBillboards * 10);
					}

					if (bIsActorLocked) {
						ColorToUse = FColor::Red;
					}
					FLinearColor LevelColorToUse = IsSelected() ? ColorToUse : (FLinearColor)GetLevelColor();
					FLinearColor PropertyColorToUse = GetPropertyColor();

					ColorToUse.A = 1.0f;

					const FLinearColor& SpriteColor = View->Family->EngineShowFlags.LevelColoration ? LevelColorToUse : View->Family->EngineShowFlags.PropertyColoration ? PropertyColorToUse : ColorToUse;

					Collector.GetPDI(ViewIndex)->DrawSprite(
						Origin,
						ViewedSizeX,
						ViewedSizeY,
						TextureResource,
						SpriteColor,
						SDPG_Foreground,
						U, UL, V, VL,
						SE_BLEND_Masked,
						OpacityMaskRefVal
						);
				}
			}
		}
	}

	virtual FPrimitiveViewRelevance GetViewRelevance(const FSceneView* View) const override {
		FPrimitiveViewRelevance Result;
		Result.bDrawRelevance = IsShown(View);
		Result.bOpaque = true;
		Result.bDynamicRelevance = true;
		Result.bShadowRelevance = IsShadowCast(View);
		Result.bEditorPrimitiveRelevance = UseEditorCompositing(View);
		return Result;
	}

	virtual void OnTransformChanged() override {
		Origin = GetLocalToWorld().GetOrigin();
	}

	virtual uint32 GetMemoryFootprint() const override {
		return sizeof *this + GetAllocatedSize();
	}
};

UEditorBillboardComponent::UEditorBillboardComponent() {
	PrimaryComponentTick.bCanEverTick = false;

	struct FConstructorStatics {
		ConstructorHelpers::FObjectFinder<UTexture2D> SpriteTexture;

		FConstructorStatics()
			: SpriteTexture(TEXT("/VeUI/UI/Textures/T_Placeable")) { }
	};

	static FConstructorStatics ConstructorStatics;
	if (ConstructorStatics.SpriteTexture.Succeeded()) {
		Sprite = ConstructorStatics.SpriteTexture.Object;
	}

	// UPrimitiveComponent::SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
	SetUsingAbsoluteScale(true);

	bIsScreenSizeScaled = 1;
	ScreenSize = BillboardConstants::DefaultScreenSize;
	U = 0;
	V = 0;
	UL = 0;
	VL = 0;
	OpacityMaskRefVal = .25f;
	bHiddenInGame = true; // By default hide in game, but will be enabled in editor mode
	SetGenerateOverlapEvents(true);
	bExcludeFromLightAttachmentGroup = true;
	// bRenderCustomDepth = 1;

#if WITH_EDITORONLY_DATA
	bUseEditorCompositing = true;
#endif
}

void UEditorBillboardComponent::BeginPlay() {
	Super::BeginPlay();

	GET_MODULE_SUBSYSTEM(EditorSubsystem, GameFramework, Editor);
	if (EditorSubsystem) {
		EditorSubsystem->GetOnEditorModeEnabled().AddWeakLambda(this, [this] {
			SetHiddenInGame(bForceInvisibility);
		});
		EditorSubsystem->GetOnEditorModeDisabled().AddWeakLambda(this, [this] {
			SetHiddenInGame(true);
		});

		if (bForceInvisibility) {
			SetHiddenInGame(true);
		} else {
			SetHiddenInGame(!EditorSubsystem->IsEditorModeEnabled());
		}
	}
}

FPrimitiveSceneProxy* UEditorBillboardComponent::CreateSceneProxy() {
	float SpriteScale = 1.0f;
#if WITH_EDITORONLY_DATA
	if (IsValid(GetOwner())) {
		SpriteScale = GetOwner()->SpriteScale;
	}
#endif
	return new FSpriteSceneProxy(this, SpriteScale);
}

FBoxSphereBounds UEditorBillboardComponent::CalcBounds(const FTransform& LocalToWorld) const {
	const float NewScale = LocalToWorld.GetScale3D().GetMax() * (Sprite ? static_cast<float>(FMath::Max(Sprite->GetSizeX(), Sprite->GetSizeY())) : 1.0f);
	return FBoxSphereBounds(LocalToWorld.GetLocation(), FVector(NewScale, NewScale, NewScale), FMath::Sqrt(3.0f * FMath::Square(NewScale)));
}

void UEditorBillboardComponent::SetForceInvisible(const bool bInInvisible) {
	bForceInvisibility = bInInvisible;
	SetHiddenInGame(bInInvisible);
}

void UEditorBillboardComponent::SetSprite(UTexture2D* NewSprite) {
	Sprite = NewSprite;
	MarkRenderStateDirty();
}

void UEditorBillboardComponent::SetUV(const int32 NewU, const int32 NewUL, const int32 NewV, const int32 NewVL) {
	U = NewU;
	UL = NewUL;
	V = NewV;
	VL = NewVL;
	MarkRenderStateDirty();
}

void UEditorBillboardComponent::SetSpriteAndUV(UTexture2D* NewSprite, int32 NewU, int32 NewUL, int32 NewV, int32 NewVL) {
	U = NewU;
	UL = NewUL;
	V = NewV;
	VL = NewVL;
	SetSprite(NewSprite);
}

void UEditorBillboardComponent::SetOpacityMaskRefVal(float RefVal) {
	OpacityMaskRefVal = RefVal;
	MarkRenderStateDirty();
}
