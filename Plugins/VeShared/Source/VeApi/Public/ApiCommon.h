// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once

#include "VeEnvironmentSubsystem.h"
#include "VeConfigLibrary.h"

namespace Api {

#pragma region
	static const FString Entities = TEXT("entities");
	static const FString Users = TEXT("users");
	static const FString Me = TEXT("me");
	static const FString Feedback = TEXT("feedback");
	static const FString Invite = TEXT("invite");
	static const FString Objects = TEXT("objects");
	static const FString ArtObjects = TEXT("art-objects");
	static const FString Analytics = TEXT("analytics");
	static const FString Search = TEXT("search");
	static const FString Types = TEXT("types");
	static const FString Museums = TEXT("museums");
	static const FString Artists = TEXT("artists");
	static const FString Media = TEXT("media");
	static const FString Collections = TEXT("collections");
	static const FString Collectables = TEXT("collectables");
	static const FString Placeables = TEXT("placeables");
	static const FString PlaceableClass = TEXT("placeable_classes");
	static const FString Entity = TEXT("entity");
	static const FString Transform = TEXT("transform");
	static const FString Spaces = TEXT("spaces");
	static const FString Worlds = TEXT("worlds");
	static const FString Servers = TEXT("servers");
	static const FString GameServers = TEXT("game-servers");
	static const FString Metaverses = TEXT("mods");
	static const FString Packages = TEXT("packages");
	static const FString Maps = TEXT("maps");
	static const FString Events = TEXT("events");
	static const FString Portals = TEXT("portals");
	static const FString Login = TEXT("login");
	static const FString Authenticate = TEXT("authenticate");
	static const FString Register = TEXT("register");
	static const FString Reset = TEXT("reset");
	static const FString Heartbeat = TEXT("heartbeat");
	static const FString Match = TEXT("match");
	static const FString Files = TEXT("files");
	static const FString Properties = TEXT("properties");
	static const FString Comments = TEXT("comments");
	static const FString Likes = TEXT("likes");
	static const FString Views = TEXT("views");
	static const FString Access = TEXT("access");
	static const FString Follow = TEXT("follow");
	static const FString Follows = TEXT("follows");
	static const FString Avatars = TEXT("avatars");
	static const FString Banned = TEXT("banned");
	static const FString Muted = TEXT("muted");
	static const FString Experience = TEXT("experience");
	static const FString Followers = TEXT("followers");
	static const FString Leaders = TEXT("leaders");
	static const FString Friends = TEXT("friends");
	static const FString Personas = TEXT("personas");
	static const FString Invitations = TEXT("invitations");
	static const FString Password = TEXT("password");
	static const FString Default = TEXT("default");
	static const FString Upload = TEXT("upload");
	static const FString Link = TEXT("link");

#pragma endregion

#if UE_EDITOR
	/** Editor API key is used for server and generic API and websocket requests during development, must not be included in production builds. */
	const FString EditorKey = TEXT("d3f0e9ff80214be0aeb5c8746570a186");
#endif

#if PLATFORM_ANDROID
    const FString OculusKey = TEXT("326bbad864ae4ec4bbde7dd4e9c2deca");
#endif

	inline FString GetApiRootUrl() {
		return FVeConfigLibrary::GetApiUrl();
	}

	inline FString GetApi2RootUrl() {
		return FVeConfigLibrary::GetApi2Url();
	}

	inline FString GetW3LoginUrl() {
		return GetApiRootUrl() / TEXT("w3/login");
	}

	inline FString GetW3MintLandUrl() {
		return GetApiRootUrl() / TEXT("w3/mint_land");
	}

	inline FString GetEntitiesUrl() {
		return GetApiRootUrl() / Entities;
	}

	inline FString GetEntitiesUrl2() {
		return GetApi2RootUrl() / Entities;
	}

	inline FString GetArtObjectUrl() {
		return GetApi2RootUrl() / ArtObjects;
	}

	inline FString GetAnalyticsUrl() {
		return GetApi2RootUrl() / Analytics;
	}

	inline FString GetWorldsUrl() {
		return GetApi2RootUrl() / Worlds;
	}

#pragma region Entities

	inline FString MakeEntityUrl(const FString EntityId) {
		return GetEntitiesUrl() / EntityId;
	}

	inline FString MakeEntityCommentsUrl(const FString EntityId) {
		return MakeEntityUrl(EntityId) / Comments;
	}

	inline FString MakeEntityFilesUrl(const FString EntityId) {
		return MakeEntityUrl(EntityId) / Files;
	}

	inline FString MakeEntityPropertiesUrl(const FString EntityId) {
		return MakeEntityUrl(EntityId) / Properties;
	}

