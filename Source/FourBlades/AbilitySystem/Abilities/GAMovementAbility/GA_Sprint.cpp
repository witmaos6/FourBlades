// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/GAMovementAbility/GA_Sprint.h"
#include "Character/FBCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AbilitySystemComponent.h"
#include "GameplayTags/FBDefineGameplayTags.h"

UGA_Sprint::UGA_Sprint()
{
	OriginSpeed = -1.f;
	IncreasingSpeed = 400.f;

	CancelStateTags.AddTag(FBTAG_CHARACTER_STATE_ATTACK);
	CancelStateTags.AddTag(FBTAG_CHARACTER_STATE_GUARD);
}

void UGA_Sprint::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (AFBCharacter* Character = Cast<AFBCharacter>(GetAvatarActorFromActorInfo()))
	{
		if (UCharacterMovementComponent* CharacterMovement = Character->GetCharacterMovement())
		{
			OriginSpeed = CharacterMovement->MaxWalkSpeed;
			float NewSpeed = OriginSpeed + IncreasingSpeed;

			CharacterMovement->MaxWalkSpeed = NewSpeed;

			GetWorld()->GetTimerManager().SetTimer(StopCheckTimer, this, &UGA_Sprint::CheckIfStopped, 0.2f, true);
		}
	}

	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
	{
		for (const FGameplayTag& Tag : CancelStateTags)
		{
			FDelegateHandle DelegateHandle = ASC->RegisterGameplayTagEvent(Tag, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &UGA_Sprint::OnCancelTagChanged);

			CancelStateTagEventHandles.Add(Tag, DelegateHandle);
		}
	}
}

void UGA_Sprint::CheckIfStopped()
{
	if (AFBCharacter* Character = Cast<AFBCharacter>(GetAvatarActorFromActorInfo()))
	{
		float CurrentVelocity = Character->GetVelocity().Size2D();

		if (CurrentVelocity < 5.0f)
		{
			EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, true);
		}
	}
}

void UGA_Sprint::OnCancelTagChanged(const FGameplayTag Tag, int32 NewCount)
{
	if (NewCount > 0)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, true);
	}
}

void UGA_Sprint::InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	EndAbility(Handle, ActorInfo, ActivationInfo, false, false);
}

void UGA_Sprint::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	AbilityReset();

	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}

void UGA_Sprint::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	AbilityReset();

	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
	{
		for (const auto& Pair : CancelStateTagEventHandles)
		{
			ASC->UnregisterGameplayTagEvent(Pair.Value, Pair.Key);
		}
		CancelStateTagEventHandles.Empty();
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_Sprint::AbilityReset()
{
	if (AFBCharacter* Character = Cast<AFBCharacter>(GetAvatarActorFromActorInfo()))
	{
		if (UCharacterMovementComponent* CharacterMovement = Character->GetCharacterMovement())
		{
			if (OriginSpeed != -1.0f)
			{
				CharacterMovement->MaxWalkSpeed = OriginSpeed;
			}
		}
	}

	if (GetWorld()->GetTimerManager().IsTimerActive(StopCheckTimer))
	{
		GetWorld()->GetTimerManager().ClearTimer(StopCheckTimer);
	}
}
