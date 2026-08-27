// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/GameplayEffect/FBDamageEffect.h"
#include "AbilitySystem/Attributes/FBCombatSet.h"
#include "FBDamageExecution.h"

UFBDamageEffect::UFBDamageEffect()
{
	DurationPolicy = EGameplayEffectDurationType::Instant;

	FGameplayEffectExecutionDefinition ExecutionDefinition;
	ExecutionDefinition.CalculationClass = UFBDamageExecution::StaticClass();
	Executions.Add(ExecutionDefinition);
}
