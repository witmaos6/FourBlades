// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Abilities/GA_NoneStopAttack.h"
#include "Animation/FBAnimNotify_AttackHitCheck.h"
#include "Animation/FBAnimNotify_ClashWindow.h"
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
#include "FourBlades.h"

UGA_NoneStopAttack::UGA_NoneStopAttack(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	AbilityTags.AddTag(FBTAG_ENEMY_ABILITY_ATTACK);

	ActivationOwnedTags.AddTag(FBTAG_ENEMY_STATE_ATTACK);

	AbilityConditionType = EAbilityConditionType::MeleeForwardAttack;

	ValidDistance = 0.0f;
	ValidAngle = 0.0f;
}

void UGA_NoneStopAttack::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
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
		}
		
		ValidDistance = EnemyDataInterface->GetAbilityActivateMinRange(AbilityConditionType);
		ValidAngle = EnemyDataInterface->GetAbilityActivateValidRadius(AbilityConditionType);
	}

	if (AttackMontage)
	{
		const TArray<FAnimNotifyEvent>& NotifyEvents = AttackMontage->Notifies;
		for (const FAnimNotifyEvent& NotifyEvent : NotifyEvents)
		{
			if (UFBAnimNotify_AttackHitCheck* AttackHitCheck = Cast<UFBAnimNotify_AttackHitCheck>(NotifyEvent.Notify))
			{
				AttackHitCheck->OnAnimNotifyAttackHitCheck.BindUObject(this, &UGA_NoneStopAttack::AnimNotifyAttackHitCheck);
			}
			if (UFBAnimNotify_ClashWindow* ClashWindowCheck = Cast<UFBAnimNotify_ClashWindow>(NotifyEvent.Notify))
			{
				ClashWindowCheck->OnAnimNotifyClashWindow.BindUObject(this, &UGA_NoneStopAttack::AnimNotifyClashWindow);
			}
		}
	}
}

void UGA_NoneStopAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	const float AnimPlayRate = 1.0f;

	UAbilityTask_PlayMontageAndWait* PlayAttackTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("Attack"), AttackMontage, AnimPlayRate);

	PlayAttackTask->OnCompleted.AddDynamic(this, &UGA_NoneStopAttack::OnCompleteCallback);
	PlayAttackTask->OnInterrupted.AddDynamic(this, &UGA_NoneStopAttack::OnInterruptedCallback);
	PlayAttackTask->ReadyForActivation();

	if (ResistLaunchEffect)
	{
		if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
		{
			FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(ResistLaunchEffect);
			if (SpecHandle.IsValid() && SpecHandle.Data.IsValid())
			{
				ResistLaunchEffectHandle = ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);
			}
		}
	}
}

void UGA_NoneStopAttack::CurrentAttackCheck()
{
	CurrentAttack = FMath::Clamp(CurrentAttack + 1, 1, AttackDamages->SkillDamageValues.Num());
}

void UGA_NoneStopAttack::AnimNotifyAttackHitCheck()
{	
	UAbilityTask_Trace* AttackTask = UAbilityTask_Trace::CreateTask(this, TargetActorClass);
	AttackTask->OnComplete.AddDynamic(this, &UGA_NoneStopAttack::OnTraceResultCallback);
	AttackTask->ReadyForActivation();
}

void UGA_NoneStopAttack::AnimNotifyClashWindow()
{
	// To do: 공격판정과 일치시키기 위해 Trace 방식으로 변경할 수 있음
	if (APawn* TargetPawn = GetCombatTargetPawn())
	{
		if (AActor* AvataActor = GetAvatarActorFromActorInfo())
		{
			bool bValidDistance = UFBDetectionUtilityLibrary::InDistanceFromActor(AvataActor, TargetPawn, ValidDistance * 2.f);
			bool bValidAngle = UFBDetectionUtilityLibrary::IsTargetInSectorFromActor(AvataActor, TargetPawn, ValidAngle * 3.f);

			if (bValidDistance && bValidAngle)
			{
				if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetPawn))
				{
					FGameplayEffectContextHandle EffectContext = GetAbilitySystemComponentFromActorInfo()->MakeEffectContext();
					EffectContext.AddInstigator(AvataActor, AvataActor);

					FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(ClashEffect);
					if (SpecHandle.IsValid())
					{
						// To do: 즉시 태그를 부착하는 방식이 아닌 신호 전달 후 플레이어 쪽에서 태그를 부여하는 방식으로 변경할 수 있음
						GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);
					}
				}
			}
		}
	}
}

