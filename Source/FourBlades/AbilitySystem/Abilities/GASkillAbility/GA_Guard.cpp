// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/GASkillAbility/GA_Guard.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "GameplayTags/FBDefineGameplayTags.h"
#include "AbilitySystem/Attributes/FBHealthSet.h"
#include "Animation/FBAnimNotify_AllowNextCombo.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "AbilityFragment/FBAbilityFragmentManager.h"
#include "AbilityFragment/AbilityFragment_AnimMontage.h"
#include "Player/FBPlayerState.h"
#include "AbilitySystem/FBAbilitySet.h"
#include "GameFramework/Character.h"
#include "AbilitySystem/Attributes/FBGuardSet.h"
#include "Character/PawnComponent/FBCharacterComponent.h"
#include "AbilitySystem/FBAbilitySystemComponent.h"
#include "AbilitySystem/AbilitySystemData.h"
#include "FourBlades.h"

UGA_Guard::UGA_Guard()
{
	AbilityTags.AddTag(FBTAG_CHARACTER_ABILITY_GUARD);

	RequiredTagsAnyOne.AddTag(FBTAG_CHARACTER_STATE_ATTACK);
	RequiredTagsAnyOne.AddTag(FBTAG_CHARACTER_STATE_SPRINT);

	CancelAbilitiesWithTag.AddTag(FBTAG_CHARACTER_ABILITY_SLASH);
	CancelAbilitiesWithTag.AddTag(FBTAG_CHARACTER_ABILITY_SMASH);

	ActivationOwnedTags.AddTag(FBTAG_CHARACTER_STATE_GUARD);

	GuardBeginSectionName = TEXT("GuardBegin");
	GuardLoopBeginSectionName = TEXT("GuardLoopBegin");
	GuardLoopEndSectionName = TEXT("GuardLoopEnd");

	ParrySectionName = TEXT("Parry");

	HitSectionName = TEXT("Hit");

	bAllowReGuard = false;
	bCounterIMCActive = false;
	bChangeNormalGuard = false;
}

void UGA_Guard::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);

	if (const UFBAbilityFragmentManager* FragmentManager = GetAbilityFragmentManager(Spec))
	{
		if (UAbilityFragment_AnimMontage* AnimMontageFragment = FragmentManager->FindFragmentByClass<UAbilityFragment_AnimMontage>())
		{
			GuardMontage = AnimMontageFragment->AnimMontage;
		}
	}

	if (GuardMontage)
	{
		const TArray<FAnimNotifyEvent>& NotifyEvents = GuardMontage->Notifies;
		for (const FAnimNotifyEvent& NotifyEvent : NotifyEvents)
		{
			if (UFBAnimNotify_AllowNextCombo* AllowReguardNotify = Cast<UFBAnimNotify_AllowNextCombo>(NotifyEvent.Notify))
			{
				AllowReguardNotify->OnAnimNotifyAllowNextCombo.BindUObject(this, &UGA_Guard::ChangeNormalGuard);
			}
		}
	}
}

void UGA_Guard::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (UFBAbilitySystemComponent* ASC = Cast<UFBAbilitySystemComponent>(GetAbilitySystemComponentFromActorInfo()))
	{
		ASC->StopGuardGaugeRecovery();
	}

	BindGuard();

	ApplyTagEffect(ParryTagEffectClass);

	float AnimPlayRate = 0.5f;
	UAbilityTask_PlayMontageAndWait* PlayGuardTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("Guard"), GuardMontage, AnimPlayRate);
	PlayGuardTask->OnCompleted.AddDynamic(this, &UGA_Guard::OnCompleteCallback);
	PlayGuardTask->OnInterrupted.AddDynamic(this, &UGA_Guard::OnInterruptedCallback);
	PlayGuardTask->ReadyForActivation();
}

void UGA_Guard::BindGuard()
{
	if (UAbilitySystemComponent* ASC = CurrentActorInfo->AbilitySystemComponent.Get())
	{
		if (const UFBHealthSet* HealthSet = ASC->GetSet<UFBHealthSet>())
		{
			HealthSet->OnCalculateDamageReduction.BindUObject(this, &UGA_Guard::GuardDecreaseApplyDamage);
		}
	}
}