	inline FString MakeEntityLikesUrl(const FString EntityId) {
		return MakeEntityUrl(EntityId) / Likes;
	}

	inline FString MakeEntityViewUrl(const FString EntityId) {
		return MakeEntityUrl(EntityId) / Views;
	}

	inline FString MakeEntityAccessUrl(const FString EntityId) {
		return MakeEntityUrl(EntityId) / Access;
	}

	inline FString MakeEntityCommentUrl(const FString EntityId, const FString CommentId) {
		return MakeEntityCommentsUrl(EntityId) / CommentId;
	}

	inline FString MakeEntityFileUrl(const FString EntityId, const FString FileId) {
		return MakeEntityFilesUrl(EntityId) / FileId;
	}

	inline FString MakeEntityPropertyUrl(const FString EntityId, const FString Name) {
		return MakeEntityPropertiesUrl(EntityId) / Name;
	}

	inline FString MakeEntityCommentsIndexUrl(const FString EntityId, const int32 Offset = 0, const int32 Limit = 12) {
		return MakeEntityCommentsUrl(EntityId) + FString::Printf(TEXT("?offset=%d&limit=%d"), Offset, Limit);
	}

	inline FString MakeEntityPropertiesIndexUrl(const FString EntityId, const int32 Offset = 0, const int32 Limit = 12) {
		return MakeEntityPropertiesUrl(EntityId) + FString::Printf(TEXT("?offset=%d&limit=%d"), Offset, Limit);
	}

	inline FString MakeEntityFilesIndexUrl(const FString EntityId, const int32 Offset = 0, const int32 Limit = 12) {
		return MakeEntityFilesUrl(EntityId) + FString::Printf(TEXT("?offset=%d&limit=%d"), Offset, Limit);
	}

	inline FString MakeEntityLikesIndexUrl(const FString EntityId, const int32 Offset = 0, const int32 Limit = 12) {
		return MakeEntityLikesUrl(EntityId) + FString::Printf(TEXT("?offset=%d&limit=%d"), Offset, Limit);
	}

	inline FString MakeEntityLikeUrl(const FString EntityId, const int32 Rating = 1) {
		return MakeEntityLikesUrl(EntityId) + FString::Printf(TEXT("?rating=%d"), Rating);
	}

#pragma endregion

#pragma region Users
	inline FString GetUsersUrl() {
		return GetApiRootUrl() / Users;
	}

	inline FString GetUsersUrl2() {
		return GetApi2RootUrl() / Users;
	}

	inline FString GetPersonaUrl2() {
		return GetUsersUrl2() / Personas;
	}

	inline FString GetUserMeUrl() {
		return GetUsersUrl() / Me;
	}

	inline FString GetUserMeUrl2() {
		return GetUsersUrl2() / Me;
	}

	inline FString GetUserMePasswordUrl() {
		return GetUserMeUrl() / Password;
	}

	inline FString GetUserMeHeartbeatUrl() {
		return GetUserMeUrl() / Heartbeat;
	}

	inline FString GetUserFeedbackUrl() {
		return GetUsersUrl() / Feedback;
	}

	inline FString GetUserInviteUrl() {
		return GetUsersUrl() / Invite;
	}

	inline FString GetUsersBannedUrl() {
		return GetUsersUrl() / Banned;
	}

	inline FString GetUsersMutedUrl() {
		return GetUsersUrl() / Muted;
	}

	inline FString GetUserMeInvitationsUrl() {
		return GetUserMeUrl() / Invitations;
	}

	inline FString GetUserMeAvatarsUrl() {
		return GetUserMeUrl() / Avatars;
	}

	inline FString GetUsersPasswordResetUrl() {
		return GetUsersUrl() / Password / Reset;
	}

	inline FString GetUsersPasswordResetUrl2() {
		return GetUsersUrl2() / Password / Reset;
	}

	inline FString MakeUserUrl(const FString& UserId) {
		return GetUsersUrl() / UserId;
	}

	inline FString MakeUserInviteUrl(const FString& Email) {
		return GetUserInviteUrl() + TEXT("?email=") + Email;
	}

	inline FString MakeUserAccessUrl(const FString& UserId) {
		return GetUsersUrl() / UserId / Access;
	}

	inline FString MakeUserExperienceUrl(const FString& UserId) {
		return GetUsersUrl() / UserId / Experience;
	}

	inline FString MakeUserMuteUrl(const FString& UserId, const bool bMute) {
		return MakeUserUrl(UserId) / TEXT("mute?mute=") + (bMute ? TEXT("true") : TEXT("false"));
	}

