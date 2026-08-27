// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/GASharedAbility/GA_HitLaunch.h"
#include "ActorComponents/LaunchData.h"
#include "GameFramework/Character.h"
#include "GameplayTags/FBDefineGameplayTags.h"
#include "CharacterComponents/FBCharacterMovementComponent.h"
#include "FourBlades.h"

UGA_HitLaunch::UGA_HitLaunch()
{
	AbilityTags.AddTag(FBTAG_SHAREDABILITY_LAUNCH);

	FAbilityTriggerData TriggerData;
	TriggerData.TriggerTag = FBTAG_SHAREDABILITY_LAUNCH;
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	AbilityTriggers.Add(TriggerData);
}

void UGA_HitLaunch::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	// 현재는 StartLaunch로 바닥을 따라 밀격이 되는 기능만 구현되어 있다. 밀격의 종류가 많아지면 수정이 필요하다.

	if (TriggerEventData && TriggerEventData->OptionalObject)
	{
		if (const ULaunchData* LaunchData = Cast<ULaunchData>(TriggerEventData->OptionalObject))
		{
			if (LaunchData->Direction == FVector::ZeroVector)
			{
				EndAbility(Handle, ActorInfo, ActivationInfo, false, true);
			}
			else if (ACharacter* Character = Cast<ACharacter>(ActorInfo->AvatarActor))
			{
				if (UFBCharacterMovementComponent* FBCMC = Cast<UFBCharacterMovementComponent>(Character->GetCharacterMovement()))
				{
					FBCMC->StartLaunch(LaunchData->Direction, LaunchData->Distance, LaunchData->Duration);
				}
			}
		}
	}

	EndAbility(Handle, ActorInfo, ActivationInfo, false, false);
}
