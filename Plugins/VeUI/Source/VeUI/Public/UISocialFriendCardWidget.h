// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.

#pragma once

#include "ApiUserMetadata.h"
#include "UIWidgetBase.h"

#include "ManagedTextureRequester.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "Components/Border.h"
#include "Components/TextBlock.h"
#include "Components/UIImageWidget.h"
#include "UISocialFriendCardWidget.generated.h"


UCLASS()
class VEUI_API UUISocialFriendCardWidget : public UUIWidgetBase, public IUserObjectListEntry {
	GENERATED_BODY()
	
public:
	
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIImageWidget* PreviewImageWidget;
	
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UBorder* AvatarBorderWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UTextBlock* NameTextWidget;
	
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UTextBlock* StatusTextWidget;
	
	UPROPERTY(EditAnywhere)
	UTexture2D* DefaultAvatar;

protected:
	
	// virtual void NativeConstruct() override;
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;

	void SetStatus(EApiPresenceStatus Status);

};
