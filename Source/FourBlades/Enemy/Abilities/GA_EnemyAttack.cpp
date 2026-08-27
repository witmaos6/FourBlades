// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Abilities/GA_EnemyAttack.h"
#include "Animation/FBAnimNotify_AttackHitCheck.h"
#include "Animation/FBAnimNotify_AllowNextCombo.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/AbilityTask/AbilityTask_Trace.h"
#include "AbilitySystem/TargetActor/TargetActor_Sweep.h"
#include "AbilitySystemComponent.h"
#include "GameplayTags/FBDefineGameplayTags.h"
#include "Library/FBDetectionUtilityLibrary.h"
#include "Enemy/Interface/FBEnemyDataInterface.h"
#include "AbilityFragment/FBAbilityFragmentManager.h"
#include "AbilityFragment/AbilityFragment_AnimMontage.h"
#include "AbilityFragment/AbilityFragment_AttackDamages.h"
#include "AbilitySystem/FBGameplayEffectContext.h"

UGA_EnemyAttack::UGA_EnemyAttack(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	AbilityTags.AddTag(FBTAG_ENEMY_ABILITY_ATTACK);

	ActivationOwnedTags.AddTag(FBTAG_ENEMY_STATE_ATTACK);

	AbilityConditionType = EAbilityConditionType::MeleeForwardAttack;
}

void UGA_EnemyAttack::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
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
				// AnimNotify를 공유하지 않도록 몽타주를 복제한다.
			}
			if (UAbilityFragment_AttackDamages* AttackDamageFragment = FragmentManager->FindFragmentByClass<UAbilityFragment_AttackDamages>())
			{
				AttackDamages = AttackDamageFragment;
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
				AttackHitCheck->OnAnimNotifyAttackHitCheck.BindUObject(this, &UGA_EnemyAttack::AnimNotifyAttackHitCheck);
			}
			if (UFBAnimNotify_AllowNextCombo* NextCombo = Cast<UFBAnimNotify_AllowNextCombo>(NotifyEvent.Notify))
			{
				NextCombo->OnAnimNotifyAllowNextCombo.BindUObject(this, &UGA_EnemyAttack::AnimNotifyNextCombo);
			}
		}
	}
}

void UGA_EnemyAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	const float AnimPlayRate = 1.0f;

	FName SectionName = GetNextSection();

	UAbilityTask_PlayMontageAndWait* PlayAttackTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("Attack"), AttackMontage, AnimPlayRate, SectionName);

	PlayAttackTask->OnCompleted.AddDynamic(this, &UGA_EnemyAttack::OnCompleteCallback);
	PlayAttackTask->OnInterrupted.AddDynamic(this, &UGA_EnemyAttack::OnInterruptedCallback);
	PlayAttackTask->ReadyForActivation();
}

FName UGA_EnemyAttack::GetNextSection()
{
	CurrentCombo = FMath::Max(CurrentCombo + 1, 1);
	FName NextSection = *FString::Printf(TEXT("%s%d"), *FString("Attack"), CurrentCombo);
	return NextSection;
}

void UGA_EnemyAttack::JumpToSection()
{
	FName NextSectionName = GetNextSection();
	if (CurrentMontage->IsValidSectionName(NextSectionName))
	{
		MontageJumpToSection(NextSectionName);
	}
}

void UGA_EnemyAttack::AnimNotifyAttackHitCheck()
{	
	UAbilityTask_Trace* AttackTask = UAbilityTask_Trace::CreateTask(this, TargetActorClass);
	AttackTask->OnComplete.AddDynamic(this, &UGA_EnemyAttack::OnTraceResultCallback);
	AttackTask->ReadyForActivation();
}

