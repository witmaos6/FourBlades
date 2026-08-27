// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Abilities/GA_SpawnActor.h"
#include "GameplayTags/FBDefineGameplayTags.h"
#include "Animation/FBAnimNotify_AttackHitCheck.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "AbilitySystem/AbilityTask/AbilityTask_SpawnActorDeferred.h"
#include "Abilities/Tasks/AbilityTask_SpawnActor.h"
#include "Enemy/Interface/FBEnemyDataInterface.h"
#include "Interface/FBGameplayEffectApplicator.h"
#include "AbilityFragment/FBAbilityFragmentManager.h"
#include "AbilityFragment/AbilityFragment_AnimMontage.h"
#include "AbilityFragment/AbilityFragment_AttackDamages.h"
#include "AbilityFragment/AbilityFragment_ShotActor.h"
#include "FourBlades.h"


UGA_SpawnActor::UGA_SpawnActor(const FObjectInitializer& ObjectInitializer)
{
	AbilityTags.AddTag(FBTAG_ENEMY_ABILITY_ATTACK);

	ActivationOwnedTags.AddTag(FBTAG_ENEMY_STATE_ATTACK);
}

void UGA_SpawnActor::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);

	IFBEnemyDataInterface* EnemyDataInterface = Cast<IFBEnemyDataInterface>(ActorInfo->AvatarActor.Get());
	if (EnemyDataInterface && AttackMontage == nullptr)
	{
		if (UFBAbilityFragmentManager* FragmentManager = EnemyDataInterface->GetAbilityFragmentManager(AbilityTags.First()))
		{
			if (UAbilityFragment_AnimMontage* AnimMontageFragment = FragmentManager->FindFragmentByClass<UAbilityFragment_AnimMontage>())
			{
				AttackMontage = DuplicateObject<UAnimMontage>(AnimMontageFragment->AnimMontage, GetTransientPackage());
			}
			if (UAbilityFragment_AttackDamages* AttackDamageFragment = FragmentManager->FindFragmentByClass<UAbilityFragment_AttackDamages>())
			{
				AttackDamages = AttackDamageFragment;
			}
			if (UAbilityFragment_ShotActor* ShotActorFragment = FragmentManager->FindFragmentByClass<UAbilityFragment_ShotActor>())
			{
				SpawnActorClass = ShotActorFragment->SpawnActorClass;
			}
		}
	}

	if (AttackMontage)
	{
		const TArray<FAnimNotifyEvent>& NotifyEvents = AttackMontage->Notifies;
		for (const FAnimNotifyEvent& NotifyEvent : NotifyEvents)
		{
			if (UFBAnimNotify_AttackHitCheck* AttackHitCheck = Cast<UFBAnimNotify_AttackHitCheck>(NotifyEvent.Notify))
			{
				AttackHitCheck->OnAnimNotifyAttackHitCheck.BindUObject(this, &UGA_SpawnActor::AnimNotifyAttackHitCheck);
			}
		}
	}
}

void UGA_SpawnActor::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	const float AnimPlayRate = 1.0f;

	UAbilityTask_PlayMontageAndWait* PlayAttackTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("Attack"), AttackMontage, AnimPlayRate);

	PlayAttackTask->OnCompleted.AddDynamic(this, &UGA_SpawnActor::OnCompleteCallback);
	PlayAttackTask->OnInterrupted.AddDynamic(this, &UGA_SpawnActor::OnInterruptedCallback);
	PlayAttackTask->ReadyForActivation();
}

void UGA_SpawnActor::AnimNotifyAttackHitCheck()
{
	UAbilityTask_SpawnActorDeferred* SpawnActorTask = UAbilityTask_SpawnActorDeferred::CreateSpawnActorTask(this, SpawnActorClass);
	if (SpawnActorTask)
	{
		AActor* SpawnActor;
		FTransform SpawnTransform = GetOwnerSocketTransform(SpawnSocketName);
		AActor* OwnerActor = GetAvatarActorFromActorInfo();
		if (OwnerActor == nullptr)
			return;

		SpawnTransform.SetRotation(OwnerActor->GetActorRotation().Quaternion());

		bool bResult = SpawnActorTask->BeginSpawningActor(this, SpawnActor, SpawnTransform);

		if (APawn* TargetPawn = Cast<APawn>(GetCombatTargetActor()))
		{
			FVector ShotDirection = (TargetPawn->GetActorLocation() - OwnerActor->GetActorLocation()).GetSafeNormal();
			FRotator ShotRotator = OwnerActor->GetActorForwardVector().Rotation();
			ShotRotator.Pitch = ShotDirection.Rotation().Pitch;
			SpawnTransform.SetRotation(ShotRotator.Quaternion());
		}	

		if (bResult && SpawnActor)
		{
			if (IFBGameplayEffectApplicator* GameplayEffectActor = Cast<IFBGameplayEffectApplicator>(SpawnActor))
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

					GameplayEffectActor->InitDamageExecutionParams(MoveTemp(DamageExecutionParams));
					GameplayEffectActor->InitTargetPayload(MoveTemp(TargetPayload));
				}
			}
			SpawnActorTask->FinishSpawningActor(this, SpawnActor, SpawnTransform);
		}
	}
}

void UGA_SpawnActor::OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	const TArray<FAnimNotifyEvent>& NotifyEvents = AttackMontage->Notifies;
	for (const FAnimNotifyEvent& NotifyEvent : NotifyEvents)
	{
		if (UFBAnimNotify_AttackHitCheck* AttackHitCheck = Cast<UFBAnimNotify_AttackHitCheck>(NotifyEvent.Notify))
		{
			AttackHitCheck->OnAnimNotifyAttackHitCheck.Unbind();
		}
	}
}