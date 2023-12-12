// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "Inspector/Inspector.h"

#include "VeShared.h"
#include "Components/ArrowComponent.h"
#include "Components/DirectionalLightComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Components/SpotLightComponent.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Kismet/KismetMathLibrary.h"
#include "Inspector/IInspectable.h"
#include "Kismet/GameplayStatics.h"
#include "VeGameFramework.h"

AInspector::AInspector() {
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	OrbitXComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("OrbitX"));
	if (!OrbitXComponent) {
		VeLogErrorFunc("no orbit x component");
		return;
	}
	OrbitXComponent->SetupAttachment(RootComponent);

	OrbitYComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("OrbitY"));
	if (!OrbitYComponent) {
		VeLogErrorFunc("no orbit y component");
		return;
	}
	OrbitYComponent->SetupAttachment(OrbitXComponent);

	SceneCaptureComponent = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("SceneCapture"));
	OrbitXInertia = CreateDefaultSubobject<UInspectInertiaManager>(TEXT("Yaw Orbit Inertia (X)"));
	OrbitYInertia = CreateDefaultSubobject<UInspectInertiaManager>(TEXT("Pitch Orbit Inertia (Y)"));
	PanXInertia = CreateDefaultSubobject<UInspectInertiaManager>(TEXT("Pan Inertia (X)"));
	PanYInertia = CreateDefaultSubobject<UInspectInertiaManager>(TEXT("Pan Inertia (Y)"));
	ZoomInertia = CreateDefaultSubobject<UInspectInertiaManager>(TEXT("Zoom Inertia (Z)"));

	AmbientLightComponent = CreateDefaultSubobject<UDirectionalLightComponent>(TEXT("Ambient Light"));
	DrawLightComponent = CreateDefaultSubobject<USpotLightComponent>(TEXT("Draw Light"));
	FillLightComponent = CreateDefaultSubobject<USpotLightComponent>(TEXT("Fill Light"));
	BackLightComponent = CreateDefaultSubobject<USpotLightComponent>(TEXT("Back Light"));

	if (SceneCaptureComponent) {
		SceneCaptureComponent->SetupAttachment(OrbitYComponent);
		SceneCaptureComponent->SetRelativeLocation(FVector(500.0f, 0.0f, 0.0f));
		SceneCaptureComponent->SetRelativeRotation(FRotator(0.0f, 180.0f, 0.0f));
		SceneCaptureComponent->FOVAngle = 45.0f;

		if (AmbientLightComponent) {
			AmbientLightComponent->SetupAttachment(SceneCaptureComponent);
			AmbientLightComponent->SetRelativeLocation(FVector(-150.0f, 0.0f, 0.0f));
			AmbientLightComponent->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
			AmbientLightComponent->SetUseTemperature(true);
			AmbientLightComponent->SetTemperature(5500.0f);
			AmbientLightComponent->SpecularScale = 0.001f;
			AmbientLightComponent->CastShadows = false;
			AmbientLightComponent->Intensity = 1.0f;
			AmbientLightComponent->LightColor = FColor(255, 250, 245);
			AmbientLightComponent->bCastVolumetricShadow = false;
			AmbientLightComponent->bAffectGlobalIllumination = false;
			AmbientLightComponent->bAtmosphereSunLight = false;
			AmbientLightComponent->SetLightingChannels(false, true, false);
		}

		if (DrawLightComponent) {
			DrawLightComponent->SetupAttachment(SceneCaptureComponent);
			DrawLightComponent->SetRelativeLocation(FVector(-50.0f, -150.0f, 100.0f));
			DrawLightComponent->SetRelativeRotation(FRotator(-10.0f, 20.0f, 0.0f));
			DrawLightComponent->InnerConeAngle = 0.0f;
			DrawLightComponent->OuterConeAngle = 30.0f;
			DrawLightComponent->SetIntensity(10000.0f);
			DrawLightComponent->SetUseTemperature(true);
			DrawLightComponent->SetTemperature(3500);
			DrawLightComponent->SpecularScale = 0.001f;
			DrawLightComponent->CastShadows = true;
			DrawLightComponent->bCastVolumetricShadow = false;
			DrawLightComponent->bAffectGlobalIllumination = false;
			DrawLightComponent->SetLightingChannels(false, true, false);
		}

		if (FillLightComponent) {
			FillLightComponent->SetupAttachment(SceneCaptureComponent);
			FillLightComponent->SetRelativeLocation(FVector(-150.0f, 250.0f, 0.0f));
			FillLightComponent->SetRelativeRotation(FRotator(-5.0f, -10.0f, 0.0f));
			FillLightComponent->SpecularScale = 0.001f;
			FillLightComponent->InnerConeAngle = 0.0f;
			FillLightComponent->OuterConeAngle = 45.0f;
			FillLightComponent->SetIntensity(5000.0f);
			FillLightComponent->SetUseTemperature(true);
			FillLightComponent->SetTemperature(8500);
			FillLightComponent->CastShadows = true;
			FillLightComponent->bCastVolumetricShadow = false;
			FillLightComponent->bAffectGlobalIllumination = false;
			FillLightComponent->SetLightingChannels(false, true, false);
		}

		if (BackLightComponent) {
			BackLightComponent->SetupAttachment(SceneCaptureComponent);
			BackLightComponent->SetRelativeLocation(FVector(1000.0f, 250.0f, 0.0f));
			BackLightComponent->SetRelativeRotation(FRotator(0.0f, 195.0f, 0.0f));
			BackLightComponent->SpecularScale = 0.001f;
			BackLightComponent->InnerConeAngle = 0.0f;
			BackLightComponent->OuterConeAngle = 30.0f;
			BackLightComponent->SetIntensity(5000.0f);
			BackLightComponent->SetUseTemperature(true);
			BackLightComponent->SetTemperature(9500);
			BackLightComponent->CastShadows = true;
			BackLightComponent->bCastVolumetricShadow = false;
			BackLightComponent->bAffectGlobalIllumination = false;
			BackLightComponent->SetLightingChannels(false, true, false);
		}
	}
}

