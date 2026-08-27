// Fill out your copyright notice in the Description page of Project Settings.


#include "FBGameplayAbility.h"
#include "Character/FBCharacterBase.h"
#include "AbilitySystemComponent.h"
#include "GameplayTags/FBDefineGameplayTags.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/FBPlayerState.h"
#include "AbilityFragment/FBAbilityFragmentManager.h"
#include "AbilitySystem/FBAbilitySet.h"
#include "Kismet/GameplayStatics.h"
#include "FourBlades.h"

UFBGameplayAbility::UFBGameplayAbility(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	ActivationPolicy = EFBAbilityActivationPolicy::OnInputTriggered;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	RequiredTagsAnyOne.AddTag(FBTAG_CHARACTER_STATE_NORMAL);
}

bool UFBGameplayAbility::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	if (RequiredTagsAnyOne.IsEmpty())
	{
		return true;
	}

	if (ActorInfo->AbilitySystemComponent.IsValid())
	{
		return ActorInfo->AbilitySystemComponent->HasAnyMatchingGameplayTags(RequiredTagsAnyOne);
	}
	
	return false;
}

void UFBGameplayAbility::SetRotationAvatarToLastMoveDirection()
{
	if (AFBCharacterBase* OwningActor = CastChecked<AFBCharacterBase>(CurrentActorInfo->AvatarActor.Get()))
	{
		FVector LastInputDirection = OwningActor->GetLastMovementInputVector();
		if (!LastInputDirection.IsZero())
		{
			FRotator CharacterRotation = LastInputDirection.Rotation();
			OwningActor->SetActorRotation(CharacterRotation);
		}
	}
}

void UFBGameplayAbility::SetMovementMode(EMovementMode InMovementMode)
{
	if (ACharacter* Character = Cast<ACharacter>(CurrentActorInfo->AvatarActor.Get()))
	{
		if (UCharacterMovementComponent* CharacterMovementComponent = Character->GetCharacterMovement())
		{
			CharacterMovementComponent->SetMovementMode(InMovementMode);
		}
	}
}

FTransform UFBGameplayAbility::GetOwnerSocketTransform(const FName SocketName)
{
	FTransform Transform = FTransform::Identity;
	ACharacter* Owner = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	if (Owner == nullptr)
	{
		FB_FUNC_LOG(LogTemp, Warning, TEXT("Owner is nullptr"));
		return Transform;
	}
	USkeletalMeshComponent* MeshComp = Owner->GetMesh();
	if (MeshComp == nullptr)
	{
		FB_FUNC_LOG(LogTemp, Warning, TEXT("MeshComponent is nullptr"));
		return Transform;
	}

	if (!MeshComp->DoesSocketExist(SocketName))
	{
		FB_FUNC_LOG(LogTemp, Warning, TEXT("%s not found"), *SocketName.ToString());
		return Transform;
	}

	Transform = MeshComp->GetSocketTransform(SocketName);
	return Transform;
}

APlayerState* UFBGameplayAbility::GetOwnerPlayerState()
{
	if (APlayerState* PS = Cast<APlayerState>(GetOwningActorFromActorInfo()))
	{
		return PS;
	}
	return nullptr;
}

UFBAbilityFragmentManager* UFBGameplayAbility::GetAbilityFragmentManager(const FGameplayTag InAbilityTag)
{
	if (AFBPlayerState* PS = Cast<AFBPlayerState>(GetOwnerPlayerState()))
	{
		if (UFBAbilitySet* AbilitySet = PS->GetAbilitySet())
		{
			return AbilitySet->GetAbilityFragmentManager(InAbilityTag);
		}
	}
	return nullptr;
}

const UFBAbilityFragmentManager* UFBGameplayAbility::GetAbilityFragmentManager(const FGameplayAbilitySpec& Spec)
{
	if (Spec.GameplayEventData.IsValid())
	{
		if (IsValid(Spec.GameplayEventData->OptionalObject))
		{
			if (const UFBAbilityFragmentManager* FragmentManager = Cast<UFBAbilityFragmentManager>(Spec.GameplayEventData->OptionalObject))
			{
				return FragmentManager;
			}
		}
	}
	return nullptr;
}

void UFBGameplayAbility::ApplyTemporaryTimeDilation(float TimeScale, float DurationInRealSeconds)
{
	UWorld* World = GetWorld();
	if (World == nullptr)
	{
		return;
	}

	TimeScale = FMath::Clamp(TimeScale, 0.01f, 1.0f);
	DurationInRealSeconds = FMath::Max(DurationInRealSeconds, 0.0f);

	UGameplayStatics::SetGlobalTimeDilation(World, TimeScale);

	float RealResetTime = DurationInRealSeconds * TimeScale;
	FTimerHandle ResetTimer;
	World->GetTimerManager().SetTimer(ResetTimer, this, &UFBGameplayAbility::ResetGlobalTimeDilation, RealResetTime, false);
}

void UFBGameplayAbility::ResetGlobalTimeDilation()
{
	if (UWorld* World = GetWorld())
	{
		UGameplayStatics::SetGlobalTimeDilation(World, 1.0f);
	}
}

void UFBGameplayAbility::OnCompleteCallback()
{
	bool bReplicatedEndAbility = false;
	bool bWasCancelled = false;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicatedEndAbility, bWasCancelled);
}

void UFBGameplayAbility::OnBlendOut()
{
	bool bReplicatedEndAbility = false;
	bool bWasCancelled = true;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicatedEndAbility, bWasCancelled);
}

void UFBGameplayAbility::OnInterruptedCallback()
{
	bool bReplicatedEndAbility = false;
	bool bWasCancelled = true;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicatedEndAbility, bWasCancelled);
}

void UFBGameplayAbility::ApplyTagEffect(TSubclassOf<UGameplayEffect>& EffectClass)
{
	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
	{
		FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(EffectClass);
		ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);
	}
}
