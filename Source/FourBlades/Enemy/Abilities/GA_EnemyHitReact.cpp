// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Abilities/GA_EnemyHitReact.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Enemy/Interface/FBEnemyDataInterface.h"
#include "AbilityFragment/FBAbilityFragmentManager.h"
#include "AbilityFragment/AbilityFragment_AnimMontage.h"
#include "ActorComponents/ReactData.h"
#include "AI/FBAIController.h"
#include "GameFramework/Character.h"
#include "FourBlades.h"

UGA_EnemyHitReact::UGA_EnemyHitReact()
{
}

void UGA_EnemyHitReact::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);

	IFBEnemyDataInterface* EnemyDataInterface = Cast<IFBEnemyDataInterface>(ActorInfo->AvatarActor.Get());
	if (EnemyDataInterface && ReactMontage == nullptr)
	{
		if (UFBAbilityFragmentManager* FragmentManager = EnemyDataInterface->GetReactAbilityFragmentManager(AbilityTags.First()))
		{
			if (UAbilityFragment_AnimMontage* AnimMontageFragment = FragmentManager->FindFragmentByClass<UAbilityFragment_AnimMontage>())
			{
				ReactMontage = AnimMontageFragment->AnimMontage;
			}
		}
	}
}

void UGA_EnemyHitReact::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	float AnimPlayRate = 1.0f;
	if (TriggerEventData && TriggerEventData->OptionalObject)
	{
		if (const UReactData* ReactData = Cast<UReactData>(TriggerEventData->OptionalObject))
		{
			if (ReactData->ReactDurationTime > 0.0f)
			{
				AnimPlayRate = (ReactMontage->GetPlayLength() / ReactData->ReactDurationTime);
			}
		}
	}

	if (ACharacter* Character = Cast<ACharacter>(ActorInfo->AvatarActor.Get()))
	{
		if (AFBAIController* AIController = Cast<AFBAIController>(Character->GetController()))
		{
			AIController->StopAI();
		}
	}

	UAbilityTask_PlayMontageAndWait* PlayAttackTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("React"), ReactMontage, AnimPlayRate);

	PlayAttackTask->OnCompleted.AddDynamic(this, &UGA_EnemyHitReact::OnCompleteCallback);
	PlayAttackTask->OnInterrupted.AddDynamic(this, &UGA_EnemyHitReact::OnInterruptedCallback);
	PlayAttackTask->ReadyForActivation();
}

void UGA_EnemyHitReact::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (ACharacter* Character = Cast<ACharacter>(ActorInfo->AvatarActor.Get()))
	{
		if (AFBAIController* AIController = Cast<AFBAIController>(Character->GetController()))
		{
			AIController->ReStartAI();
		}
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
