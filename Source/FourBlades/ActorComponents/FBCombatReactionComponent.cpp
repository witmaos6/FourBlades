// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorComponents/FBCombatReactionComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/Attributes/FBHealthSet.h"
#include "AbilitySystem/Attributes/FBCombatSet.h"
#include "GameplayTags/FBDefineGameplayTags.h"
#include "LaunchData.h"
#include "ReactData.h"
#include "Character/FBCharacter.h"
#include "GameModes/FBGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "FourBlades.h"

UFBCombatReactionComponent::UFBCombatReactionComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;

	FlinchMaxCount = 5;
	FlinchCount = 0;
}

void UFBCombatReactionComponent::BeginPlay()
{
	Super::BeginPlay();

	if (!HealthSetBind())
	{
		GetWorld()->GetTimerManager().SetTimer(RetryBindTimerHandle, this, &UFBCombatReactionComponent::RetryBind, 0.1f, true);
	}

	if (APawn* Pawn = Cast<APawn>(GetOwner()))
	{
		const bool bIsPlayerCharacter = Pawn->IsA<AFBCharacter>();
		if (bIsPlayerCharacter)
		{
			if (AFBGameModeBase* GameMode = Cast<AFBGameModeBase>(UGameplayStatics::GetGameMode(GetOwner())))
			{
				GameMode->OnPlayerResurrectionDelegate.AddUObject(this, &UFBCombatReactionComponent::Resurrection);
			}
		}
	}
}

void UFBCombatReactionComponent::RetryBind()
{
	if (HealthSetBind())
	{
		GetWorld()->GetTimerManager().ClearTimer(RetryBindTimerHandle);
	}
}

bool UFBCombatReactionComponent::HealthSetBind()
{
	if (APawn* Pawn = Cast<APawn>(GetOwner()))
	{
		if (UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Pawn))
		{
			if (const UFBHealthSet* HealthSet = ASC->GetSet<UFBHealthSet>())
			{
				HealthSet->OnApplyDamageToHealth.AddUObject(this, &UFBCombatReactionComponent::OnReaction);

				return true;
			}
		}
	}
	return false;
}

void UFBCombatReactionComponent::OnReaction(AActor* CurrentInstigator, AActor* CurrentEffectCauser, const FFBTargetPayload& TargetPayload)
{
	if (TargetPayload.AttackType == FBTAG_ATTACKTYPE_ATTACK_NORMAL)
		return;

	APawn* Pawn = Cast<APawn>(GetOwner());
	if (Pawn == nullptr)
	{
		return;
	}
	UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Pawn);
	if (ASC == nullptr)
	{
		return;
	}

	CheckReaction(ASC, TargetPayload);

	if (TargetPayload.LaunchStrength > 0.0f)
	{
		ULaunchData* LaunchData = NewObject<ULaunchData>();
		LaunchData->Direction = GetLaunchDirection(CurrentEffectCauser, TargetPayload);
		LaunchData->Distance = TargetPayload.LaunchStrength;
		// To do: TargetPayload에 LaunchDuration값 추가시 LaunchData->Duration 추가
		if (ASC->HasMatchingGameplayTag(FBTAG_ATTACKTYPE_RESIST_LAUNCH))
		{
			if (const UFBCombatSet* CombatSet = ASC->GetSet<UFBCombatSet>())
			{
				float ResistLaunchValue = CombatSet->GetResistLaunch();
				float ResistMultiplier = (1.0f - FMath::Clamp(ResistLaunchValue, 0.0f, 1.0f));
				LaunchData->Distance *= ResistMultiplier;
			}
		}
		FGameplayEventData LaunchEventData;
		LaunchEventData.OptionalObject = LaunchData;

		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Pawn, FBTAG_SHAREDABILITY_LAUNCH, LaunchEventData);
	}
}

void UFBCombatReactionComponent::CheckReaction(UAbilitySystemComponent* OwnerASC, const FFBTargetPayload& TargetPayload)
{
	if (OwnerASC == nullptr)
		return;

	if (TargetPayload.AttackType == FBTAG_ATTACKTYPE_ATTACK_FLINCH)
	{
		if (!OwnerASC->HasMatchingGameplayTag(FBTAG_ATTACKTYPE_RESIST_FLINCH) && !OwnerASC->HasMatchingGameplayTag(FBTAG_ATTACKTYPE_RESIST_KNOCKBACK))
		{
			FGameplayTagContainer CancelTags(FBTAG_SHAREDABILITY_FLINCH);
			OwnerASC->CancelAbilities(&CancelTags);

			ActivateReaction(FBTAG_SHAREDABILITY_FLINCH, TargetPayload.ReactDurationTime);

			FlinchCount++;
			if (FlinchCount >= FlinchMaxCount)
			{
				ApplyResistEffect(OwnerASC, ResistFlinchLaunch);
			}
		}
	}
	else if (TargetPayload.AttackType == FBTAG_ATTACKTYPE_ATTACK_KNOCKBACK)
	{
		if (!OwnerASC->HasMatchingGameplayTag(FBTAG_ATTACKTYPE_RESIST_KNOCKBACK))
		{
			ActivateReaction(FBTAG_SHAREDABILITY_KNOCKBACK, TargetPayload.ReactDurationTime);

			ApplyResistEffect(OwnerASC, ResistKnockbackLaunch);
		}
	}
}