void AInspector::EndPlay(const EEndPlayReason::Type EndPlayReason) {
	ResetInspectedActorLightingChannels();

	if (SceneCaptureComponent) {
		if (const auto TextureRenderTarget = SceneCaptureComponent->TextureTarget) {
			TextureRenderTarget->UpdateResourceImmediate();
		}
	}

	Super::EndPlay(EndPlayReason);
}

void AInspector::SetupInspectedActorLightingChannels() {
	if (InspectedActor) {
		InspectedActor->ForEachComponent(true, [=](UActorComponent* Component) {
			if (auto* PrimitiveComponent = Cast<UPrimitiveComponent>(Component)) {
				// Store original lighting channel values.
				InspectedActorLightingChannels.Add(PrimitiveComponent, PrimitiveComponent->LightingChannels);
				// Enable lighting channel 2.
				PrimitiveComponent->SetLightingChannels(false, true, false);
			}
		});
	}
}

void AInspector::ResetInspectedActorLightingChannels() {
	if (InspectedActor) {
		InspectedActor->ForEachComponent(true, [=](UActorComponent* Component) {
			if (auto* PrimitiveComponent = Cast<UPrimitiveComponent>(Component)) {
				if (const auto* OriginalChannels = InspectedActorLightingChannels.Find(PrimitiveComponent)) {
					PrimitiveComponent->SetLightingChannels(OriginalChannels->bChannel0, OriginalChannels->bChannel1, OriginalChannels->bChannel2);
				}
			}
		});
	}
}

