// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/FBAbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"

//FGameplayEffectContextHandle UFBAbilitySystemBlueprintLibrary::ApplyDamageEffect(const FFBDamageEffectParams& DamageEffectParams)
//{
//	const AActor* SourceAvatarActor = DamageEffectParams.SourceASC->GetAvatarActor();
//	FGameplayEffectContextHandle EffectContextHandle = DamageEffectParams.SourceASC->MakeEffectContext();
//	EffectContextHandle.AddSourceObject(SourceAvatarActor);
//	FGameplayEffectSpecHandle SpecHandle = DamageEffectParams.SourceASC->MakeOutgoingSpec(DamageEffectParams.DamageGameplayEffectClass, 1.0f, EffectContextHandle);
//
//	DamageEffectParams.SourceASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), DamageEffectParams.TargetASC);
//	return EffectContextHandle;
//}