	inline FString MakeUserBanUrl(const FString& UserId, const bool bMute) {
		return MakeUserUrl(UserId) / TEXT("ban?ban=") + (bMute ? TEXT("true") : TEXT("false"));
	}

	inline FString MakeUsersBannedIndexUrl(const int32 Offset = 0, const int32 Limit = 12) {
		return GetUsersBannedUrl() + FString::Printf(TEXT("?offset=%d&limit=%d"), Offset, Limit);
	}

	inline FString MakeUsersMutedIndexUrl(const int32 Offset = 0, const int32 Limit = 12) {
		return GetUsersMutedUrl() + FString::Printf(TEXT("?offset=%d&limit=%d"), Offset, Limit);
	}

	inline FString MakeUserAvatarsUrl(const FString& UserId) {
		return MakeUserUrl(UserId) / Avatars;
	}

	inline FString MakeUserAvatarsIndexUrl(const FString& UserId, const int32 Offset = 0, const int32 Limit = 12) {
		return MakeUserAvatarsUrl(UserId) + FString::Printf(TEXT("?offset=%d&limit=%d"), Offset, Limit);
	}

	inline FString MakeUserCollectionsUrl(const FString& UserId) {
		return MakeUserUrl(UserId) / Collections;
	}

	inline FString MakeUserCollectionsIndexUrl(const FString& UserId, const int32 Offset = 0, const int32 Limit = 12) {
		return MakeUserCollectionsUrl(UserId) + FString::Printf(TEXT("?offset=%d&limit=%d"), Offset, Limit);
	}

	inline FString MakeUserObjectsUrl(const FString& UserId) {
		return MakeUserUrl(UserId) / Objects;
	}

	inline FString MakeUserObjectsIndexUrl(const FString& UserId, const int32 Offset = 0, const int32 Limit = 12) {
		return MakeUserObjectsUrl(UserId) + FString::Printf(TEXT("?offset=%d&limit=%d"), Offset, Limit);
	}

	inline FString MakeUserSpacesUrl(const FString& UserId) {
		return MakeUserUrl(UserId) / Spaces;
	}

	inline FString MakeUserSpacesIndexUrl(const FString& UserId, const int32 Offset = 0, const int32 Limit = 12) {
		return MakeUserSpacesUrl(UserId) + FString::Printf(TEXT("?offset=%d&limit=%d"), Offset, Limit);
	}

	inline FString MakeUserActivateUrl(const FString& UserId, const bool bActivate) {
		return MakeUserUrl(UserId) / TEXT("activate?activate=") + (bActivate ? TEXT("true") : TEXT("false"));
	}

	inline FString MakeUserFollowUrl(const FString& UserId) {
		return MakeUserUrl(UserId) / Follow;
	}

	inline FString MakeUserFollowsUrl(const FString& UserId, const FString& TargetId) {
		return MakeUserUrl(UserId) / Follows / TargetId;
	}

	inline FString MakeUserFollowersUrl(const FString& UserId) {
		return MakeUserUrl(UserId) / Followers;
	}

	inline FString MakeUserFollowersIndexUrl(const FString& UserId, const int32 Offset = 0, const int32 Limit = 12) {
		return MakeUserFollowersUrl(UserId) + FString::Printf(TEXT("?offset=%d&limit=%d"), Offset, Limit);
	}

	inline FString MakeUserLeadersUrl(const FString& UserId) {
		return MakeUserUrl(UserId) / Leaders;
	}

	inline FString MakeUserLeadersIndexUrl(const FString& UserId, const int32 Offset = 0, const int32 Limit = 12) {
		return MakeUserLeadersUrl(UserId) + FString::Printf(TEXT("?offset=%d&limit=%d"), Offset, Limit);
	}

	inline FString MakeUserFriendsUrl(const FString& UserId) {
		return MakeUserUrl(UserId) / Friends;
	}

	inline FString MakeUserFriendsIndexUrl(const FString& UserId, const int32 Offset = 0, const int32 Limit = 12) {
		return MakeUserFriendsUrl(UserId) + FString::Printf(TEXT("?offset=%d&limit=%d"), Offset, Limit);
	}

	inline FString MakeUserMeAvatarUrl(const FString& AvatarId) {
		return GetUserMeAvatarsUrl() + TEXT("?avatar_id=") + AvatarId;
	}

	inline FString MakeUsersIndexUrl(const int32 Offset = 0, const int32 Limit = 12, const FString& Query = "") {
		return GetUsersUrl() + FString::Printf(TEXT("?offset=%d&limit=%d&query=%s"), Offset, Limit, *Query);
	}

#pragma endregion

#pragma region Objects
	inline FString GetObjectsUrl() {
		return GetApiRootUrl() / Objects;
	}

