// 

#pragma once

#include "CoreMinimal.h"
#include "UIWidgetBase.h"
#include "UObject/Object.h"
#include "UIPawnWidgetBase.generated.h"

/**
 * 
 */
UCLASS()
class VEUI_API UUIPawnWidgetBase : public UUIWidgetBase {
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	APawn* Pawn = nullptr;

	UFUNCTION(BlueprintImplementableEvent)
	void OnPawnSet(APawn* InPawn);
	
	void SetPawn(APawn* InPawn);
	APawn* GetPawn() const;
};
