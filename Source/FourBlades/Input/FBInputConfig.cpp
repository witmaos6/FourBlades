// Fill out your copyright notice in the Description page of Project Settings.


#include "Input/FBInputConfig.h"

UFBInputConfig::UFBInputConfig(const FObjectInitializer& ObjectInitializer)
{
}

const UInputAction* UFBInputConfig::FindNativeInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound) const
{
    for (const FFBInputAction& Action : NativeInputActions)
    {
        if (Action.InputAction && (Action.InputTag == InputTag))
        {
            return Action.InputAction;
        }
    }
    
    if (bLogNotFound)
    {
        // To do: UE_LOG
    }

    return nullptr;
}

const UInputAction* UFBInputConfig::FindAbilityInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound) const
{
    for (const FFBInputAction& Action : AbilityInputActions)
    {
        if (Action.InputAction && (Action.InputTag == InputTag))
        {
            return Action.InputAction;
        }
    }

    if (bLogNotFound)
    {
        // To do: UE_LOG
    }
    return nullptr;
}
