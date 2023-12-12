// Author: Egor A. Pristavka. 
// Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#pragma once

/** Portal state. */
UENUM(BlueprintType)
enum class EVePortalState : uint8 {
	/** Portal is disabled and can not be used. The portal has no target or disabled manually. */
	Disabled,
	/** Portal target is offline due to error and can not teleport player to the new place. Tried to establish connection to the target portal, but failed. */
	Error,
	/** Portal target is offline because no pak available for download. */
	NoPak,
	/** Portal target is offline and can not teleport player to the new place. */
	Offline,
	/** Portal target is online and can teleport player to the new place. Successfully established connection to the target portal. */
	Online,
	/** Portal is busy with asynchronous operation. */
	Loading,
	/** Portal is waiting for the server to start. */
	WaitServer,
	/** Portal is waiting for user confirmation. */
	ConfirmDownload,
	/** Portal is busy with downloading mod content. */
	Downloading,
	/** Local portal linked to other portals in the current world. */
	Local,
	/** Portal in editor/sdk mode can not download PAKs. */
	Editor,
};
