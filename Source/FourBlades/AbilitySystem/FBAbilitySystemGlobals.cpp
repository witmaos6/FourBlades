// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/FBAbilitySystemGlobals.h"
#include "AbilitySystem/FBGameplayEffectContext.h"

FGameplayEffectContext* UFBAbilitySystemGlobals::AllocGameplayEffectContext() const
{
	return new FFBGameplayEffectContext();
}