void UFBCombatReactionComponent::ActivateReaction(FGameplayTag ActivateReaction, float ReactDurationTime)
{
	UReactData* ReactData = NewObject<UReactData>();
	ReactData->ReactDurationTime = ReactDurationTime;

	FGameplayEventData ReactEventData;
	ReactEventData.OptionalObject = ReactData;

	if (APawn* Pawn = Cast<APawn>(GetOwner()))
	{
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Pawn, ActivateReaction, ReactEventData);
	}
}

void UFBCombatReactionComponent::ApplyResistEffect(UAbilitySystemComponent* OwnerASC, TSubclassOf<UGameplayEffect> ResistEffect)
{
	if (OwnerASC == nullptr)
		return;

	FGameplayEffectContextHandle EffectContextHandle = OwnerASC->MakeEffectContext();
	FGameplayEffectSpecHandle EffectSpecHandle = OwnerASC->MakeOutgoingSpec(ResistEffect, 1.0f, EffectContextHandle);
	ResistEffectHandle = OwnerASC->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data);

	if (ResistEffectHandle.IsValid())
	{
		FOnActiveGameplayEffectRemoved_Info* RemovedDelegate = OwnerASC->OnGameplayEffectRemoved_InfoDelegate(ResistEffectHandle);
		if (RemovedDelegate)
		{
			ResistRemovedDelegateHandle = RemovedDelegate->AddUObject(this, &UFBCombatReactionComponent::OnResistEffectRemoved);
		}
	}
}

void UFBCombatReactionComponent::OnResistEffectRemoved(const FGameplayEffectRemovalInfo& RemovedEffect)
{
	FlinchCount = 0;
}

FVector UFBCombatReactionComponent::GetLaunchDirection(AActor* EffectCauser, const FFBTargetPayload& TargetPayload)
{
	if (EffectCauser == nullptr)
	{
		FB_FUNC_LOG(LogTemp, Warning, TEXT("Effect Causer is null"));
		return FVector::ZeroVector;
	}

	APawn* Pawn = Cast<APawn>(GetOwner());
	if (Pawn == nullptr)
	{
		return FVector::ZeroVector;
	}

	if (TargetPayload.DirectionType == EAttackedLaunchDirectionType::TowardTarget)
	{
		return (Pawn->GetActorLocation() - EffectCauser->GetActorLocation()).GetSafeNormal();
	}
	else if (TargetPayload.DirectionType == EAttackedLaunchDirectionType::AttackerForward)
	{
		FVector EffectCauserForwardVector = EffectCauser->GetActorForwardVector();

		FVector DirectionToTarget = (Pawn->GetActorLocation() - EffectCauser->GetActorLocation()).GetSafeNormal();
		float PitchAngle = FMath::Atan2(DirectionToTarget.Z, FVector(DirectionToTarget.X, DirectionToTarget.Y, 0.0f).Size());

		FVector HorizontalDirection = FVector(EffectCauserForwardVector.X, EffectCauserForwardVector.Y, 0.0f).GetSafeNormal();

		FVector LaunchDirection = HorizontalDirection.RotateAngleAxis(FMath::RadiansToDegrees(PitchAngle), FVector::CrossProduct(HorizontalDirection, FVector::UpVector));
		return LaunchDirection;
	}
	else if (TargetPayload.DirectionType == EAttackedLaunchDirectionType::SelfBack)
	{
		return -Pawn->GetActorForwardVector();
	}

	FB_FUNC_LOG(LogTemp, Warning, TEXT("DirectionType is invalid"));
	return FVector::ZeroVector;
}

void UFBCombatReactionComponent::Resurrection()
{
	if (!HealthSetBind())
	{
		GetWorld()->GetTimerManager().SetTimer(RetryBindTimerHandle, this, &UFBCombatReactionComponent::RetryBind, 0.1f, true);
	}
}