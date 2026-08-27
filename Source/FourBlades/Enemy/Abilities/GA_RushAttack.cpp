// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Abilities/GA_RushAttack.h"
#include "GameplayTags/FBDefineGameplayTags.h"
#include "Animation/FBAnimNotify_AttackHitCheck.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "AbilitySystem/AbilityTask/AbilityTask_Trace.h"
#include "AbilitySystem/TargetActor/TargetActor_Sweep.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Enemy/Interface/FBEnemyDataInterface.h"
#include "AbilityFragment/FBAbilityFragmentManager.h"
#include "AbilityFragment/AbilityFragment_AnimMontage.h"
#include "AbilityFragment/AbilityFragment_AttackDamages.h"
#include "AbilitySystem/FBGameplayEffectContext.h"

UGA_RushAttack::UGA_RushAttack()
{
	AbilityTags.AddTag(FBTAG_ENEMY_ABILITY_ATTACK);

	ActivationOwnedTags.AddTag(FBTAG_ENEMY_STATE_ATTACK);
}

void UGA_RushAttack::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);

	IFBEnemyDataInterface* EnemyDataInterface = Cast<IFBEnemyDataInterface>(ActorInfo->AvatarActor.Get());
	if (EnemyDataInterface && RushMontage == nullptr)
	{
		if (UFBAbilityFragmentManager* FragmentManager = EnemyDataInterface->GetAbilityFragmentManager(AbilityTags.First()))
		{
			if (UAbilityFragment_AnimMontage* AnimMontageFragment = FragmentManager->FindFragmentByClass<UAbilityFragment_AnimMontage>())
			{
				RushMontage = DuplicateObject<UAnimMontage>(AnimMontageFragment->AnimMontage, GetTransientPackage());
			}
			if (UAbilityFragment_AttackDamages* AttackDamageFragment = FragmentManager->FindFragmentByClass<UAbilityFragment_AttackDamages>())
			{
				AttackDamages = AttackDamageFragment;
			}
		}
	}

	if (RushMontage)
	{
		const TArray<FAnimNotifyEvent>& NotifyEvents = RushMontage->Notifies;
		for (const FAnimNotifyEvent& NotifyEvent : NotifyEvents)
		{
			if (UFBAnimNotify_AttackHitCheck* AttackHitCheck = Cast<UFBAnimNotify_AttackHitCheck>(NotifyEvent.Notify))
			{
				AttackHitCheck->OnAnimNotifyAttackHitCheck.BindUObject(this, &UGA_RushAttack::AnimNotifyAttackHitCheck);
			}
		}
	}
}

void UGA_RushAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	const float AnimPlayRate = 1.0f;

	UAbilityTask_PlayMontageAndWait* RushTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("Attack"), RushMontage, AnimPlayRate);

	RushTask->OnCompleted.AddDynamic(this, &UGA_RushAttack::OnCompleteCallback);
	RushTask->OnInterrupted.AddDynamic(this, &UGA_RushAttack::OnInterruptedCallback);
	RushTask->ReadyForActivation();
}

void UGA_RushAttack::AnimNotifyAttackHitCheck()
{
	UAbilityTask_Trace* AttackTask = UAbilityTask_Trace::CreateTask(this, TargetActorClass);
	AttackTask->OnComplete.AddDynamic(this, &UGA_RushAttack::OnTraceResultCallback);
	AttackTask->ReadyForActivation();
}

void UGA_RushAttack::OnTraceResultCallback(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
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
		}
	}
}

void UGA_RushAttack::OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	const TArray<FAnimNotifyEvent>& NotifyEvents = RushMontage->Notifies;
	for (const FAnimNotifyEvent& NotifyEvent : NotifyEvents)
	{
		if (UFBAnimNotify_AttackHitCheck* AttackHitCheck = Cast<UFBAnimNotify_AttackHitCheck>(NotifyEvent.Notify))
		{
			AttackHitCheck->OnAnimNotifyAttackHitCheck.Unbind();
		}
	}
}