void AInspector::Tick(const float DeltaTime) {
	Super::Tick(DeltaTime);

	float OrbitXVelocity = 0.0f;
	if (OrbitXInertia) {
		OrbitXInertia->UpdateVelocity(DeltaTime);
		OrbitXVelocity = OrbitXInertia->GetVelocity() * OrbitSpeed * DeltaTime;
	}

	float OrbitYVelocity = 0.0f;
	if (OrbitYInertia) {
		OrbitYInertia->UpdateVelocity(DeltaTime);
		OrbitYVelocity = OrbitYInertia->GetVelocity() * OrbitSpeed * DeltaTime;
	}

	float PanXVelocity = 0.0f;
	if (PanXInertia) {
		PanXInertia->UpdateVelocity(DeltaTime);
		PanXVelocity = PanXInertia->GetVelocity() * PanSpeed * DeltaTime;
	}

	float PanYVelocity = 0.0f;
	if (PanYInertia) {
		PanYInertia->UpdateVelocity(DeltaTime);
		PanYVelocity = PanYInertia->GetVelocity() * PanSpeed * DeltaTime;
	}

	float ZoomVelocity = 0.0f;
	if (ZoomInertia) {
		ZoomInertia->UpdateVelocity(DeltaTime);
		ZoomVelocity = ZoomInertia->GetVelocity() * ZoomSpeed * DeltaTime;
	}

	if (InspectedActor) {
		if (SceneCaptureComponent) {
			if (OrbitXComponent && OrbitYComponent) {
				if (!bIsPanning) {
					OrbitXComponent->AddLocalRotation(FRotator(0.0f, OrbitXVelocity, 0.0f));
					OrbitYComponent->AddLocalRotation(FRotator(OrbitYVelocity, 0.0f, 0.0f));
				} else {
					if (!BP_Pan()) {
						const FVector Offset = GetActorRightVector() * PanXVelocity + GetActorUpVector() * PanYVelocity;

						// todo: begin limit object movement to camera frustum
						{
							// FMinimalViewInfo ViewInfo;
							// SceneCaptureComponent->GetCameraView(0.0f, ViewInfo);

							// FMatrix ViewMatrix, ProjectionMatrix, ViewProjectionMatrix;
							// UGameplayStatics::GetViewProjectionMatrix(ViewInfo, ViewMatrix, ProjectionMatrix, ViewProjectionMatrix);
							//
							// FPlane LeftPlane, RightPlane, TopPlane, BottomPlane, NearPlane, FarPlane;
							// ViewMatrix.GetFrustumLeftPlane(LeftPlane);
							// ViewMatrix.GetFrustumRightPlane(RightPlane);
							// ViewMatrix.GetFrustumTopPlane(TopPlane);
							// ViewMatrix.GetFrustumBottomPlane(BottomPlane);
							// ViewMatrix.GetFrustumNearPlane(NearPlane);
							// ViewMatrix.GetFrustumFarPlane(FarPlane);

							// const auto BoundingBox = InspectedActor->GetComponentsBoundingBox(false, false);
							// const auto ActorCenter = BoundingBox.GetCenter();

							// const float LeftPlaneDot = LeftPlane.PlaneDot(ActorCenter);
							// const float RightPlaneDot = RightPlane.PlaneDot(ActorCenter);
							// const float TopPlaneDot = TopPlane.PlaneDot(ActorCenter);
							// const float BottomPlaneDot = BottomPlane.PlaneDot(ActorCenter);

							// if (LeftPlaneDot > 0 || RightPlaneDot > 0) {
							// 	Offset.Y = 0;
							// }
							//
							// if (TopPlaneDot > 0 || BottomPlaneDot > 0) {
							// 	Offset.Z = 0;
							// }

							ScreenColorF(FColor::Magenta, "Offset: %.3f, %.3f, %.3f", 0.0f, Offset.X, Offset.Y, Offset.Z);
						}
						// todo: end

						AddActorWorldOffset(Offset);
					}
				}

			}

			// Zooming
			if (!FMath::IsNearlyEqual(ZoomVelocity, 0.0f)) {
				SceneCaptureComponent->AddRelativeLocation(FVector(ZoomVelocity, 0.0f, 0.0f));
			}
		}
	}
}

