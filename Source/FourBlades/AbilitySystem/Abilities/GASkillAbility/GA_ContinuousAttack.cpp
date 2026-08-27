// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/GASkillAbility/GA_ContinuousAttack.h"
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
#include "Kismet/GameplayStatics.h"
#include "AbilityFragment/FBSkillDamageValue.h"
#include "FourBlades.h"

UGA_ContinuousAttack::UGA_ContinuousAttack()
{
	RequiredTagsAnyOne.AddTag(FBTAG_CHARACTER_STATE_ATTACK);
	RequiredTagsAnyOne.AddTag(FBTAG_CHARACTER_STATE_SPRINT);
	// BP에서 AbilityTag 설정 

	// BP에서 ActivationOwnedTags 설정
	// ActivationOwnedTags.LastTag를 이용하여 블록

	// ActivationBlockedTags 설정

	ContinuousDirection = EContinuousDirection::NONE;
}

void UGA_ContinuousAttack::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
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

void UGA_ContinuousAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	const float AnimPlayRate = 1.3f;
	FName StartSectionName = GetNextSection();

	UAbilityTask_PlayMontageAndWait* PlayAttackTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("Attack"), AttackMontage, AnimPlayRate, StartSectionName);
	PlayAttackTask->OnCompleted.AddDynamic(this, &UGA_ContinuousAttack::OnCompleteCallback);
	PlayAttackTask->OnBlendOut.AddDynamic(this, &UGA_ContinuousAttack::OnBlendOut);
	PlayAttackTask->OnInterrupted.AddDynamic(this, &UGA_ContinuousAttack::OnInterruptedCallback);
	PlayAttackTask->ReadyForActivation();

	CheckClash();

	bAllowAttack = false;
}

FName UGA_ContinuousAttack::GetNextSection()
{
	UFBAbilitySystemComponent* ASC = Cast<UFBAbilitySystemComponent>(GetAbilitySystemComponentFromActorInfo());
	if (ASC && ASC->AbilitySystemData)
	{
		EContinuousDirection PrevDirection = ASC->AbilitySystemData->GetPrevDirection();
		CurrentCombo = ASC->AbilitySystemData->GetDirectionCombo(ContinuousDirection);
		uint8 ReverseCombo = ASC->AbilitySystemData->GetReverseDirectionCombo(ContinuousDirection);

		if (PrevDirection == EContinuousDirection::NONE)
		{
			CurrentCombo = FMath::Clamp(CurrentCombo + 1, 1, MaxComboCount);
			ASC->AbilitySystemData->SetContinuousCombo(ContinuousDirection, CurrentCombo);
		}
		else if (PrevDirection == ContinuousDirection)
		{
			if (CurrentCombo == 2)
			{
				CurrentCombo = 1;
			}
			else
			{
				CurrentCombo = FMath::Clamp(CurrentCombo + 1, 1, MaxComboCount);
			}
			ASC->AbilitySystemData->ResetContinuousCombo();
		}
		else if (PrevDirection != ContinuousDirection)
		{
			if (ReverseCombo == 2 && CurrentCombo == 2)
			{
				CurrentCombo = FMath::Clamp(CurrentCombo + 1, 1, MaxComboCount);
				ASC->AbilitySystemData->ResetContinuousCombo();
			}
			else
			{
				CurrentCombo = FMath::Clamp(CurrentCombo + 1, 1, MaxComboCount);
				ASC->AbilitySystemData->SetContinuousCombo(ContinuousDirection, CurrentCombo);
			}
		}
		FName NextSection = *FString::Printf(TEXT("%s%d"), *FString("Attack"), CurrentCombo);
		return NextSection;
	}
	return FName();
}

void UGA_ContinuousAttack::InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
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

void UGA_ContinuousAttack::CheckClash()
{
	if (UFBAbilitySystemComponent* OwnerASC = Cast<UFBAbilitySystemComponent>(GetAbilitySystemComponentFromActorInfo()))
	{
		bool bHasClashTag = OwnerASC->HasMatchingGameplayTag(FBTAG_CHARACTER_ABILITY_CONTINUOUSATTACK_CLASH);
		if (bHasClashTag)
		{
			SuccessClash();
		}

		if (UAbilitySystemData* ASD = OwnerASC->AbilitySystemData)
		{
			float ComboTimeOutTime = bHasClashTag ? 1.5f : 1.0f;
			ASD->StartContinuousComboTimeout(ComboTimeOutTime);
		}
	}
}

