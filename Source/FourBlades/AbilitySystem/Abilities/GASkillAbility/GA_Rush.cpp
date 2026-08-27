// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/GASkillAbility/GA_Rush.h"
#include "GameplayTags/FBDefineGameplayTags.h"
#include "AbilityFragment/FBSkillDamageValue.h"
#include "Animation/FBAnimNotify_AttackHitCheck.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "AbilitySystem/TargetActor/TargetActor_Sweep.h"
#include "AbilitySystem/AbilityTask/AbilityTask_Trace.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/FBAbilitySystemComponent.h"
#include "AbilityFragment/FBAbilityFragmentManager.h"
#include "AbilityFragment/AbilityFragment_AnimMontage.h"
#include "AbilityFragment/AbilityFragment_AttackDamages.h"
#include "Player/FBPlayerState.h"
#include "AbilitySystem/FBAbilitySet.h"

UGA_Rush::UGA_Rush()
{
	AbilityTags.AddTag(FBTAG_CHARACTER_ABILITY_RUSH);

	RequiredTagsAnyOne.AddTag(FBTAG_CHARACTER_STATE_SPRINT);

	ActivationOwnedTags.AddTag(FBTAG_CHARACTER_STATE_ATTACK);

	BlockAbilitiesWithTag.AddTag(FBTAG_CHARACTER_ABILITY_SMASH);
	// BlockAbilitiesWithTag¿¡ Character.Ability.RightCoutinuousAttack Ãß°¡
}

void UGA_Rush::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);

	if (const UFBAbilityFragmentManager* FragmentManager = GetAbilityFragmentManager(Spec))
	{
		if (UAbilityFragment_AnimMontage* AnimMontageFragment = FragmentManager->FindFragmentByClass<UAbilityFragment_AnimMontage>())
		{
			AttackMontage = AnimMontageFragment->AnimMontage;
		}
		if (UAbilityFragment_AttackDamages* AttackDamageFragment = FragmentManager->FindFragmentByClass<UAbilityFragment_AttackDamages>())
		{
			AttackDamages = AttackDamageFragment;
		}
	}

	if (AttackMontage)
	{
		const TArray<FAnimNotifyEvent>& NotifyEvents = AttackMontage->Notifies;

		for (const FAnimNotifyEvent& NotifyEvent : NotifyEvents)
		{
			if (UFBAnimNotify_AttackHitCheck* AttackHitCheck = Cast<UFBAnimNotify_AttackHitCheck>(NotifyEvent.Notify))
			{
				AttackHitCheck->OnAnimNotifyAttackHitCheck.BindUObject(this, &ThisClass::AnimNotifyAttackHitCheck);
			}
		}
	}
}

void UGA_Rush::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	const float AnimPlayRate = 1.5f;
	FName StartSectionName = FName(TEXT("Attack1"));

	UAbilityTask_PlayMontageAndWait* PlayAttackTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("Attack"), AttackMontage, AnimPlayRate, StartSectionName);
	PlayAttackTask->OnCompleted.AddDynamic(this, &UGA_Rush::OnCompleteCallback);
	PlayAttackTask->OnBlendOut.AddDynamic(this, &UGA_Rush::OnBlendOut);
	PlayAttackTask->OnInterrupted.AddDynamic(this, &UGA_Rush::OnInterruptedCallback);
	PlayAttackTask->ReadyForActivation();
}

void UGA_Rush::AnimNotifyAttackHitCheck()
{
	UAbilityTask_Trace* AttackTask = UAbilityTask_Trace::CreateTask(this, TargetActorClass);
	AttackTask->OnComplete.AddDynamic(this, &UGA_Rush::OnTraceResultCallback);
	AttackTask->ReadyForActivation();
}

void UGA_Rush::OnTraceResultCallback(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
{
	UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo_Checked();
	AActor* OwningActor = CurrentActorInfo->AvatarActor.Get();

	if (UAbilitySystemBlueprintLibrary::TargetDataHasActor(TargetDataHandle, 0))
	{
		FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingGameplayEffectSpec(DamageEffect);
		FGameplayEffectContextHandle DamageEffectContextHandle = UAbilitySystemBlueprintLibrary::GetEffectContext(EffectSpecHandle);
		FFBGameplayEffectContext* DamageContext = FFBGameplayEffectContext::ExtractEffectContext(DamageEffectContextHandle);

		if (EffectSpecHandle.IsValid() && DamageContext)
		{
			if (AttackDamages->SkillDamageValues.IsValidIndex(0))
			{
				const FFBSkillDamageValue& CurrentSkillDamageValue = AttackDamages->SkillDamageValues[0];
				FFBDamageExecutionParams DamageExecutionParams = FFBDamageExecutionParams(CurrentSkillDamageValue.DamageCoefficient, CurrentSkillDamageValue.AdditionalFixedDamage);

				float LaunchPower = 0.0f;
				float ReactDuration = 0.0f;
				if (AttackDamages->LaunchInfos.IsValidIndex(0))
				{
					LaunchPower = AttackDamages->LaunchInfos[0].LaunchPower;
					ReactDuration = AttackDamages->LaunchInfos[0].ReactDuration;
				}

				FFBTargetPayload TargetPayload = FFBTargetPayload(CurrentSkillDamageValue.AttackType, ReactDuration, LaunchPower);

				DamageContext->SetDamageExecutionParams(MoveTemp(DamageExecutionParams));
				DamageContext->SetTargetPayload(MoveTemp(TargetPayload));

				ApplyGameplayEffectSpecToTarget(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, EffectSpecHandle, TargetDataHandle);
			}

			if (TargetDataGameplayCueTag.IsValid())
			{
				FGameplayEffectContextHandle CueContextHandle = UAbilitySystemBlueprintLibrary::GetEffectContext(EffectSpecHandle);
				CueContextHandle.AddActors(TargetDataHandle.Data[0].Get()->GetActors(), false);
				FGameplayCueParameters CueParam;
				CueParam.EffectContext = CueContextHandle;

				SourceASC->ExecuteGameplayCue(TargetDataGameplayCueTag, CueParam);
			}
		}
	}
}
