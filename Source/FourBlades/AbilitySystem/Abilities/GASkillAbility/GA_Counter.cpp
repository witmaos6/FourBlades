// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/GASkillAbility/GA_Counter.h"
#include "GameplayTags/FBDefineGameplayTags.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Animation/FBAnimNotify_AttackHitCheck.h"
#include "Animation/FBAnimNotify_AllowNextCombo.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/AbilityTask/AbilityTask_Trace.h"
#include "AbilitySystem/TargetActor/TargetActor_Sweep.h"
#include "AbilitySystem/AbilitySystemData.h"
#include "AbilitySystem/GameplayEffect/FBDamageEffect.h"
#include "AbilitySystem/FBGameplayEffectContext.h"
#include "AbilityFragment/FBAbilityFragmentManager.h"
#include "Player/FBPlayerState.h"
#include "AbilitySystem/FBAbilitySet.h"
#include "AbilityFragment/AbilityFragment_AnimMontage.h"
#include "AbilityFragment/AbilityFragment_AttackDamages.h"
#include "AbilitySystem/FBAbilitySystemComponent.h"
#include "GameFramework/Character.h"
#include "Character/PawnComponent/FBCharacterComponent.h"
#include "FourBlades.h"

UGA_Counter::UGA_Counter()
{
	RequiredTagsAnyOne.AddTag(FBTAG_CHARACTER_STATE_GUARD);

	AbilityTags.AddTag(FBTAG_CHARACTER_ABILITY_GUARD_COUNTER);

	ActivationOwnedTags.AddTag(FBTAG_CHARACTER_STATE_ATTACK);
	ActivationOwnedTags.AddTag(FBTAG_CHARACTER_ABILITY_GUARD_COUNTER); // 임시로 사용

	BlockAbilitiesWithTag.AddTag(FBTAG_CHARACTER_ABILITY_GUARD); // 추후에 Ability.Block.Guard를 활성화하고 다시 제거하는 방향으로 수정 가능

	CancelAbilitiesWithTag.AddTag(FBTAG_CHARACTER_ABILITY_GUARD);
}

void UGA_Counter::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
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
			if (UFBAnimNotify_AllowNextCombo* AllowNextCombo = Cast<UFBAnimNotify_AllowNextCombo>(NotifyEvent.Notify))
			{
				AllowNextCombo->OnAnimNotifyAllowNextCombo.BindUObject(this, &ThisClass::AnimNotifyAllowNextCombo);
			}
		}
	}
	if (AttackDamages)
	{
		if (!AttackDamages->SkillDamageValues.IsEmpty())
		{
			MaxComboCount = AttackDamages->SkillDamageValues.Num();
		}
	}
}

void UGA_Counter::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	const float AnimPlayRate = 1.5f;
	FName StartSectionName = GetNextSection();

	UAbilityTask_PlayMontageAndWait* PlayAttackTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("Attack"), AttackMontage, AnimPlayRate, StartSectionName);
	PlayAttackTask->OnCompleted.AddDynamic(this, &UGA_Counter::OnCompleteCallback);
	PlayAttackTask->OnBlendOut.AddDynamic(this, &UGA_Counter::OnBlendOut);
	PlayAttackTask->OnInterrupted.AddDynamic(this, &UGA_Counter::OnInterruptedCallback);
	PlayAttackTask->ReadyForActivation();

	bAllowAttack = false;
}

FName UGA_Counter::GetNextSection()
{
	CurrentCombo = FMath::Clamp(CurrentCombo + 1, 1, MaxComboCount);
	FName NextSection = *FString::Printf(TEXT("%s%d"), *FString("Attack"), CurrentCombo);
	return NextSection;
}

void UGA_Counter::InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	if (bAllowAttack)
	{
		if (CurrentCombo < MaxComboCount)
		{
			bInput = true;
			JumpToSection();
		}
	}
}

void UGA_Counter::JumpToSection()
{
	if (bInput)
	{
		MontageJumpToSection(GetNextSection());
		bInput = false;
		bAllowAttack = false;
	}
}

void UGA_Counter::AnimNotifyAttackHitCheck()
{
	UAbilityTask_Trace* AttackTask = UAbilityTask_Trace::CreateTask(this, TargetActorClass);
	AttackTask->OnComplete.AddDynamic(this, &UGA_Counter::OnTraceResultCallback);
	AttackTask->ReadyForActivation();
}

void UGA_Counter::AnimNotifyAllowNextCombo()
{
	bAllowAttack = true;
}

void UGA_Counter::OnTraceResultCallback(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
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

void UGA_Counter::OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	const TArray<FAnimNotifyEvent>& NotifyEvents = AttackMontage->Notifies;
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

void UGA_Counter::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	AbilityReset();

	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}

void UGA_Counter::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	AbilityReset();

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_Counter::AbilityReset()
{
	CurrentCombo = 0;
	bAllowAttack = true;
	bInput = false;

	if (UFBAbilitySystemComponent* ASC = Cast<UFBAbilitySystemComponent>(GetAbilitySystemComponentFromActorInfo()))
	{
		if (ACharacter* OwningCharacter = Cast<ACharacter>(GetAvatarActorFromActorInfo()))
		{
			if (UFBCharacterComponent* CharacterComponent = OwningCharacter->GetComponentByClass<UFBCharacterComponent>())
			{
				FName SwitchingInputConfig = ASC->AbilitySystemData->GetSwitchingInputConfig();
				FName Counter = FName(TEXT("Counter"));

				if (!SwitchingInputConfig.IsNone() && CharacterComponent->IsInputConfigActive(Counter))
				{
					CharacterComponent->SwitchInputConfig(Counter, SwitchingInputConfig);
				}
				else
				{
					CharacterComponent->DeactivateInputConfig(Counter);
				}

				ASC->AbilitySystemData->SetSwitchingInputConfig(NAME_None);
			}
		}
	}
}