	inline FString GetObjectSearchUrl() {
		return GetObjectsUrl() / Search;
	}

	inline FString GetObjectTypesUrl() {
		return GetObjectsUrl() / Types;
	}

	inline FString GetObjectMuseumsUrl() {
		return GetObjectsUrl() / Museums;
	}

	inline FString GetObjectArtistsUrl() {
		return GetObjectsUrl() / Artists;
	}

	inline FString GetObjectMediaUrl() {
		return GetObjectsUrl() / Media;
	}

	inline FString MakeObjectsIndexUrl(const int32 Offset = 0, const int32 Limit = 12, const FString& Query = "") {
		return GetObjectsUrl() + FString::Printf(TEXT("?offset=%d&limit=%d&query=%s"), Offset, Limit, *Query);
	}

	inline FString MakeObjectUrl(const FString EntityId) {
		return GetObjectsUrl() / EntityId;
	}

	inline FString MakeObjectArtistsIndexUrl(const int32 Offset = 0, const int32 Limit = 12, const FString& Query = "") {
		return GetObjectArtistsUrl() + FString::Printf(TEXT("?offset=%d&limit=%d&query=%s"), Offset, Limit, *Query);
	}

	inline FString MakeObjectMuseumsIndexUrl(const int32 Offset = 0, const int32 Limit = 12, const FString& Query = "") {
		return GetObjectMuseumsUrl() + FString::Printf(TEXT("?offset=%d&limit=%d&query=%s"), Offset, Limit, *Query);
	}

	inline FString MakeObjectMediaIndexUrl(const int32 Offset = 0, const int32 Limit = 12, const FString& Query = "") {
		return GetObjectMediaUrl() + FString::Printf(TEXT("?offset=%d&limit=%d&query=%s"), Offset, Limit, *Query);
	}

	inline FString MakeObjectTypesIndexUrl(const int32 Offset = 0, const int32 Limit = 12, const FString& Query = "") {
		return GetObjectTypesUrl() + FString::Printf(TEXT("?offset=%d&limit=%d&query=%s"), Offset, Limit, *Query);
	}
#pragma endregion

#pragma region Collections
	inline FString GetCollectionsUrl() {
		return GetApiRootUrl() / Collections;
	}

	// Url to index the single collection collectables.
	inline FString MakeCollectionsIndexUrl(const int32 Offset = 0, const int32 Limit = 12, const FString& Query = "") {
		return GetCollectionsUrl() + FString::Printf(TEXT("?offset=%d&limit=%d&query=%s"), Offset, Limit, *Query);
	}

	// Url to refer to single collection by its id.
	inline FString MakeCollectionUrl(const FString CollectionId) {
		return GetCollectionsUrl() / CollectionId;
	}

	// Url to refer to the single collection collectables.
	inline FString MakeCollectionCollectablesUrl(const FString CollectionId) {
		return GetCollectionsUrl() / CollectionId / Collectables;
	}

	// Url to index the single collection collectables.
	inline FString MakeCollectionCollectablesIndexUrl(const FString CollectionId, const int32 Offset = 0, const int32 Limit = 12) {
		return MakeCollectionCollectablesUrl(CollectionId) + FString::Printf(TEXT("?offset=%d&limit=%d"), Offset, Limit);
	}

	// Url to refer to the single collection collectable by its id.
	inline FString MakeCollectionCollectableUrl(const FString CollectionId, const FString ObjectId) {
		return GetCollectionsUrl() / CollectionId / Collectables / ObjectId;
	}
#pragma endregion

#pragma region Spaces
	inline FString GetSpacesUrl() {
		return GetApiRootUrl() / Spaces;
	}

	inline FString GetPlaceableClassesUrl() {
		return GetApiRootUrl() / PlaceableClass;
	}

	inline FString GetPlaceableClassesUrl2() {
		return GetApi2RootUrl() / PlaceableClass;
	}

	// Url to index the single collection collectables.
	inline FString MakeSpacesIndexUrl(const int32 Offset = 0, const int32 Limit = 12, const FString& Query = "") {
		return GetSpacesUrl() + FString::Printf(TEXT("?offset=%d&limit=%d&query=%s"), Offset, Limit, *Query);
	}

	// Url to refer to single collection by its id.
	inline FString MakeSpaceUrl(const FString SpaceId) {
		return GetSpacesUrl() / SpaceId;
	}

	// Url to refer to the single collection collectables.
	inline FString MakeSpacePlaceablesUrl(const FString SpaceId) {
		return GetSpacesUrl() / SpaceId / Placeables;
	}

