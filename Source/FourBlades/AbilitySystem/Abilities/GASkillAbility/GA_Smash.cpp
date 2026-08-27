// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/GASkillAbility/GA_Smash.h"
#include "Character/FBCharacterBase.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Animation/FBAnimNotify_AttackHitCheck.h"
#include "Animation/FBAnimNotify_AllowNextCombo.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/AbilityTask/AbilityTask_Trace.h"
#include "AbilitySystem/TargetActor/TargetActor_Sweep.h"
#include "AbilitySystem/FBAbilitySystemComponent.h"
#include "AbilitySystem/AbilitySystemData.h"
#include "GameplayTags/FBDefineGameplayTags.h"
#include "AbilitySystem/FBGameplayEffectContext.h"
#include "AbilityFragment/FBAbilityFragmentManager.h"
#include "AbilityFragment/AbilityFragment_AnimMontageList.h"
#include "AbilityFragment/AbilityFragment_AttackDamageList.h"
#include "AbilityFragment/FBSkillDamageValue.h"

UGA_Smash::UGA_Smash()
{
	AbilityTags.AddTag(FBTAG_CHARACTER_ABILITY_SMASH);

	RequiredTagsAnyOne.AddTag(FBTAG_CHARACTER_STATE_NORMAL);
	RequiredTagsAnyOne.AddTag(FBTAG_CHARACTER_STATE_ATTACK);
	RequiredTagsAnyOne.AddTag(FBTAG_CHARACTER_STATE_SPRINT);

	ActivationOwnedTags.AddTag(FBTAG_CHARACTER_STATE_ATTACK);

	ActivationBlockedTags.AddTag(FBTAG_CHARACTER_ABILITY_BLOCK_SMASH);

	ComboAttackDataIndex = 0;
}

void UGA_Smash::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);

	if (const UFBAbilityFragmentManager* FragmentManager = GetAbilityFragmentManager(Spec))
	{
		if (UAbilityFragment_AnimMontageList* AnimMontageFragment = FragmentManager->FindFragmentByClass<UAbilityFragment_AnimMontageList>())
		{
			AnimMontageListFragment = AnimMontageFragment;
		}
		if (UAbilityFragment_AttackDamageList* AttackDamageFragment = FragmentManager->FindFragmentByClass<UAbilityFragment_AttackDamageList>())
		{
			AttackDamageListFragment = AttackDamageFragment;
		}
	}

	if (AnimMontageListFragment)
	{
		for (UAnimMontage* AnimMontage : AnimMontageListFragment->AnimMontageList)
		{
			const TArray<FAnimNotifyEvent>& NotifyEvents = AnimMontage->Notifies;

			for (const FAnimNotifyEvent& NotifyEvent : NotifyEvents)
			{
				if (UFBAnimNotify_AttackHitCheck* AttackHitCheck = Cast<UFBAnimNotify_AttackHitCheck>(NotifyEvent.Notify))
				{
					AttackHitCheck->OnAnimNotifyAttackHitCheck.BindUObject(this, &ThisClass::AnimNotifyAttackHitCheck);
				}
				if (UFBAnimNotify_AllowNextCombo* AllowNextCombo = Cast<UFBAnimNotify_AllowNextCombo>(NotifyEvent.Notify))
				{
					AllowNextCombo->OnAnimNotifyAllowNextCombo.BindUObject(this, &ThisClass::AnimNotifyAllowNextCombo);
				}
			}
		}
	}
}

void UGA_Smash::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	AFBCharacterBase* Character = CastChecked<AFBCharacterBase>(ActorInfo->AvatarActor.Get());

	SetRotationAvatarToLastMoveDirection();

	UFBAbilitySystemComponent* ASC = Cast<UFBAbilitySystemComponent>(GetAbilitySystemComponentFromActorInfo());
	ComboAttackDataIndex = FMath::Clamp(ASC->AbilitySystemData->GetSlashComboIndex(), 0, AnimMontageListFragment->AnimMontageList.Num() - 1);

	const float AnimPlayRate = 1.2f;
	FName StartSectionName = GetNextSection();

	check(AnimMontageListFragment->AnimMontageList.IsValidIndex(ComboAttackDataIndex));

	MaxComboCount = AttackDamageListFragment->GetSkillDamageValueList(ComboAttackDataIndex).SkillDamageValues.Num();

	UAnimMontage* PlayMontage = AnimMontageListFragment->AnimMontageList[ComboAttackDataIndex];

	UAbilityTask_PlayMontageAndWait* PlayAttackTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("Attack"), PlayMontage, AnimPlayRate, StartSectionName);
	PlayAttackTask->OnCompleted.AddDynamic(this, &UGA_Smash::OnCompleteCallback);
	PlayAttackTask->OnInterrupted.AddDynamic(this, &UGA_Smash::OnInterruptedCallback);
	PlayAttackTask->ReadyForActivation();

	bAllowAttack = false;
}

