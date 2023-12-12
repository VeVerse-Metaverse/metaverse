// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "Inspector/InspectInertiaManager.h"

UInspectInertiaManager::UInspectInertiaManager() {
	PrimaryComponentTick.bCanEverTick = false;
}

void UInspectInertiaManager::AddSample(const float Delta, const double CurrentTime) {
	new(Samples) FInspectInertiaSample(Delta, CurrentTime);

	// Sum of all sample velocities.
	float Total = 0;

	// Oldest sample time.
	double OldestTime = 0;

	// Use backwards iteration so we can remove older elements.
	for (int32 i = Samples.Num() - 1; i >= 0; --i) {
		const double SampleTime = Samples[i].Time;
		const float SampleDelta = Samples[i].Delta;
		if (CurrentTime - SampleTime > SampleTimeout) {
			Samples.RemoveAt(i);
		} else {
			if (SampleTime < OldestTime || OldestTime == 0) {
				OldestTime = SampleTime;
			}
			Total += SampleDelta;
		}
	}

	// Set the current velocity to the average of the previous recent samples.
	const double Duration = OldestTime > 0 ? CurrentTime - OldestTime : 0;
	if (Duration > 0) {
		Velocity = Total / Duration;
	} else {
		Velocity = 0;
	}
}

void UInspectInertiaManager::UpdateVelocity(const float DeltaTime) {
	const float VelocityLostPerSecond = Velocity > 0 ? StaticVelocityDrag : -StaticVelocityDrag;
	const float DeltaVelocity = FrictionCoefficient * Velocity * DeltaTime + VelocityLostPerSecond * DeltaTime;

	if (Velocity > 0) {
		Velocity = FMath::Max<float>(0.f, Velocity - DeltaVelocity);
	} else if (Velocity < 0) {
		Velocity = FMath::Min<float>(0.f, Velocity - DeltaVelocity);
	}
}

void UInspectInertiaManager::ClearVelocity() {
	Velocity = 0;
}
