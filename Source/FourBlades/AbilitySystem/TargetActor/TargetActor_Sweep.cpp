// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/TargetActor/TargetActor_Sweep.h"
#include "Abilities/GameplayAbility.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Physics/FBCollisionChannels.h"

ATargetActor_Sweep::ATargetActor_Sweep()
{
	Radius = 100.f;
	Range = 100.f;
	TraceChannel = FB_TraceChannel_PlayerAttack;
}

void ATargetActor_Sweep::StartTargeting(UGameplayAbility* Ability)
{
	Super::StartTargeting(Ability);

	SourceActor = Ability->GetCurrentActorInfo()->AvatarActor.Get();
}

void ATargetActor_Sweep::ConfirmTargetingAndContinue()
{
	if (SourceActor)
	{
		FGameplayAbilityTargetDataHandle DataHandle = MakeTargetData();
		TargetDataReadyDelegate.Broadcast(DataHandle);
	}
}

FGameplayAbilityTargetDataHandle ATargetActor_Sweep::MakeTargetData() const
{
	ACharacter* Character = CastChecked<ACharacter>(SourceActor);
	UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(SourceActor);
	if (!ASC)
	{
		return FGameplayAbilityTargetDataHandle();
	}

	FCollisionQueryParams Params(SCENE_QUERY_STAT(ATargetActor_Sweep), false, Character);

	FVector Forward = Character->GetActorForwardVector();
	const FVector Start = Character->GetActorLocation() + Forward * Character->GetCapsuleComponent()->GetScaledCapsuleRadius();
	const FVector End = Start + Forward * Range;
	const float CapsuleHalfHeight = Range * 0.5f;

	TArray<FHitResult> HitResults;
	GetWorld()->SweepMultiByChannel(HitResults, Start, End, FQuat::Identity, TraceChannel, FCollisionShape::MakeSphere(Radius), Params);

	TArray<TWeakObjectPtr<AActor>> HitActors;
	for (const FHitResult& HitResult : HitResults)
	{
		AActor* HitActor = HitResult.GetActor();
		if (HitActor)
		{
			HitActors.Add(HitActor);
		}
	}

	FGameplayAbilityTargetData_ActorArray* ActorsData = new FGameplayAbilityTargetData_ActorArray();
	ActorsData->SetActors(HitActors);

#if ENABLE_DRAW_DEBUG
	if (bShowDebug)
	{
		FVector CapsuleOrigin = Start + (End - Start) * 0.5f;
		FColor DrawColor = (HitActors.Num() > 0) ? FColor::Green : FColor::Red;
		DrawDebugCapsule(GetWorld(), CapsuleOrigin, CapsuleHalfHeight, Radius, FRotationMatrix::MakeFromZ(Forward).ToQuat(), DrawColor, false, 3.0f);
	}
#endif
	return FGameplayAbilityTargetDataHandle(ActorsData);
}