float UGA_Guard::GuardDecreaseApplyDamage(float ApplyDamage)
{
	float DamageReduction = 0.0f;

	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
	{
		if (ASC->HasMatchingGameplayTag(FBTAG_CHARACTER_ABILITY_GUARD_PARRY))
		{
			DamageReduction = ApplyDamage;
		}
		else
		{
			if (UFBGuardSet* GuardSet = const_cast<UFBGuardSet*>(ASC->GetSet<UFBGuardSet>()))
			{
				float RemainingDamage = GuardSet->AbsorbDamage(ApplyDamage);
				DamageReduction = ApplyDamage - RemainingDamage;
			}
		}
	}

	if (DamageReduction > 0.0f)
	{
		OnGuardSuccess();
	}
	else
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, true);
	}

	return DamageReduction;
}

void UGA_Guard::OnGuardSuccess()
{
	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
	{
		ASC->ExecuteGameplayCue(GuardSoundCueTag);

		if (ASC->HasMatchingGameplayTag(FBTAG_CHARACTER_ABILITY_GUARD_PARRY))
		{
			ApplyTagEffect(RemoveParryTagEffectClass);

			JumpToParry();

			UnbindDecreaseApplyDamage(CurrentActorInfo);

			ApplyTemporaryTimeDilation(0.4f, 0.2f);
		}
		else
		{
			MontageJumpToSection(HitSectionName);
		}
	}
}

void UGA_Guard::JumpToParry()
{
	if (UAnimInstance* AnimInstance = CurrentActorInfo->GetAnimInstance())
	{
		AnimInstance->Montage_SetPlayRate(GuardMontage, 1.0f);
		MontageJumpToSection(ParrySectionName);
	}
	ActivateCounterIMC();
}

void UGA_Guard::ActivateCounterIMC()
{
	ACharacter* OwningCharacter = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	UFBAbilitySystemComponent* ASC = Cast<UFBAbilitySystemComponent>(GetAbilitySystemComponentFromActorInfo());
	if (!OwningCharacter || !ASC)
	{
		return;
	}

	UFBCharacterComponent* CharacterComponent = OwningCharacter->GetComponentByClass<UFBCharacterComponent>();
	if (!CharacterComponent)
	{
		return;
	}

	FName Counter = FName(TEXT("Counter"));
	if (CharacterComponent->IsInputConfigActive(Counter))
	{
		return;
	}

	FName ContinuousAttack = FName(TEXT("ContinuousAttack"));
	FName BasicCombat = FName(TEXT("BasicCombat"));
	FName SwitchingInputConfig = NAME_None;

	if (CharacterComponent->IsInputConfigActive(BasicCombat))
	{
		CharacterComponent->SwitchInputConfig(BasicCombat, Counter);
		SwitchingInputConfig = BasicCombat;
	}
	else if (CharacterComponent->IsInputConfigActive(ContinuousAttack))
	{
		CharacterComponent->SwitchInputConfig(ContinuousAttack, Counter);
		SwitchingInputConfig = ContinuousAttack;
	}
	else
	{
		CharacterComponent->ActivateInputConfig(Counter);
	}

	ASC->AbilitySystemData->SetSwitchingInputConfig(SwitchingInputConfig);
	bCounterIMCActive = true;
}

void UGA_Guard::RestoreIMCFromCounter()
{
	if (!bCounterIMCActive)
	{
		return;
	}

	ACharacter* OwningCharacter = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	UFBAbilitySystemComponent* ASC = Cast<UFBAbilitySystemComponent>(GetAbilitySystemComponentFromActorInfo());
	if (!OwningCharacter || !ASC)
	{
		return;
	}

	UFBCharacterComponent* CharacterComponent = OwningCharacter->GetComponentByClass<UFBCharacterComponent>();
	if (!CharacterComponent)
	{
		return;
	}

	FName Counter = FName(TEXT("Counter"));
	FName SwitchingInputConfig = ASC->AbilitySystemData->GetSwitchingInputConfig();

	if (!SwitchingInputConfig.IsNone() && CharacterComponent->IsInputConfigActive(Counter))
	{
		CharacterComponent->SwitchInputConfig(Counter, SwitchingInputConfig);
	}
	else
	{
		CharacterComponent->DeactivateInputConfig(Counter);
	}

	ASC->AbilitySystemData->SetSwitchingInputConfig(NAME_None);
	bCounterIMCActive = false;
}

