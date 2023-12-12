// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.

#pragma once

#include "ApiPersonaMetadata.h"
#include "ApiUserMetadata.h"
#include "ManagedTextureRequester.h"
#include "Components/UIInputWidget.h"
#include "Components/UIFormInputWidget.h"
#include "UIWidget.h"
#include "UIRPMAvatarLinkWidget.generated.h"

/**
 * 
 */
UCLASS()
class VEUI_API UUIRPMAvatarLinkWidget final : public UUIWidget {
	GENERATED_BODY()

	virtual void RegisterCallbacks() override;
	virtual void UnregisterCallbacks() override;

	virtual void NativeConstruct() override;
	
	
	//void SetIsLoading(bool bInIsLoading) const;
	
	FDelegateHandle OnCloseButtonWidgetDelegateHandle;
	FDelegateHandle OnFollowButtonWidgetDelegateHandle;
	FDelegateHandle OnCancelButtonClickedDelegateHandle;
	FDelegateHandle OnSubmitButtonClickedDelegateHandle;
	FDelegateHandle OnCreateButtonClickedDelegateHandle;
	FDelegateHandle OnUrlPersonInputDelegateHandle;
	

public:
	
	void SuccessUpdateAvatarFiles(const FApiFileMetadata& InMetadata, const bool bInSuccessful, const FString& InError) const;
	//void SuccessPostDefaultAvatarFiles(const bool bInSuccessful, const FString& InError) const;
	void SuccessCreatePersona(const FApiPersonaMetadata& InMetadata,bool bInSuccessful, const FString& InError);
	void CreatePersona();
	void UpdateFilePersona(const FApiPersonaMetadata& InMetadata);
	//void SetDefaultPersona(const FApiPersonaMetadata& InMetadata);
	void UpdateAvatarFiles();
	
	FApiUserMetadata Metadata;

	void SetMetadata(const FApiUserMetadata& InMetadata);

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UUIButtonWidget* CloseButtonWidget;
	
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UUIButtonWidget* CancelButtonWidget;
		
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UUIButtonWidget* SubmitButtonWidget;
	
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UUIButtonWidget* CreateButtonWidget;
	
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UUIFormInputWidget* UrlWidget;
	
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UUIInputWidget* PersonaNameWidget;

	DECLARE_EVENT(UUIUserAddFileLink, FOnClosed);
	FOnClosed OnClosedDelegate;

	DECLARE_EVENT(UUIUserAddFileLink, FOnSubmit);
	FOnSubmit OnSubmitDelegate;
	
	DECLARE_EVENT(UUIUserAddFileLink, FOnCreate);
	FOnCreate OnCreateDelegate;

	void OnUrlTextCommittedCallback(const FText& InText, ETextCommit::Type InCommitMethod);

	/** Validation result message. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UTextBlock* MessageWidget;

	/** Sound to play on message fade in. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Widget|Sound")
	USoundBase* MessageFadeInSound;

	/** Sound to play on message fade out. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Widget|Sound")
	USoundBase* MessageFadeOutSound;

	/** Sound to play on successful login. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Widget|Sound")
	USoundBase* LoginSuccessSound;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Widget|Content")
	FText MessageText;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Animation", meta=(BindAnimOptional))
	UWidgetAnimation* MessageFadeInAnimation;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Animation", meta=(BindAnimOptional))
	UWidgetAnimation* MessageFadeOutAnimation;

	UFUNCTION(BlueprintCallable, Category="Widget|Animation")
	void ShowMessage();

	UFUNCTION(BlueprintCallable, Category="Widget|Animation")
	void HideMessage();

	UFUNCTION(BlueprintCallable)
	void SetMessageText(const FText& InText);
	
	virtual UUMGSequencePlayer* Hide(bool bAnimate = true, bool bPlaySound = true) override;
};