void UGA_ContinuousAttack::SuccessClash()
{
	bClashSuccess = true;

	ApplyTagEffect(ResistKnockback);

	ApplyTemporaryTimeDilation(0.4f, 0.2f);

	ApplyInstigatorClash();
}

void UGA_ContinuousAttack::ApplyInstigatorClash()
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (ASC == nullptr)
	{
		return;
	}

	FGameplayEffectQuery Query;
	Query.EffectDefinition = ClashEffect;

	TArray<FActiveGameplayEffectHandle> ActiveEffects = ASC->GetActiveEffects(Query);

	if (ActiveEffects.Num() > 0)
	{
		if (const FActiveGameplayEffect* Effect = ASC->GetActiveGameplayEffect(ActiveEffects[0]))
		{
			if (AActor* Instigator = Effect->Spec.GetEffectContext().GetInstigator())
			{
				FGameplayEffectContextHandle EffectContext = GetAbilitySystemComponentFromActorInfo()->MakeEffectContext();
				EffectContext.AddInstigator(GetOwningActorFromActorInfo(), GetAvatarActorFromActorInfo());

				FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(ClashSuccessEffect);
				if (SpecHandle.IsValid())
				{
					GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Instigator));
					ASC->RemoveActiveGameplayEffect(ActiveEffects[0]);
				}
			}
		}
	}
}

void UGA_ContinuousAttack::JumpToSection()
{
	if (bInput)
	{
		GetAbilitySystemComponentFromActorInfo()->AddLooseGameplayTag(ActivationOwnedTags.Last());

		MontageJumpToSection(GetNextSection());
		bInput = false;
		bAllowAttack = false;

		CheckClash();
	}
}

void UGA_ContinuousAttack::AnimNotifyAttackHitCheck()
{
	UAbilityTask_Trace* AttackTask = UAbilityTask_Trace::CreateTask(this, TargetActorClass);
	AttackTask->OnComplete.AddDynamic(this, &UGA_ContinuousAttack::OnTraceResultCallback);
	AttackTask->ReadyForActivation();
}

void UGA_ContinuousAttack::AnimNotifyAllowNextCombo()
{
	bAllowAttack = true;

	GetAbilitySystemComponentFromActorInfo()->RemoveLooseGameplayTag(ActivationOwnedTags.Last());
}

void UGA_ContinuousAttack::OnTraceResultCallback(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
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
				if (!bClashSuccess && AttackDamages->LaunchInfos.IsValidIndex(CurrentCombo - 1))
				{
					LaunchPower = AttackDamages->LaunchInfos[CurrentCombo - 1].LaunchPower;
					ReactDuration = AttackDamages->LaunchInfos[CurrentCombo - 1].ReactDuration;
				}
				FFBTargetPayload TargetPayload = FFBTargetPayload(CurrentSkillDamageValue.AttackType, ReactDuration, LaunchPower);

				DamageContext->SetDamageExecutionParams(MoveTemp(DamageExecutionParams));
				DamageContext->SetTargetPayload(MoveTemp(TargetPayload));

				ApplyGameplayEffectSpecToTarget(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, EffectSpecHandle, TargetDataHandle);
			}

			if (ClashSuccessGameplayCue.IsValid() && bClashSuccess)
			{
				SourceASC->ExecuteGameplayCue(ClashSuccessGameplayCue);
			}
			bClashSuccess = false;

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

void UGA_ContinuousAttack::OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
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

void UGA_ContinuousAttack::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	AbilityReset();

	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}

void UGA_ContinuousAttack::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	AbilityReset();

	UFBAbilitySystemComponent* ASC = Cast<UFBAbilitySystemComponent>(GetAbilitySystemComponentFromActorInfo());
	if (ASC && ASC->AbilitySystemData)
	{
		if (!bWasCancelled)
		{
			ASC->AbilitySystemData->ResetContinuousCombo();
		}
		ASC->AbilitySystemData->StartContinuousComboTimeout();
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_ContinuousAttack::AbilityReset()
{
	CurrentCombo = 0;
	bAllowAttack = true;
	bInput = false;

	ResetGlobalTimeDilation();
}