// Author: Egor A. Pristavka. Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.


#include "VeTestPawn.h"

#include "UIUserInfoWidget.h"
#include "VeShared.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Components/WidgetComponent.h"

// Sets default values
AVeTestPawn::AVeTestPawn() {
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// UserComponent = CreateDefaultSubobject<UVeUserComponent>("UserComponent");

	UserInfoWidgetComponent = CreateDefaultSubobject<UWidgetComponent>("UserInfoWidgetComponent");
	UserInfoWidgetComponent->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AVeTestPawn::BeginPlay() {
	Super::BeginPlay();

	// Subscribe for the metadata received from the server.
	// if (GetNetMode() != NM_DedicatedServer) {
	// 	if (!OnUserMetadataUpdatedDelegateHandle.IsValid()) {
	// 		if (UserComponent) {
	// 			OnUserMetadataUpdatedDelegateHandle = UserComponent->GetOnUserMetadataUpdated().AddWeakLambda(this, [this](const FVeUserMetadata& InMetadata) {
	// 				if (UUserWidget* Widget = UserInfoWidgetComponent->GetWidget()) {
	// 					if (UUIUserInfoWidget* UserInfoWidget = Cast<UUIUserInfoWidget>(Widget)) {
	// 						UserInfoWidget->SetMetadata(InMetadata);
	// 					}
	// 				}
	// 			});
	// 		}
	// 	}
	// }

	// Test user component "authenticating" with random user metadata.
	// if (IsLocallyControlled()) {
		// if (UserComponent) {
			// const int Random = FMath::RandRange(0, 100);
			// if (Random <= 30) {
				// UserComponent->ServerRPC_RequestUserMetadata(FGuid(TEXT("f0000000-0000-4000-a000-00000000000e")));
			// } else if (Random <= 60) {
				// UserComponent->ServerRPC_RequestUserMetadata(FGuid(TEXT("f0000000-0000-4000-a000-00000000000b")));
			// } else if (Random <= 90) {
				// UserComponent->ServerRPC_RequestUserMetadata(FGuid(TEXT("f0000000-0000-4000-a000-00000000000d")));
			// }
		// }
	// }

	if (!IsLocallyControlled()) {
		// Test voice widget.
		if (!IsValid(this)) {
			if (const UWorld* const World = GetWorld()) {
				FTimerHandle __tempTimerHandle;
				World->GetTimerManager().SetTimer(__tempTimerHandle, FTimerDelegate().CreateWeakLambda(this, [this, World]() {
					TArray<AActor*> FoundActors;
					UGameplayStatics::GetAllActorsOfClass(World, AVeTestPawn::StaticClass(), FoundActors);

					for (const AActor* Actor : FoundActors) {
						if (const AVeTestPawn* Pawn = Cast<AVeTestPawn>(Actor)) {
							if (Pawn != this) {
								if (const UUIUserInfoWidget* UserInfoWidget = Cast<UUIUserInfoWidget>(Pawn->UserInfoWidgetComponent->GetUserWidgetObject())) {
									const EVoiceWidgetStatus Rand = StaticCast<EVoiceWidgetStatus>(FMath::RandRange(0, 3));
									// UserInfoWidget->SetVoiceStatus(Rand);
								}
							}
						}
					}
				}), 5.0f, true);
			}
		}
	}
}

void AVeTestPawn::EndPlay(const EEndPlayReason::Type EndPlayReason) {
	Super::EndPlay(EndPlayReason);

	// if (GetNetMode() != NM_DedicatedServer) {
		// if (OnUserMetadataUpdatedDelegateHandle.IsValid()) {
			// if (UserComponent) {
				// UserComponent->GetOnUserMetadataUpdated().Remove(OnUserMetadataUpdatedDelegateHandle);
			// }
			// OnUserMetadataUpdatedDelegateHandle.Reset();
		// }
	// }
}

void AVeTestPawn::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

// Called every frame
void AVeTestPawn::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AVeTestPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) {
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}