void UGA_EnemyAttack::AnimNotifyNextCombo()
{
	if (CombatTargetActor.IsValid())
	{
		bool ValidDistance = false;
		bool ValidRadius = false;
		AActor* Owner = CurrentActorInfo->AvatarActor.Get();
		AActor* Target = CombatTargetActor.Get();
		if (Owner && Target)
		{
			if (IFBEnemyDataInterface* AIPawn = Cast<IFBEnemyDataInterface>(Owner))
			{
				ValidDistance = UFBDetectionUtilityLibrary::InDistanceFromActor(Owner, Target, AIPawn->GetAbilityActivateMinRange(AbilityConditionType));
				ValidRadius = UFBDetectionUtilityLibrary::IsTargetInSectorFromActor(Owner, Target, AIPawn->GetAbilityActivateValidRadius(AbilityConditionType));
			}
		}

		if (ValidDistance && ValidRadius)
		{
			JumpToSection();
		}
	}
}

void UGA_EnemyAttack::OnTraceResultCallback(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
{
	AActor* OwningActor = CurrentActorInfo->AvatarActor.Get();

	if (UAbilitySystemBlueprintLibrary::TargetDataHasActor(TargetDataHandle, 0))
	{
		FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingGameplayEffectSpec(DamageEffect);
		FGameplayEffectContextHandle DamageEffectContextHandle = UAbilitySystemBlueprintLibrary::GetEffectContext(EffectSpecHandle);
		FFBGameplayEffectContext* DamageContext = FFBGameplayEffectContext::ExtractEffectContext(DamageEffectContextHandle);

		if (EffectSpecHandle.IsValid() && DamageContext)
		{
			if (AttackDamages->SkillDamageValues.IsValidIndex(CurrentCombo - 1))
			{
				const FFBSkillDamageValue& CurrentSkillDamageValue = AttackDamages->SkillDamageValues[CurrentCombo - 1];
				FFBDamageExecutionParams DamageExecutionParams = FFBDamageExecutionParams(CurrentSkillDamageValue.DamageCoefficient, CurrentSkillDamageValue.AdditionalFixedDamage);

				float LaunchPower = 0.0f;
				float ReactDuration = 0.0f;
				if (AttackDamages->LaunchInfos.IsValidIndex(CurrentCombo - 1))
				{
					LaunchPower = AttackDamages->LaunchInfos[CurrentCombo - 1].LaunchPower;
					ReactDuration = AttackDamages->LaunchInfos[CurrentCombo - 1].ReactDuration;
				}

				FFBTargetPayload TargetPayload = FFBTargetPayload(CurrentSkillDamageValue.AttackType, ReactDuration, LaunchPower);

				DamageContext->SetDamageExecutionParams(MoveTemp(DamageExecutionParams));
				DamageContext->SetTargetPayload(MoveTemp(TargetPayload));

				ApplyGameplayEffectSpecToTarget(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, EffectSpecHandle, TargetDataHandle);
			}
		}

		const FGameplayAbilityTargetData* TargetData = TargetDataHandle.Get(0);
		if (TargetData->GetActors().IsValidIndex(0))
		{
			CombatTargetActor = TargetData->GetActors()[0];
		}
	}
}

void UGA_EnemyAttack::OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	const TArray<FAnimNotifyEvent>& NotifyEvents = AttackMontage->Notifies;
	for (const FAnimNotifyEvent& NotifyEvent : NotifyEvents)
	{
		if (UFBAnimNotify_AttackHitCheck* AttackHitCheck = Cast<UFBAnimNotify_AttackHitCheck>(NotifyEvent.Notify))
		{
			AttackHitCheck->OnAnimNotifyAttackHitCheck.Unbind();
		}
		if (UFBAnimNotify_AllowNextCombo* NextCombo = Cast<UFBAnimNotify_AllowNextCombo>(NotifyEvent.Notify))
		{
			NextCombo->OnAnimNotifyAllowNextCombo.Unbind();
		}
	}
}

void UGA_EnemyAttack::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
	CurrentCombo = 0;
}

void UGA_EnemyAttack::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	CurrentCombo = 0;
}