void UGA_NoneStopAttack::OnTraceResultCallback(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
{
	AActor* OwningActor = CurrentActorInfo->AvatarActor.Get();

	if (UAbilitySystemBlueprintLibrary::TargetDataHasActor(TargetDataHandle, 0))
	{
		FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingGameplayEffectSpec(DamageEffect);
		FGameplayEffectContextHandle DamageEffectContextHandle = UAbilitySystemBlueprintLibrary::GetEffectContext(EffectSpecHandle);
		FFBGameplayEffectContext* DamageContext = FFBGameplayEffectContext::ExtractEffectContext(DamageEffectContextHandle);

		CurrentAttackCheck();

		if (EffectSpecHandle.IsValid() && DamageContext)
		{
			if (AttackDamages->SkillDamageValues.IsValidIndex(CurrentAttack - 1))
			{
				bool bIsClash = IsClash();

				const FFBSkillDamageValue& CurrentSkillDamageValue = AttackDamages->SkillDamageValues[CurrentAttack - 1];
				float DamageCoefficient = bIsClash ? 0.0f : CurrentSkillDamageValue.DamageCoefficient;
				float AdditionalFixedDamage = bIsClash ? 0.0f : CurrentSkillDamageValue.AdditionalFixedDamage;

				FFBDamageExecutionParams DamageExecutionParams = FFBDamageExecutionParams(DamageCoefficient, AdditionalFixedDamage);

				float LaunchPower = 0.0f;
				float ReactDuration = 0.0f;
				if (AttackDamages->LaunchInfos.IsValidIndex(CurrentAttack - 1))
				{
					LaunchPower = AttackDamages->LaunchInfos[CurrentAttack - 1].LaunchPower;
					ReactDuration = AttackDamages->LaunchInfos[CurrentAttack - 1].ReactDuration;
				}
				FFBTargetPayload TargetPayload = FFBTargetPayload(CurrentSkillDamageValue.AttackType, ReactDuration, LaunchPower);

				DamageContext->SetDamageExecutionParams(MoveTemp(DamageExecutionParams));
				DamageContext->SetTargetPayload(MoveTemp(TargetPayload));

				ApplyGameplayEffectSpecToTarget(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, EffectSpecHandle, TargetDataHandle);

				if (bIsClash)
				{
					SelfLaunch();
				}
			}
		}
	}
}

bool UGA_NoneStopAttack::IsClash()
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (ASC == nullptr)
	{
		FB_FUNC_LOG(LogTemp, Warning, TEXT("ASC is null"));
		return false;
	}
	if (ASC->HasMatchingGameplayTag(FBTAG_ENEMY_ABILITY_BLOCK_CLASH) == false)
	{
		return false;
	}

	FGameplayEffectQuery Query;
	Query.EffectDefinition = ClashSuccessEffect;

	TArray<FActiveGameplayEffectHandle> ActiveEffects = ASC->GetActiveEffects(Query);

	if (ActiveEffects.Num() > 0)
	{
		ASC->RemoveActiveGameplayEffect(ActiveEffects[0]);

		FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(DecreaseResistLaunchEffect);
		if (SpecHandle.IsValid() && SpecHandle.Data.IsValid())
		{
			ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);
		}
	}

	return true;
}

void UGA_NoneStopAttack::SelfLaunch()
{
	FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingGameplayEffectSpec(DamageEffect);
	FGameplayEffectContextHandle DamageEffectContextHandle = UAbilitySystemBlueprintLibrary::GetEffectContext(EffectSpecHandle);
	FFBGameplayEffectContext* DamageContext = FFBGameplayEffectContext::ExtractEffectContext(DamageEffectContextHandle);

	float LaunchPower = 0.0f;
	float ReactDuration = 0.0f;
	if (AttackDamages->LaunchInfos.IsValidIndex(CurrentAttack - 1))
	{
		LaunchPower = AttackDamages->LaunchInfos[CurrentAttack - 1].LaunchPower;
		ReactDuration = AttackDamages->LaunchInfos[CurrentAttack - 1].ReactDuration;
	}
	FFBTargetPayload TargetPayload = FFBTargetPayload(FBTAG_ATTACKTYPE_ATTACK_FLINCH, ReactDuration, LaunchPower, EAttackedLaunchDirectionType::SelfBack);

	DamageContext->SetTargetPayload(MoveTemp(TargetPayload));

	ApplyGameplayEffectSpecToOwner(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, EffectSpecHandle);
}

void UGA_NoneStopAttack::OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	const TArray<FAnimNotifyEvent>& NotifyEvents = AttackMontage->Notifies;
	for (const FAnimNotifyEvent& NotifyEvent : NotifyEvents)
	{
		if (UFBAnimNotify_AttackHitCheck* AttackHitCheck = Cast<UFBAnimNotify_AttackHitCheck>(NotifyEvent.Notify))
		{
			AttackHitCheck->OnAnimNotifyAttackHitCheck.Unbind();
		}
		if (UFBAnimNotify_ClashWindow* ClashWindowCheck = Cast<UFBAnimNotify_ClashWindow>(NotifyEvent.Notify))
		{
			ClashWindowCheck->OnAnimNotifyClashWindow.Unbind();
		}
	}
}

void UGA_NoneStopAttack::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
	CurrentAttack = 0;
}

void UGA_NoneStopAttack::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	if (ResistLaunchEffect && ResistLaunchEffectHandle.IsValid())
	{
		if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
		{
			ASC->RemoveActiveGameplayEffect(ResistLaunchEffectHandle);
		}	
	}

	CurrentAttack = 0;
}