void AInspector::ResetView_Implementation() {
	LookDistance = FVector::ZeroVector;
	OrbitAngles = FVector::ZeroVector;

	// Reset location to the inspected actor physical center
	SetActorLocation(GetInspectedActorOrigin());

	if (OrbitXComponent) {
		OrbitXComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
		OrbitXComponent->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
	}

	if (OrbitYComponent) {
		OrbitYComponent->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
		OrbitYComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	}

	if (OrbitXInertia) {
		OrbitXInertia->ClearVelocity();
	}

	if (OrbitYInertia) {
		OrbitYInertia->ClearVelocity();
	}

	if (PanXInertia) {
		PanXInertia->ClearVelocity();
	}

	if (PanYInertia) {
		PanYInertia->ClearVelocity();
	}

	if (InspectedActor && SceneCaptureComponent) {
#pragma region Zoom to fit object
		// Get the bounding box
		const auto BoundingBox = InspectedActor->GetComponentsBoundingBox(false, false);
		// Get the bound box extents
		const FVector Extents = BoundingBox.GetExtent();
		// Take radius as the max component of inspected actor extents
		const float Diameter = Extents.GetAbsMax() * 2;
		// Direction is the normalized difference between the camera and the focus point
		const FVector Direction = (SceneCaptureComponent->GetComponentLocation() - GetInspectedActorOrigin()).GetSafeNormal();
		// Calculate the distance as the object diameter divided by the tangent of the halved camera FOV angle 
		const float Distance = Diameter / FMath::Tan(SceneCaptureComponent->FOVAngle / 2.0f);
		// Move camera to the required distance to fit the object
		SceneCaptureComponent->SetRelativeLocation(Direction * Distance);
		// Rotate camera to look at the focus point at center of the object
		SceneCaptureComponent->SetRelativeRotation(FRotator(0.0f, 180.0f, 0.0f));
#pragma endregion

#pragma region Rotate to face the actor
		// Select the actor forward vector, interactive actors may have a custom forward vector
		FVector ActorForwardVector;
		if (const auto* InteractiveActor = Cast<IInspectable>(InspectedActor)) {
			// ActorForwardVector = InteractiveActor->bUseCustomForwardVector ? InspectedActor->GetActorTransform().TransformPositionNoScale(InteractiveActor->CustomForwardVector).GetSafeNormal() : InspectedActor->GetActorForwardVector();
			ActorForwardVector = InteractiveActor->Execute_GetInspectableForwardVector(InspectedActor);
		} else {
			ActorForwardVector = InspectedActor->GetActorForwardVector();
		}
		// Calculate the yaw angle
		const float YawAngle = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(OrbitXComponent->GetForwardVector(), ActorForwardVector)));
		// Rotate the X component to face the inspected actor
		OrbitXComponent->SetRelativeRotation(FRotator(0.0f, YawAngle, 0.0f));
#pragma endregion
	}
}

void AInspector::SetInspectedActor(AActor* InInspectedActor) {
	InspectedActor = InInspectedActor;

	if (InspectedActor) {
		SetActorLocation(GetInspectedActorOrigin());
		AttachToActor(InspectedActor, FAttachmentTransformRules::KeepWorldTransform);
		SetupInspectedActorLightingChannels();
	}
}

void AInspector::CaptureScene() const {
	if (SceneCaptureComponent && InspectedActor) {
		SceneCaptureComponent->ShowOnlyActorComponents(InspectedActor);
	}

	SceneCaptureComponent->CaptureScene();
}

void AInspector::OrbitBegin() {}

void AInspector::OrbitUpdate(const float DeltaX, const float DeltaY) {
	if (OrbitXInertia) {
		OrbitXInertia->AddSample(DeltaX, FPlatformTime::Seconds());
	}

	if (OrbitYInertia) {
		OrbitYInertia->AddSample(DeltaY, FPlatformTime::Seconds());
	}
}

void AInspector::OrbitEnd() {
	if (OrbitXInertia) {
		OrbitXInertia->AddSample(0, FPlatformTime::Seconds());
	}

	if (OrbitYInertia) {
		OrbitYInertia->AddSample(0, FPlatformTime::Seconds());
	}
}

FVector AInspector::GetInspectedActorOrigin() const {
	if (InspectedActor) {
		FVector Origin, Bounds;
		InspectedActor->GetActorBounds(true, Origin, Bounds);
		return Origin;
	}
	return FVector{0};
}

void AInspector::PanBegin() {
	bIsPanning = true;
}

void AInspector::PanUpdate(const float DeltaX, const float DeltaY) {
	if (PanXInertia) {
		PanXInertia->AddSample(-DeltaX, FPlatformTime::Seconds());
	}

	if (PanYInertia) {
		PanYInertia->AddSample(-DeltaY, FPlatformTime::Seconds());
	}
}

void AInspector::PanEnd() {
	if (PanXInertia) {
		PanXInertia->AddSample(0, FPlatformTime::Seconds());
	}

	if (PanYInertia) {
		PanYInertia->AddSample(0, FPlatformTime::Seconds());
	}

	bIsPanning = false;
}

void AInspector::Zoom(const float ZoomValue) const {
	if (ZoomInertia) {
		ZoomInertia->AddSample(ZoomValue, FPlatformTime::Seconds());
	}
}