FName UGA_Smash::GetNextSection()
{
	CurrentCombo = FMath::Clamp(CurrentCombo + 1, 1, MaxComboCount);
	FName NextSection = *FString::Printf(TEXT("%s%d"), *FString("Attack"), CurrentCombo);
	return NextSection;
}

void UGA_Smash::InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	if (bAllowAttack)
	{
		SetRotationAvatarToLastMoveDirection();

		if (CurrentCombo < MaxComboCount)
		{
			bInput = true;
			JumpToSection();
		}
	}
}

void UGA_Smash::JumpToSection()
{
	if (bInput)
	{
		MontageJumpToSection(GetNextSection());
		bInput = false;
		bAllowAttack = false;
	}
}

void UGA_Smash::AnimNotifyAttackHitCheck()
{
	UAbilityTask_Trace* AttackTask = UAbilityTask_Trace::CreateTask(this, TargetActorClass);
	AttackTask->OnComplete.AddDynamic(this, &UGA_Smash::OnTraceResultCallback);
	AttackTask->ReadyForActivation();
}

void UGA_Smash::AnimNotifyAllowNextCombo()
{
	bAllowAttack = true;
}

void UGA_Smash::OnTraceResultCallback(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
{
	AActor* OwningActor = CurrentActorInfo->AvatarActor.Get();
	if (UAbilitySystemBlueprintLibrary::TargetDataHasActor(TargetDataHandle, 0))
	{
		FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingGameplayEffectSpec(DamageEffect);
		FGameplayEffectContextHandle DamageEffectContextHandle = UAbilitySystemBlueprintLibrary::GetEffectContext(EffectSpecHandle);
		FFBGameplayEffectContext* DamageContext = FFBGameplayEffectContext::ExtractEffectContext(DamageEffectContextHandle);
		if (EffectSpecHandle.IsValid() && DamageContext)
		{
			if (AttackDamageListFragment->SkillDamageValueList.IsValidIndex(ComboAttackDataIndex))
			{
				const FSkillDamageList& SkillDamageList = AttackDamageListFragment->GetSkillDamageValueList(ComboAttackDataIndex);

				const FFBSkillDamageValue& CurrentSkillDamageValue = SkillDamageList.SkillDamageValues[CurrentCombo - 1];
				FFBDamageExecutionParams DamageExecutionParams = FFBDamageExecutionParams(CurrentSkillDamageValue.DamageCoefficient, CurrentSkillDamageValue.AdditionalFixedDamage);

				float LaunchPower = 0.0f;
				float ReactDuration = 0.0f;
				if (SkillDamageList.LaunchInfos.IsValidIndex(CurrentCombo - 1))
				{
					LaunchPower = SkillDamageList.LaunchInfos[CurrentCombo - 1].LaunchPower;
					ReactDuration = SkillDamageList.LaunchInfos[CurrentCombo - 1].ReactDuration;
				}

				FFBTargetPayload TargetPayload = FFBTargetPayload(CurrentSkillDamageValue.AttackType, ReactDuration, LaunchPower);

				DamageContext->SetDamageExecutionParams(MoveTemp(DamageExecutionParams));
				DamageContext->SetTargetPayload(MoveTemp(TargetPayload));

				ApplyGameplayEffectSpecToTarget(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, EffectSpecHandle, TargetDataHandle);
			}

			if (TargetDataGameplayCueTag.IsValid())
			{
				UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo_Checked();
				FGameplayEffectContextHandle CueContextHandle = UAbilitySystemBlueprintLibrary::GetEffectContext(EffectSpecHandle);
				CueContextHandle.AddActors(TargetDataHandle.Data[0].Get()->GetActors(), false);
				FGameplayCueParameters CueParam;
				CueParam.EffectContext = CueContextHandle;

				SourceASC->ExecuteGameplayCue(TargetDataGameplayCueTag, CueParam);
			}
		}
	}
}

void UGA_Smash::OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	if (AnimMontageListFragment)
	{
		for (UAnimMontage* AnimMontage : AnimMontageListFragment->AnimMontageList)
		{
			const TArray<FAnimNotifyEvent>& NotifyEvents = AnimMontage->Notifies;

			for (const FAnimNotifyEvent& NotifyEvent : NotifyEvents)
			{
				if (UFBAnimNotify_AttackHitCheck* AttackHitCheck = Cast<UFBAnimNotify_AttackHitCheck>(NotifyEvent.Notify))
				{
					AttackHitCheck->OnAnimNotifyAttackHitCheck.Unbind();
				}
				if (UFBAnimNotify_AllowNextCombo* AllowNextCombo = Cast<UFBAnimNotify_AllowNextCombo>(NotifyEvent.Notify))
				{
					AllowNextCombo->OnAnimNotifyAllowNextCombo.Unbind();
				}
			}
		}
	}
}

void UGA_Smash::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	ComboAttackDataIndex = 0;
	CurrentCombo = 0;
	bAllowAttack = true;
	bInput = false;

	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}

void UGA_Smash::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	ComboAttackDataIndex = 0;
	CurrentCombo = 0;
	bAllowAttack = true;
	bInput = false;

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
