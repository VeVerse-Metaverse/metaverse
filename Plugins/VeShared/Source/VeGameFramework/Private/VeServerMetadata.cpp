// Author: Egor A. Pristavka

#include "VeServerMetadata.h"

#include "ApiServerMetadata.h"
#include "ApiSpaceMetadata.h"

FVeServerMetadata::FVeServerMetadata() {}

FVeServerMetadata::FVeServerMetadata(const FApiServerMetadata& InMetadata) {
	Id = InMetadata.Id;
	bPublic = InMetadata.bPublic;
	Host = InMetadata.Host;
	Map = InMetadata.Map;
	Port = InMetadata.Port;
	GameMode = InMetadata.GameMode;
	MaxPlayers = InMetadata.MaxPlayers;
	OnlinePlayers = InMetadata.OnlinePlayers;
	SpaceId = InMetadata.SpaceId;
	Status = ApiServerStatusToString(InMetadata.Status);

	UserId = InMetadata.UserId;
	CreatedAt = InMetadata.CreatedAt;
	UpdatedAt = InMetadata.UpdatedAt;
}

FVeServerMetadata::operator FApiUpdateServerMetadata() const {
	FApiUpdateServerMetadata Metadata;

	Metadata.Id = Id;
	Metadata.bPublic = bPublic;
	Metadata.Host = Host;
	Metadata.Map = Map;
	Metadata.Port = Port;
	Metadata.GameMode = GameMode;
	Metadata.MaxPlayers = MaxPlayers;
	Metadata.OnlinePlayers = OnlinePlayers;
	Metadata.SpaceId = SpaceId;
	Metadata.Status = Status;

	return Metadata;
}

FApiUpdateServerMetadata FVeServerMetadata::ToUpdateServerMetadata(const FVeServerMetadata& InServerMetadata) {
	FApiUpdateServerMetadata Metadata;
	Metadata.Id = InServerMetadata.Id;
	Metadata.bPublic = InServerMetadata.bPublic;
	Metadata.Host = InServerMetadata.Host;
	Metadata.Map = InServerMetadata.Map;
	Metadata.Port = InServerMetadata.Port;
	Metadata.GameMode = InServerMetadata.GameMode;
	Metadata.MaxPlayers = InServerMetadata.MaxPlayers;
	Metadata.OnlinePlayers = InServerMetadata.OnlinePlayers;
	Metadata.SpaceId = InServerMetadata.SpaceId;
	Metadata.Status = InServerMetadata.Status;
	return Metadata;
}