	// Url to index the single collection collectables.
	inline FString MakeSpacePlaceablesIndexUrl(const FString SpaceId, const int32 Offset = 0, const int32 Limit = 12) {
		return MakeSpacePlaceablesUrl(SpaceId) + FString::Printf(TEXT("?offset=%d&limit=%d"), Offset, Limit);
	}

	// Url to refer to the single collection collectable by its id.
	inline FString MakeSpacePlaceableUrl(const FString SpaceId, const FString PlaceableId) {
		return GetSpacesUrl() / SpaceId / Placeables / PlaceableId;
	}

	inline FString GetWorldUrl2() {
		return GetApi2RootUrl() / Worlds;
	}

#pragma endregion

	inline FString GetLoginUrl() {
		return GetApiRootUrl() / Login;
	}

#pragma region Online
	inline FString GetServersUrl() {
		return GetApiRootUrl() / Servers;
	}

	inline FString GetGameServersUrl2() {
		return GetApi2RootUrl() / GameServers;
	}

	inline FString GetServerAuthenticateUrl() {
		return GetServersUrl() / Authenticate;
	}

	inline FString GetServerRegisterUrl() {
		return GetServersUrl() / Register;
	}

	inline FString GetServerHeartbeatUrl() {
		return GetServersUrl() / Heartbeat;
	}

	inline FString GetServerMatchUrl() {
		return GetServersUrl() / Match;
	}

	inline FString MakeOnlineGameMatchUrl(const FString& SpaceId) {
		return GetServerMatchUrl() / SpaceId;
	}

	inline FString MakeOnlineGameHeartbeatUrl(const FString& ServerId) {
		return GetServerHeartbeatUrl() / ServerId;
	}
#pragma endregion

#pragma region
	inline FString GetMetaversesUrl() {
		return GetApiRootUrl() / Metaverses;
	}

	inline FString GetPackagesUrl2() {
		return GetApi2RootUrl() / Packages;
	}
#pragma endregion

#pragma region
	inline FString GetEventsUrl() {
		return GetApiRootUrl() / Events;
	}
#pragma endregion

#pragma region
	inline FString GetPortalsUrl() {
		return GetApiRootUrl() / Portals;
	}
#pragma endregion

	namespace DeploymentType {
		static constexpr auto Client = TEXT("Client");
		static constexpr auto Server = TEXT("Server");
	}

	namespace AnalyticsType {
		static constexpr auto AppId = TEXT("appId");
		static constexpr auto ContextEntityId = TEXT("contextEntityId");
		static constexpr auto ContextEntityType = TEXT("contextEntityType");
		static constexpr auto Event = TEXT("event");
	}

	namespace HttpMethods {
		static constexpr auto Get = TEXT("GET");
		static constexpr auto Post = TEXT("POST");
		static constexpr auto Put = TEXT("PUT");
		static constexpr auto Patch = TEXT("PATCH");
		static constexpr auto Delete = TEXT("DELETE");
	}

	namespace Headers {
		static constexpr auto RequestId = TEXT("id");
		static constexpr auto MetaverseId = TEXT("ve-meta-id");
		static constexpr auto MetaverseKey = TEXT("ve-meta-key");
		static constexpr auto Key = TEXT("key");
		static constexpr auto ContentType = TEXT("Content-Type");
		static constexpr auto Accept = TEXT("Accept");
	}

	namespace ContentTypes {
		static constexpr auto ApplicationJson = TEXT("application/json");
		static constexpr auto BinaryOctetStream = TEXT("binary/octet-stream");
		static constexpr auto MultipartFormData = TEXT("multipart/form-data");
		static constexpr auto ImageJpeg = TEXT("image/jpeg");
		static constexpr auto ImagePng = TEXT("image/png");
		static constexpr auto ImageExr = TEXT("image/x-exr");
		static constexpr auto Binary = TEXT("binary/octet-stream");
		static constexpr auto UnrealBlueprint = TEXT("application/x-unreal-blueprint");
	}

	namespace EntityTypes {
		static constexpr auto Entity = TEXT("entity");
		static constexpr auto Object = TEXT("object");
		static constexpr auto Collection = TEXT("collection");
		static constexpr auto Space = TEXT("space");
		static constexpr auto User = TEXT("user");
	}

	namespace Presence {
		static constexpr auto Offline = TEXT("offline");
		static constexpr auto Away = TEXT("away");
		static constexpr auto Available = TEXT("available");
		static constexpr auto Playing = TEXT("playing");
	}

