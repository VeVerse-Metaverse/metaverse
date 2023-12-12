// Author: Egor A. Pristavka

#include "VeUserMetadata.h"
#include "ApiUserMetadata.h"
#include "ApiFileMetadata.h"

static const FString DefaultUserName = TEXT("Unnamed");
static const FString DefaultAvatarUrl = TEXT("https://www.gravatar.com/avatar/37c46b88275aec8658adf4bc0afa725d?d=identicon");

FVeUserMetadata::FVeUserMetadata()
	: Name(DefaultUserName),
	  AvatarUrl(DefaultAvatarUrl),
	  Level(0),
	  Rating(0),
	  bIsAdmin(false),
	  bIsMuted(false) {}

FVeUserMetadata::FVeUserMetadata(const FApiUserMetadata& InUserMetadata)
	: Id(InUserMetadata.Id),
	  Name(InUserMetadata.Name),
	  Description(InUserMetadata.Description),
	  Level(InUserMetadata.Level),
	  Rating(InUserMetadata.GetRating()),
	  bIsAdmin(InUserMetadata.bIsAdmin),
	  bIsMuted(InUserMetadata.bIsMuted) {
	if (const FApiFileMetadata* AvatarMetadata = InUserMetadata.Files.FindByPredicate([](const FApiFileMetadata& Item) {
		return Item.Type == EApiFileType::ImageAvatar;
	})) {
		AvatarUrl = AvatarMetadata->Url;
	} else {
		const FString Hash = FMD5::HashAnsiString(*InUserMetadata.Id.ToString(EGuidFormats::Digits));
		AvatarUrl = FString::Printf(TEXT("https://www.gravatar.com/avatar/%s?d=identicon"), *Hash);
	}

	DefaultPersonaMetadata = FVePersonaMetadata(InUserMetadata.DefaultPersonaMetadata);
}

FApiUserMetadata FVeUserMetadata::ToApiUserMetadata() const {
	FApiUserMetadata UserMetadata;
	UserMetadata.Id = Id;
	UserMetadata.Name = Name;
	UserMetadata.Description = Description;
	UserMetadata.Level = Level;
	UserMetadata.bIsAdmin = bIsAdmin;
	UserMetadata.bIsMuted = bIsMuted;
	return UserMetadata;
}

FVeAiUserMetadata::FVeAiUserMetadata()
	: Name(DefaultUserName) {}

FVeAiUserMetadata::FVeAiUserMetadata(const FApiUserMetadata& InUserMetadata)
	: Id(InUserMetadata.Id),
	  Name(InUserMetadata.Name),
	  Description(InUserMetadata.Description) {

	DefaultPersonaMetadata = FVePersonaMetadata(InUserMetadata.DefaultPersonaMetadata);
}