void UGA_Guard::InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	if (bAllowReGuard)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, false, false);
		// Note: 어빌리티 상태 초기화를 위해 EndAbility 호출, 이후 가드 어빌리티 즉시 재시작되는 것은 의도한 것
	}
	else
	{
		if (bChangeNormalGuard)
		{
			if (UAnimInstance* AnimInstance = CurrentActorInfo->GetAnimInstance())
			{
				FName SectionName = AnimInstance->Montage_GetCurrentSection();
				if (SectionName == ParrySectionName)
				{
					MontageJumpToSection(GuardLoopBeginSectionName);
					RestoreIMCFromCounter();
					BindGuard();
					bChangeNormalGuard = false;
				}
			}
		}
	}
}

void UGA_Guard::ChangeNormalGuard()
{
	bChangeNormalGuard = true;
}

void UGA_Guard::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	if (UAnimInstance* AnimInstance = CurrentActorInfo->GetAnimInstance())
	{
		FName SectionName = AnimInstance->Montage_GetCurrentSection();
		if (SectionName == GuardBeginSectionName || SectionName == GuardLoopBeginSectionName || SectionName == HitSectionName)
		{
			UnbindDecreaseApplyDamage(ActorInfo);
			AnimInstance->Montage_SetPlayRate(GuardMontage, 1.0f);
			MontageJumpToSection(GuardLoopEndSectionName);
		}
		else if (SectionName == ParrySectionName)
		{
			AllowReGuard();
		}
	}
}

void UGA_Guard::AllowReGuard()
{
	RestoreIMCFromCounter();
	bAllowReGuard = true;
}

void UGA_Guard::OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	if (GuardMontage)
	{
		const TArray<FAnimNotifyEvent>& NotifyEvents = GuardMontage->Notifies;
		for (const FAnimNotifyEvent& NotifyEvent : NotifyEvents)
		{
			if (UFBAnimNotify_AllowNextCombo* AllowReguardNotify = Cast<UFBAnimNotify_AllowNextCombo>(NotifyEvent.Notify))
			{
				AllowReguardNotify->OnAnimNotifyAllowNextCombo.Unbind();
			}
		}
	}
}

void UGA_Guard::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	AbilityReset();

	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}

void UGA_Guard::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	AbilityReset();

	if (UAbilitySystemComponent* ASC = CurrentActorInfo->AbilitySystemComponent.Get())
	{
		if (!ASC->HasMatchingGameplayTag(FBTAG_CHARACTER_ABILITY_GUARD_COUNTER) && !bWasCancelled)
		{
			RestoreIMCFromCounter();
		}
	}
	bCounterIMCActive = false;

	if (UFBAbilitySystemComponent* ASC = Cast<UFBAbilitySystemComponent>(GetAbilitySystemComponentFromActorInfo()))
	{
		ASC->StartGuardGaugeRecovery();
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_Guard::AbilityReset()
{
	ResetGlobalTimeDilation();

	bAllowReGuard = false;
	bChangeNormalGuard = false;

	UnbindDecreaseApplyDamage(CurrentActorInfo);

	ApplyTagEffect(RemoveParryTagEffectClass);
}

void UGA_Guard::UnbindDecreaseApplyDamage(const FGameplayAbilityActorInfo* ActorInfo)
{
	UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();
	if (ASC)
	{
		if (const UFBHealthSet* HealthSet = ASC->GetSet<UFBHealthSet>())
		{
			if (HealthSet->OnCalculateDamageReduction.IsBound())
			{
				HealthSet->OnCalculateDamageReduction.Unbind();
			}
		}
	}
}