	namespace FileTypes {
		static constexpr auto Unknown = TEXT("unknown");
		static constexpr auto MeshAvatar = TEXT("mesh_avatar");
		static constexpr auto Model = TEXT("model");
		static constexpr auto Video = TEXT("video");
		static constexpr auto ImagePreview = TEXT("image_preview");
		static constexpr auto ImageFull = TEXT("image_full");
		static constexpr auto ImageFullInitial = TEXT("image_full_initial");
		static constexpr auto Pak = TEXT("pak");
		static constexpr auto Pdf = TEXT("pdf");
		static constexpr auto PdfImage = TEXT("pdf_image");
		static constexpr auto PluginDescriptor = TEXT("uplugin");
		static constexpr auto PluginContent = TEXT("uplugin_content");
		static constexpr auto CharacterCustomizerSaveGame = TEXT("character_customizer_save_game");
		static constexpr auto ImageAvatar = TEXT("image_avatar");
		static constexpr auto TextureDiffuse = TEXT("texture_diffuse");
		static constexpr auto TextureNormal = TEXT("texture_normal");
		static constexpr auto Audio = TEXT("audio");
		static constexpr auto Cubemap = TEXT("cubemap");
		static constexpr auto RenderTargetPreview = TEXT("rendertarget_preview");
		static constexpr auto PakExtraContent = TEXT("pak-extra-content");
		static constexpr auto AppLogoImage = TEXT("image-app-logo");
		static constexpr auto AppPlaceholderImage = TEXT("image-app-placeholder");
	}

	namespace LinkTypes {
		static constexpr auto Unexplored = TEXT("unexplored");
		static constexpr auto Website = TEXT("website");
		static constexpr auto Facebook = TEXT("facebook");
		static constexpr auto Twitter = TEXT("twitter");
		static constexpr auto Youtube = TEXT("youtube");
		static constexpr auto Discord = TEXT("discord");
	}

	namespace PlaceableTypes {
		static constexpr auto Image2D = TEXT("image_2d");
		static constexpr auto Mesh3D = TEXT("mesh_3d");
		static constexpr auto Video2D = TEXT("video_2d");
		static constexpr auto Audio = TEXT("audio");
		static constexpr auto Pdf = TEXT("pdf");
	}

	namespace ServerStatus {
		static constexpr auto Online = TEXT("online");
		static constexpr auto Error = TEXT("error");
	}

	namespace Template {
		static constexpr auto TemplatesId = TEXT("templatesId");
		static constexpr auto Templates = TEXT("templates");
		static constexpr auto Error = TEXT("error");

		inline FString GetTemplatesUrl() {
			return GetApiRootUrl() / Templates;
		}
	}

