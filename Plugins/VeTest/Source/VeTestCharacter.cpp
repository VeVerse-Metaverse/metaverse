// Author: Egor A. Pristavka. Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.


#include "VeTestCharacter.h"

#include "UIUserInfoWidget.h"
#include "VePlayerController.h"
#include "Components/WidgetComponent.h"
#include "Engine/TextureRenderTarget2D.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "VeShared.h"

// Sets default values
AVeTestCharacter::AVeTestCharacter(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	// UserComponent = CreateDefaultSubobject<UVeUserComponent>("UserComponent");

	UserInfoWidgetComponent = CreateDefaultSubobject<UWidgetComponent>("UserInfoWidgetComponent");
	UserInfoWidgetComponent->SetupAttachment(RootComponent);

	UserVRMenuWidgetComponent = CreateDefaultSubobject<UWidgetComponent>("UserVRMenuWidgetComponent");
	UserVRMenuWidgetComponent->SetupAttachment(RootComponent);

	StereoLayer = CreateDefaultSubobject<UStereoLayerComponent>("StereoLayer");
	if (StereoLayer) {
		StereoLayer->bLiveTexture = true;
	} else {
		LogErrorF("failed to create stereo layer component");
	}
}

// Called when the game starts or when spawned
void AVeTestCharacter::BeginPlay() {
	Super::BeginPlay();

	// Subscribe for the metadata received from the server.
	if (GetNetMode() != NM_DedicatedServer) {
		if (!OnUserMetadataUpdatedDelegateHandle.IsValid()) {
			// if (UserComponent) {
			// 	OnUserMetadataUpdatedDelegateHandle = UserComponent->GetOnUserMetadataUpdated().AddWeakLambda(this, [this](const FVeUserMetadata& InMetadata) {
			// 		if (UUserWidget* Widget = UserInfoWidgetComponent->GetWidget()) {
			// 			if (UUIUserInfoWidget* UserInfoWidget = Cast<UUIUserInfoWidget>(Widget)) {
			// 				UserInfoWidget->SetMetadata(InMetadata);
			// 			}
			// 		}
			// 	});
			// }
		}
	}

	// Test user component "authenticating" with random user metadata.
	if (IsLocallyControlled()) {
		// if (UserComponent) {
		// 	const int Random = FMath::RandRange(0, 100);
		// 	if (Random <= 30) {
		// 		// UserComponent->ServerRPC_RequestUserMetadata(FGuid(TEXT("f0000000-0000-4000-a000-00000000000e")));
		// 	} else if (Random <= 60) {
		// 		// UserComponent->ServerRPC_RequestUserMetadata(FGuid(TEXT("f0000000-0000-4000-a000-00000000000b")));
		// 	} else if (Random <= 90) {
		// 		// UserComponent->ServerRPC_RequestUserMetadata(FGuid(TEXT("f0000000-0000-4000-a000-00000000000d")));
		// 	}
		// }
	}

	if (!IsLocallyControlled()) {
		if (!IsValid(this)) {
			if (const UWorld* const World = GetWorld()) {
				// Test voice widget.
				FTimerHandle TempTimerHandle;
				World->GetTimerManager().SetTimer(TempTimerHandle, FTimerDelegate().CreateWeakLambda(this, [this, World]() {
					TArray<AActor*> FoundActors;
					UGameplayStatics::GetAllActorsOfClass(World, StaticClass(), FoundActors);

					for (const AActor* Actor : FoundActors) {
						if (const AVeTestCharacter* Pawn = Cast<AVeTestCharacter>(Actor)) {
							if (Pawn != this) {
								if ([[maybe_unused]] const UUIUserInfoWidget* UserInfoWidget = Cast<UUIUserInfoWidget>(Pawn->UserInfoWidgetComponent->GetUserWidgetObject())) {
									[[maybe_unused]] const EVoiceWidgetStatus Rand = StaticCast<EVoiceWidgetStatus>(FMath::RandRange(0, 3));
									// UserInfoWidget->SetVoiceStatus(Rand);
								}
							}
						}
					}
				}), 5.0f, true);

				// Rotate user information widgets of every simulated (remote) pawn to face the local player's camera. 
				if (!UserInfoWidgetUpdateTimer.IsValid()) {
					World->GetTimerManager().SetTimer(UserInfoWidgetUpdateTimer, FTimerDelegate().CreateWeakLambda(this, [this, World]() {
						if (GetLocalRole() == ROLE_SimulatedProxy) {
							if (const AVePlayerController* PlayerController = World->GetFirstPlayerController<AVePlayerController>()) {
								if (const APlayerCameraManager* CameraManager = PlayerController->PlayerCameraManager) {
									const FVector CameraLocation = CameraManager->GetCameraLocation();
									FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(UserInfoWidgetComponent->GetComponentLocation(), CameraLocation);
									const FRotator OriginalRotation = UserInfoWidgetComponent->GetComponentRotation();
									LookAtRotation.Pitch = OriginalRotation.Pitch;
									LookAtRotation.Roll = OriginalRotation.Roll;
									UserInfoWidgetComponent->SetWorldRotation(LookAtRotation);
								}
							}
						}
					}), 0.25f, true);
				}
			}
		}
	}
}

void AVeTestCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason) {
	Super::EndPlay(EndPlayReason);

	// Unsubscribe from metadata updates.
	// if (GetNetMode() != NM_DedicatedServer) {
		// if (OnUserMetadataUpdatedDelegateHandle.IsValid()) {
			// if (UserComponent) {
				// UserComponent->GetOnUserMetadataUpdated().Remove(OnUserMetadataUpdatedDelegateHandle);
			// }
			// OnUserMetadataUpdatedDelegateHandle.Reset();
		// }
	// }

	// Clear user info widget update timer.
	if (UserInfoWidgetUpdateTimer.IsValid()) {
		if (const UWorld* const World = GetWorld()) {
			World->GetTimerManager().ClearTimer(UserInfoWidgetUpdateTimer);
		}
	}
}

void AVeTestCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void AVeTestCharacter::Input_MoveForward(const float InAxis) {
	const auto Direction = this->GetActorForwardVector();
	AddMovementInput(Direction, InAxis);
}

void AVeTestCharacter::Input_MoveRight(const float InAxis) {
	const auto Direction = this->GetActorRightVector();
	AddMovementInput(Direction, InAxis);
}

void AVeTestCharacter::ServerRPC_SetRunMode_Implementation() {
	if (UCharacterMovementComponent* const CharacterMovementPtr = GetCharacterMovement()) {
		CharacterMovementPtr->MaxWalkSpeed = RunMovementSpeed;
	}
}

void AVeTestCharacter::ServerRPC_SetWalkMode_Implementation() {
	if (UCharacterMovementComponent* const CharacterMovementPtr = GetCharacterMovement()) {
		CharacterMovementPtr->MaxWalkSpeed = WalkMovementSpeed;
	}
}

void AVeTestCharacter::ClientRPC_UpdateMovementSpeed_Implementation(const float InMovementSpeed) {
	if (UCharacterMovementComponent* const CharacterMovementPtr = GetCharacterMovement()) {
		CharacterMovementPtr->MaxWalkSpeed = InMovementSpeed;
	}
}

// Called every frame
void AVeTestCharacter::Tick(const float DeltaTime) {
	Super::Tick(DeltaTime);

	if (UserVRMenuWidgetComponent) {
		if (UTextureRenderTarget2D* RenderTarget = UserVRMenuWidgetComponent->GetRenderTarget()) {
			if (StereoLayer) {
				StereoLayer->SetTexture(RenderTarget);
			}
		}
	}
}

// Called to bind functionality to input
void AVeTestCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) {
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (!PlayerInputComponent) {
		LogErrorF("failed to get input component");
		return;
	}

	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("MoveForward", this, &AVeTestCharacter::Input_MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AVeTestCharacter::Input_MoveRight);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAction("Run", IE_Pressed, this, &AVeTestCharacter::ServerRPC_SetRunMode);
	PlayerInputComponent->BindAction("Run", IE_Released, this, &AVeTestCharacter::ServerRPC_SetWalkMode);
}
