# UE4 Module README

This README describes the purpose and usage of the header files included in the UE4 module. The module is closed-source,
so no license or contact information is provided.

## Header Files

- **UIAIInfoWidget.h**: A widget used in the widget component over the heads of AI characters, displaying their names,
  voice indicators, and subtitles for their spoken text.
- **VeAI.h**: The module definition.
- **VeAiBlackboardKeys.h**: A namespace containing all blackboard keys used for Simple FSM mechanics, defined as FName
  literals.
- **VeAiBlueprintFunctionLibrary.h**: A helper library containing functions to generate TTS URLs for TTS voiceovers for
  phrases spoken by the AI.
- **VeAiCharacter**: The base AI character, which has a reference to its AIPlayerState, and a widget component
  containing the UIAIInfoWidget.
- **VeAiEnums.h**: Contains various enums used in the Simple FSM mechanics, such as supported character voices, entity
  types, and supported action types.
- **VeAiPlayerState.h**: The player state actor (derived from AInfo) for the AI player state, containing user metadata
  and replicated state for character voice (whether the AI is speaking or not).
- **VeAiSimpleFsmAction.h**: A struct defining a simple action used with the neural network to generate FSM states.
- **VeAiSimpleFsmAiController.h**: A controller class used for FSM mechanics, containing a behavior component and
  perception component.
- **VeAiSimpleFsmBehaviorComponent.h**: A component class that manages the FSM state context for a specific pawn and its
  blackboard state.
- **VeAiSimpleFsmEntityComponent.h**: A component class used as a base for all Simple FSM mechanics-based AI perceptible
  entities, featuring an ID and description used for neural network awareness of objects in the scene around AI
  characters.
- **VeAiSimpleFsmLocationComponent.h**: A component class derived from Simple FSM Entity Component, describing a
  location where other AI entities reside.
- **VeAiSimpleFsmManager.h**: An actor placed in the scene that manages all Simple FSM mechanics-powered entity actors
  in the current world. Builds the structure of locations and entities within them, then requests Simple FSM states to
  be played by AI NPCs.
- **VeAiSimpleFsmState.h**: A struct containing metadata for a Simple FSM state, including the NPC that performs the
  action and all metadata required for the AI to perform the action.
- **VeAiSimpleFsmSubjectComponent.h**: A component derived from the Entity Component, marking the owner actor as an NPC
  and allowing developers to set the AI-generated voice.
- **VeAiSubject.h**: Contains an interface with "Say" and "Emote" functions used in blueprints by AI NPC characters.
- **VeAiSubjectInfoWidget.h**: Contains a debug widget used to display information about the current state assigned to
  an NPC in screen space over the character.
- **VeAITask_CompleteSimpleFsmState.h**: UBTTaskNode allowing developers to notify the FSM manager that the current
  state is completed using the decision tree.
- **VeAITask_Emote.h**: UBTTaskNode used for making characters express emotions, currently calling the Emote function
  from the IVeAiSubject interface, handled in BP.
- **VeAITask_SayTo.h**: UBTTaskNode used for making characters say something to each other. Calls the "SayTo" function
  from the interface.

## Cog AI (Hackerman AI v2)

```json
[
  {
    "from": "npc",
    "content": {
      "query": "whoami"
    }
  },
  {
    "from": "system",
    "content": {
      "query": "whoami",
      "name": "Ava Reyes",
      "desc": "Woman, 30yo, blonde, beautiful, 175cm tall, yuo wear white blouse, black skirt and black shoes. Your eyes are blue. Artist. From small town, middle-class family, had good grades in school, didn't graduate the university",
      "faction": "evil",
      "personality": {
        "aggression": "low",
        "greed": "low",
        "loyalty": "high",
        "courage": "medium",
        "intelligence": "high",
        "compassion": "medium",
        "honor": "medium",
        "stealth": "low"
      }
    }
  },
  {
    "from": "npc",
    "content": {
      "query": "context"
    }
  },
  {
    "from": "system",
    "content": {
      "query": "context",
      "desc": "Unreal Engine editor test level",
      "location": "Unreal editor test level, blue sky with volumetric clouds, default checkerboard floor, platform floating in the middle of nowhere",
      "time": "noon",
      "weather": "sunny",
      "vibe": "neutral"
    }
  },
  {
    "from": "npc",
    "content": {
      "query": "perception"
    }
  },
  {
    "from": "system",
    "content": {
      "query": "perception",
      "perception": {
        "visual": [
          {
            "name": "Cube",
            "desc": "White cube, rigid, static"
          },
          {
            "name": "Sun",
            "desc": "Bright"
          }
        ],
        "audio": [
          {
            "name": "Wind",
            "desc": "very light wind is blowing"
          }
        ],
        "other": [
          {
            "name": "Sun",
            "desc": "warm sunlight"
          }
        ]
      }
    }
  },
  {
    "from": "npc",
    "content": {
      "query": "actions"
    }
  },
  {
    "from": "system",
    "content": {
      "query": "actions",
      "actions": [
        {
          "type": "say",
          "target": "whom to say to",
          "message": "what to say",
          "emotion": "what do you feel",
          "thoughts": "your thoughts",
          "voice": "whisper, normal, loud"
        }
      ]
    }
  },
  {
    "from": "npc",
    "content": {
      "query": "inspect",
      "target": "Cube"
    }
  },
  {
    "from": "system",
    "content": {
      "query": "inspect",
      "target": {
        "name": "Cube",
        "desc": "White cube, rigid, static. Can't be moved. Fixed on the floor."
      }
    }
  },
  {
    "from": "npc",
    "content": {
      "query": "perception"
    }
  },
  {
    "from": "system",
    "content": {
      "query": "perception",
      "perception": {
        "visual": [
          {
            "name": "Bandit Nober",
            "desc": "faction: same as yours"
          }
        ],
        "audio": [
        ],
        "other": [
        ]
      }
    }
  }
]
```

Backend response (AI action and system request):

```json
{
  "data": [
    {
      "action": {
        "emotion": "friendly",
        "message": "Hey Nober, how are you doing?",
        "target": "Bandit Nober",
        "thoughts": "I hope he can help me with something and become my partner",
        "type": "say"
      }
    },
    {
      "system": "perception"
    }
  ]
}
```