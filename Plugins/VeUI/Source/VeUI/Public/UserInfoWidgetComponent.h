// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.
#pragma once

#include "Components/WidgetComponent.h"
#include "UserInfoWidgetComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class VEUI_API UUserInfoWidgetComponent final : public UWidgetComponent {
	GENERATED_BODY()

public:
	UUserInfoWidgetComponent();

	void SetPlayerState(APlayerState* InPlayerState) const;

protected:
	virtual void BeginPlay() override;
};
