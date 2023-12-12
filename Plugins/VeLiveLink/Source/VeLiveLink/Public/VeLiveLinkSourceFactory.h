// Author: Nikolay Bulatov, Egor A. Pristavka. 
// Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#pragma once

#include "CoreMinimal.h"
#include "LiveLinkSourceFactory.h"
//#include "VeLiveLinkSourceFactory.generated.h"

struct FVeLiveLinkConnectionSettings;
class FVeLiveLinkSource;

#if 1
class UVeLiveLinkSourceFactory {
public:
	static TSharedPtr<FVeLiveLinkSource> CreateLiveLinkSource();

// private:
// 	static TSharedPtr<FVeLiveLinkSource> Source;
};

#else

UCLASS()
class VELIVELINK_API UVeLiveLinkSourceFactory : public ULiveLinkSourceFactory {
	GENERATED_BODY()

public:
	DECLARE_DELEGATE_RetVal_TwoParams(TSharedPtr<SWidget>, FBuildCreationPanelDelegate, const UVeLiveLinkSourceFactory*, FOnLiveLinkSourceCreated);
	static FBuildCreationPanelDelegate OnBuildCreationPanel;

	//~ Begin ULiveLinkSourceFactory interface
	virtual FText GetSourceDisplayName() const override;
	virtual FText GetSourceTooltip() const override;

	virtual EMenuType GetMenuType() const override { return EMenuType::SubPanel; }
	virtual TSharedPtr<SWidget> BuildCreationPanel(FOnLiveLinkSourceCreated OnLiveLinkSourceCreated) const override;
	virtual TSharedPtr<ILiveLinkSource> CreateSource(const FString& ConnectionString) const override;
	//~ End ULiveLinkSourceFactory interface

	static FString CreateConnectionString(const FVeLiveLinkConnectionSettings& Settings);
};

#endif