	namespace Fields {
		static constexpr auto Payload = TEXT("data");
		static constexpr auto Entities = TEXT("entities");
		static constexpr auto Offset = TEXT("offset");
		static constexpr auto Limit = TEXT("limit");
		static constexpr auto Count = TEXT("count");
		static constexpr auto Total = TEXT("total");
		static constexpr auto Id = TEXT("id");
		static constexpr auto Class = TEXT("class");
		static constexpr auto Category = TEXT("category");
		static constexpr auto EntityType = TEXT("entityType");
		static constexpr auto DeploymentType = TEXT("deploymentType");
		static constexpr auto Configuration = TEXT("configuration");
		static constexpr auto EntityId = TEXT("entityId");
		static constexpr auto TargetId = TEXT("targetId");
		static constexpr auto Public = TEXT("public");
		static constexpr auto CreatedAt = TEXT("createdAt");
		static constexpr auto UpdatedAt = TEXT("updatedAt");
		static constexpr auto Timestamp = TEXT("timestamp");
		static constexpr auto ReleasedAt = TEXT("releasedAt");
		static constexpr auto StartsAt = TEXT("startsAt");
		static constexpr auto EndsAt = TEXT("endsAt");
		static constexpr auto Views = TEXT("views");
		static constexpr auto Likables = TEXT("likables");
		static constexpr auto Comments = TEXT("comments");
		static constexpr auto Accessibles = TEXT("accessibles");
		static constexpr auto File = TEXT("file");
		static constexpr auto Files = TEXT("files");
		static constexpr auto Properties = TEXT("properties");
		static constexpr auto Fields = TEXT("fields");
		static constexpr auto Type = TEXT("type");
		static constexpr auto LinkType = TEXT("linkType");
		static constexpr auto Title = TEXT("title");
		static constexpr auto Artist = TEXT("artist");
		static constexpr auto Date = TEXT("date");
		static constexpr auto Summary = TEXT("summary");
		static constexpr auto Description = TEXT("description");
		static constexpr auto Geolocation = TEXT("geolocation");
		static constexpr auto Medium = TEXT("medium");
		static constexpr auto Width = TEXT("width");
		static constexpr auto Height = TEXT("height");
		static constexpr auto ScaleMultiplier = TEXT("scaleMultiplier");
		static constexpr auto Dimensions = TEXT("dimensions");
		static constexpr auto Source = TEXT("source");
		static constexpr auto SourceId = TEXT("sourceId");
		static constexpr auto SourceUrl = TEXT("sourceUrl");
		static constexpr auto License = TEXT("license");
		static constexpr auto Copyright = TEXT("copyright");
		static constexpr auto Credit = TEXT("credit");
		static constexpr auto Origin = TEXT("origin");
		static constexpr auto Location = TEXT("location");
		static constexpr auto UserId = TEXT("userId");
		static constexpr auto User = TEXT("user");
		static constexpr auto Name = TEXT("name");
		static constexpr auto Domain = TEXT("domain");
		static constexpr auto Value = TEXT("value");
		static constexpr auto AvatarUrl = TEXT("avatarUrl");
		static constexpr auto Liked = TEXT("liked");
		static constexpr auto Text = TEXT("text");
		static constexpr auto Engine = TEXT("engine");
		static constexpr auto LanguageCode = TEXT("languageCode");
		static constexpr auto OutputFormat = TEXT("outputFormat");
		static constexpr auto SampleRate = TEXT("sampleRate");
		static constexpr auto TextType = TEXT("textType");
		static constexpr auto VoiceId = TEXT("voiceId");
		static constexpr auto CanView = TEXT("canView");
		static constexpr auto CanEdit = TEXT("canEdit");
		static constexpr auto CanDelete = TEXT("canDelete");
		static constexpr auto IsOwner = TEXT("isOwner");
		static constexpr auto Url = TEXT("url");
		static constexpr auto Mime = TEXT("mime");
		static constexpr auto Objects = TEXT("objects");
		static constexpr auto Subjects = TEXT("subjects");
		static constexpr auto SlotId = TEXT("slotId");
		static constexpr auto ObjectId = TEXT("objectId");
		static constexpr auto Object = TEXT("object");
		static constexpr auto AiFsmVersion = TEXT("v");
		static constexpr auto AiFsmStateNum = TEXT("n");
		static constexpr auto AiFsmContext = TEXT("c");
		static constexpr auto AiFsmAction = TEXT("a");
		static constexpr auto AiFsmNPC = TEXT("c");
		static constexpr auto AiFsmName = TEXT("n");
		static constexpr auto AiFsmDescription = TEXT("d");
		static constexpr auto AiFsmTarget = TEXT("t");
		static constexpr auto AiFsmMetadata = TEXT("m");
		static constexpr auto AiFsmEmotion = TEXT("e");
		static constexpr auto AiFsmObject = TEXT("o");
		static constexpr auto AiFsmNpcs = TEXT("n");
		static constexpr auto AiFsmPlayers = TEXT("p");
		static constexpr auto AiFsmObjects = TEXT("o");
		static constexpr auto AiFsmLocations = TEXT("l");
		static constexpr auto AiFsmLocationLinks = TEXT("l");
		static constexpr auto AiFsmLocationEntities = TEXT("e");
		static constexpr auto SpaceId = TEXT("spaceId");
		static constexpr auto OffsetX = TEXT("pX");
		static constexpr auto OffsetY = TEXT("pY");
		static constexpr auto OffsetZ = TEXT("pZ");
		static constexpr auto RotationX = TEXT("rX");
		static constexpr auto RotationY = TEXT("rY");
		static constexpr auto RotationZ = TEXT("rZ");
		static constexpr auto ScaleX = TEXT("sX");
		static constexpr auto ScaleY = TEXT("sY");
		static constexpr auto ScaleZ = TEXT("sZ");
		static constexpr auto Placeables = TEXT("placeables");
		static constexpr auto PlaceableClass = TEXT("placeableClass");
		static constexpr auto PlaceableClassId = TEXT("placeableClassId");
		static constexpr auto Map = TEXT("map");
		static constexpr auto Environment = TEXT("environment");
		static constexpr auto Lines = TEXT("lines");
		static constexpr auto Email = TEXT("email");
		static constexpr auto Password = TEXT("password");
		static constexpr auto PasswordConfirmation = TEXT("passwordConfirmation");
		static constexpr auto DeviceId = TEXT("deviceId");
		static constexpr auto InviteCode = TEXT("inviteCode");
		static constexpr auto ApiKey = TEXT("apiKey");
		static constexpr auto EthAddress = TEXT("ethAddress");
		static constexpr auto Active = TEXT("active");
		static constexpr auto Address = TEXT("address");
		static constexpr auto Host = TEXT("host");
		static constexpr auto Port = TEXT("port");
		static constexpr auto SessionId = TEXT("sessionId");
		static constexpr auto MaxPlayers = TEXT("maxPlayers");
		static constexpr auto GameMode = TEXT("gameMode");
		static constexpr auto Build = TEXT("build");
		static constexpr auto Query = TEXT("query");
		static constexpr auto IsActive = TEXT("isActive");
		static constexpr auto IsAdmin = TEXT("isAdmin");
		static constexpr auto IsMuted = TEXT("isMuted");
		static constexpr auto IsBanned = TEXT("isBanned");
		static constexpr auto Owner = TEXT("owner");
		static constexpr auto Entity = TEXT("entity");
		static constexpr auto Error = TEXT("error");
		static constexpr auto Assets = TEXT("assets");
		static constexpr auto Avatar = TEXT("avatar");
		static constexpr auto Ok = TEXT("ok");
		static constexpr auto Detail = TEXT("detail");
		static constexpr auto Level = TEXT("level");
		static constexpr auto Rank = TEXT("rank");
		static constexpr auto Experience = TEXT("experience");
		static constexpr auto Used = TEXT("used");
		static constexpr auto Unused = TEXT("unused");
		static constexpr auto NewPassword = TEXT("newPassword");
		static constexpr auto NewPasswordConfirmation = TEXT("newPasswordConfirmation");
		static constexpr auto Size = TEXT("size");
		static constexpr auto AllowEmails = TEXT("allowEmails");
		static constexpr auto OnlinePlayers = TEXT("onlinePlayers");
		static constexpr auto Version = TEXT("version");
		static constexpr auto Variation = TEXT("variation");
		static constexpr auto Platform = TEXT("platform");
		static constexpr auto Deployment = TEXT("deployment");
		static constexpr auto ReleaseName = TEXT("releaseName");
		static constexpr auto Tags = TEXT("tags");
		static constexpr auto Platforms = TEXT("platforms");
		static constexpr auto Downloads = TEXT("downloads");
		static constexpr auto Likes = TEXT("likes");
		static constexpr auto Dislikes = TEXT("dislikes");
		static constexpr auto TotalLikes = TEXT("totalLikes");
		static constexpr auto TotalDislikes = TEXT("totalDislikes");
		static constexpr auto Price = TEXT("price");
		static constexpr auto Discount = TEXT("discount");
		static constexpr auto Purchased = TEXT("purchased");
		static constexpr auto DownloadSize = TEXT("downloadSize");
		static constexpr auto UploadedBy = TEXT("uploadedBy");
		static constexpr auto Rating = TEXT("rating");
		static constexpr auto Sort = TEXT("sort");
		static constexpr auto MetaverseId = TEXT("modId");
		static constexpr auto Metaverse = TEXT("metaverse");
		static constexpr auto Mod = TEXT("mod");
		static constexpr auto Space = TEXT("space");
		static constexpr auto Spaces = TEXT("spaces");
		static constexpr auto Server = TEXT("server");
		static constexpr auto Status = TEXT("status");
		static constexpr auto StatusMessage = TEXT("statusMessage");
		static constexpr auto Details = TEXT("details");
		static constexpr auto DestinationId = TEXT("destinationId");
		static constexpr auto Destination = TEXT("destination");
		static constexpr auto Image = TEXT("image");
		static constexpr auto IncludeFriends = TEXT("include_friends");
		static constexpr auto Personas = TEXT("personas");
		static constexpr auto DefaultPersona = TEXT("defaultPersona");
		static constexpr auto Presence = TEXT("presence");
		static constexpr auto PersonaId = TEXT("personaId");
		static constexpr auto RPM = TEXT("RPM");
		static constexpr auto CharacterCustomizer = TEXT("CharacterCustomizer");
		static constexpr auto OriginalPath = TEXT("originalPath");
		static constexpr auto Hash = TEXT("hash");
		static constexpr auto PlayerId = TEXT("playerId");
		static constexpr auto RegionId = TEXT("regionId");
		static constexpr auto ReleaseId = TEXT("releaseId");
		static constexpr auto WorldId = TEXT("worldId");
		static constexpr auto GameModeId = TEXT("gameModeId");
		static constexpr auto OnlinePlayerIds = TEXT("onlinePlayerIds");
		static constexpr auto Search = TEXT("search");
		static constexpr auto Options = TEXT("options");
		static constexpr auto Column = TEXT("column");
		static constexpr auto Direction = TEXT("direction");
		static constexpr auto Pak = TEXT("pak");
		static constexpr auto PakOptions = TEXT("pakOptions");
		static constexpr auto Preview = TEXT("preview");
	}}
