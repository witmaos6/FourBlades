// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AbilityTask/AbilityTask_Trace.h"
#include "AbilitySystem/TargetActor/TargetActor_Sweep.h"
#include "AbilitySystemComponent.h"

UAbilityTask_Trace::UAbilityTask_Trace()
{
}

UAbilityTask_Trace* UAbilityTask_Trace::CreateTask(UGameplayAbility* OwningAbility, TSubclassOf<ATargetActor_Sweep> TargetActorClass)
{
	UAbilityTask_Trace* NewTask = NewAbilityTask<UAbilityTask_Trace>(OwningAbility);
	NewTask->TargetActor = TargetActorClass;
	return NewTask;
}

void UAbilityTask_Trace::Activate()
{
	Super::Activate();

	SpawnAndInitializeTargetActor();
	FinalizeTargetActor();

	SetWaitingOnAvatar();
}

void UAbilityTask_Trace::SpawnAndInitializeTargetActor()
{
	SpawnedTargetActor = Cast<ATargetActor_Sweep>(Ability->GetWorld()->SpawnActorDeferred<AGameplayAbilityTargetActor>(TargetActor, FTransform::Identity, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn));

	if (SpawnedTargetActor)
	{
		SpawnedTargetActor->TargetDataReadyDelegate.AddUObject(this, &UAbilityTask_Trace::OnTargetDataReadyCallback);
	}
}

void UAbilityTask_Trace::FinalizeTargetActor()
{
	UAbilitySystemComponent* ASC = AbilitySystemComponent.Get();
	if (ASC)
	{
		const FTransform SpawnTransform = ASC->GetAvatarActor()->GetTransform();
		SpawnedTargetActor->FinishSpawning(SpawnTransform);

		ASC->SpawnedTargetActors.Push(SpawnedTargetActor);
		SpawnedTargetActor->StartTargeting(Ability);
		SpawnedTargetActor->ConfirmTargeting();
	}
}

void UAbilityTask_Trace::OnTargetDataReadyCallback(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
{
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		OnComplete.Broadcast(TargetDataHandle);
	}
	EndTask();
}

void UAbilityTask_Trace::OnDestroy(bool AbilityEnded)
{
	if (SpawnedTargetActor)
	{
		SpawnedTargetActor->Destroy();
	}

	Super::OnDestroy(AbilityEnded);
}
