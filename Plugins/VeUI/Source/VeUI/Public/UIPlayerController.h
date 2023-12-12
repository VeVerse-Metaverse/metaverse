// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.
#pragma once

#include "VePlayerControllerBase.h"
#include "VeWorldMetadata.h"
#include "UIPlayerController.generated.h"

class AVRHUD;

/** UI interaction mode. */
UENUM(BlueprintType)
enum class EUIInteractionMode : uint8 {
	Desktop,
	Mobile,
	VR,
	Unknown,
};

/**
 * 
 */
UCLASS()
class VEUI_API AUIPlayerController : public AVePlayerControllerBase {
	GENERATED_BODY()

public:
#pragma region UI
	/**
	 * Server->Client RPC to spawns a new instance of HUD for non-VR player controller.
	 * @remark If there was already a HUD active, it is destroyed.
	 * @remark Will not create HUD for VR mode.
	 * @remark Called from the game mode after this player controller has been spawned.
	 */
	UFUNCTION(Client, Reliable)
	void ClientRPC_InitializeHUD(TSubclassOf<AHUD> InHUDClass);

	/**
	 * Creates a HUD for the player.
	 * @remark Owning client only.
	 */
	void Owner_InitializeHUD(const TSubclassOf<AHUD>& InHUDClass);

	/**
	 * Open new level and close the menu.
	 */
	void OpenLevel(FName LevelName, const bool bAbsolute = true, const FString Options = TEXT(""));

	/** Show mouse cursor and switch to UI input mode. */
	void SwitchToUIMode();

	/** Hide mouse cursor and switch to game input mode. */
	void SwitchToGameMode();

protected:
	/**
	 * Creates a HUD for the player.
	 * @remark Owning client only.
	 */
	void Owner_SpawnUIHUD(const TSubclassOf<AHUD>& InHUDClass);

	/**
	 * Creates a VR menu for the player.
	 * @remark Owning client only.
	 */
	void Owner_SpawnVRHUD(const TSubclassOf<AVRHUD>& InHUDClass);

	FDelegateHandle OnMenuReadyDelegateHandle;

	/** 3D VR menu associated with this controller and attached to its pawn. */
	UPROPERTY(EditDefaultsOnly, Category="Metaverse")
	TSubclassOf<AVRHUD> VRHUDClass = nullptr;

	/** VR menu. */
	UPROPERTY()
	AVRHUD* MyVRHUD = nullptr;

	/**
	 * Tracks the interaction mode of the player controller.
	 * @remarks Notice that the flag is not replicated, but set separately at client and server,
	 * originating at the client. Set when the player controller starts game at the owning client
	 * and during pawn spawn RPC request at the server. 
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EUIInteractionMode InteractionMode = EUIInteractionMode::Desktop;

public:
	EUIInteractionMode GetInteractionMode() const { return InteractionMode; }

	/** Checks if the game has started in VR mode. */
	void Owner_UpdateInteractionMode();

	void Login_SetInteractionMode(const EUIInteractionMode InInteractionMode) {
		InteractionMode = InInteractionMode;
	}

	/**
	 * Show menu for the user.
	 * @remark Owning client only.
	 */
	bool Input_ShowMenu();

	/**
	 * Hide menu for the user.
	 * @remark Owning client only.
	 */
	bool Input_HideMenu();

	/**
	 * Show menu for the user and open world detail page.
	 * @remark Owning client only.
	 */
	void Owner_ShowMenuCurrentWorld(const FGuid& InWorldId);

	/**
	 * Show text chat for the user.
	 * @remark Owning client only.
	 */
	UFUNCTION(BlueprintCallable)
	bool Input_ShowTextChat();

	/**
	 * Hide text chat for the user.
	 * @remark Owning client only.
	 */
	UFUNCTION(BlueprintCallable)
	bool Input_HideTextChat();

#pragma endregion UI

#pragma region Touch
	DECLARE_EVENT_FourParams(AUIPlayerController, FOnInputTouch, uint32 /* InIndex */, ETouchType::Type /* InType */, const FVector2D& /* InLocation */, float /* InForce */);

	FOnInputTouch OnInputTouch;
	FOnInputTouch& GetOnInputTouch() { return OnInputTouch; }

	virtual bool InputTouch(uint32 Handle, ETouchType::Type Type, const FVector2D& TouchLocation, float Force, FDateTime DeviceTimestamp, uint32 TouchpadIndex) override;
#pragma endregion Touch
};
