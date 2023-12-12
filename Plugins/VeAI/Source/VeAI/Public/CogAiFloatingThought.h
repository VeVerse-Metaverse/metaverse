// Authors: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022-2023 VeVerse AS. All rights reserved.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once

#include "GameFramework/Actor.h"
#include "CogAiFloatingThought.generated.h"

UCLASS(BlueprintType, Blueprintable)
class VEAI_API ACogAiFloatingThought : public AActor {
	GENERATED_BODY()

public:
	ACogAiFloatingThought();

	void SetThoughtAndEmotion(const FString& InThoughts, const FString& InEmotionName);

protected:
#pragma region Components

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="UI")
	class UWidgetComponent* FloatingThoughtWidgetComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="UI")
	UAudioComponent* AudioComponent = nullptr;

#pragma endregion

#pragma region Settings

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="UI")
	TSubclassOf<UUserWidget> FloatingThoughtWidgetClass = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="UI")
	float FlyUpSpeed = 30.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="UI")
	float FadeOutDuration = 1.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="UI")
	float LifeTime = 10.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="UI")
	USoundBase* Sound = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Widget|Content")
	float MinVisibilityDistance = 500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Widget|Content")
	float MaxVisibilityDistance = 1000.0f;

#pragma endregion

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="UI")
	FString EmotionName = TEXT("");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="UI")
	FString Thoughts = TEXT("");

	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

private:
	float ElapsedTime = 0.0f;
	FVector InitialLocation = FVector::ZeroVector;
};


/*
an animated emotion smiley for your widget. To do this, you'll need to create a custom UserWidget with an animated image. Here's a step-by-step guide on how to create an animated emotion smiley for your widget:

1. First, create a sprite sheet containing all the frames of your smiley animation. A sprite sheet is a single image file containing all the frames arranged in a grid.

2. Import the sprite sheet into your UE5 project as a Texture2D. To do this, open the Content Browser, right-click, and select "Import to /Game...". Then, navigate to your sprite sheet file and click "Open".

3. Create a new Material that will display the animated smiley. In the Content Browser, right-click and select "Material". Name it "M_AnimatedSmiley" and open it.

4. In the Material Editor, set the Material Domain to "User Interface" and the Blend Mode to "Translucent".

5. Add a "Texture Sample" node and assign your imported sprite sheet to it.

6. Add a "Customized UV" node and connect it to the UV input of the Texture Sample node.

7. To animate the smiley, create a "Time" node, and multiply it by a constant value to control the animation speed. Then, use a "Frac" node to loop the animation.

8. Calculate the UV coordinates for each frame of the animation using the "Floor" and "Divide" nodes, and connect the result to the Customized UV node.

9. Connect the output of the Texture Sample node to the Emissive Color and Opacity inputs of the material.

10. Save and close the Material Editor.

11. Create a new UserWidget blueprint and name it "W_AnimatedSmiley". Open it and add an "Image" widget to the canvas.

12. Assign the M_AnimatedSmiley material to the Image widget's "Brush" property.

13. Save and close the Widget Blueprint.

Now you can use the W_AnimatedSmiley widget in your floating text actor. Assign the W_AnimatedSmiley class to the `FloatingTextWidgetClass` property in your AMyFloatingTextActor-derived Blueprint, and the widget will display the animated emotion smiley.
 */
