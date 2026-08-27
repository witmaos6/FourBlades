// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AbilityTask/AbilityTask_SpawnActorDeferred.h"
#include "AbilityTask_SpawnActorDeferred.h"

UAbilityTask_SpawnActorDeferred* UAbilityTask_SpawnActorDeferred::CreateSpawnActorTask(UGameplayAbility* OwningAbility, TSubclassOf<AActor> Class)
{
	UAbilityTask_SpawnActorDeferred* NewTask = NewAbilityTask<UAbilityTask_SpawnActorDeferred>(OwningAbility);
	NewTask->SpawnActorClass = Class;
	return NewTask;
}

bool UAbilityTask_SpawnActorDeferred::BeginSpawningActor(UGameplayAbility* OwningAbility, AActor*& SpawnedActor, const FTransform& SpawnTransform)
{
	if (Ability) // Ability->GetCurrentActorInfo()->IsNetAuthority() && ShouldBroadcastAbilityTaskDelegates() 조건 삭제 
	{
		UWorld* const World = GEngine->GetWorldFromContextObject(OwningAbility, EGetWorldErrorMode::LogAndReturnNull);
		AActor* OwnerActor = OwningAbility->GetOwningActorFromActorInfo();
		APawn* Instigator = Cast<APawn>(OwningAbility->GetAvatarActorFromActorInfo());
		if (World && SpawnActorClass)
		{
			if (OwnerActor && Instigator)
			{
				SpawnedActor = World->SpawnActorDeferred<AActor>(SpawnActorClass, SpawnTransform, OwnerActor, Instigator, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
			}
			else
			{
				SpawnedActor = World->SpawnActorDeferred<AActor>(SpawnActorClass, SpawnTransform, NULL, NULL, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
			}
		}
	}

	if (SpawnedActor == nullptr)
	{
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			DidNotSpawn.Broadcast(nullptr);
		}
		return false;
	}

	return true;
}

void UAbilityTask_SpawnActorDeferred::FinishSpawningActor(UGameplayAbility* OwningAbility, AActor* SpawnedActor, const FTransform SpawnTransform)
{
	if (SpawnedActor)
	{
		SpawnedActor->FinishSpawning(SpawnTransform);

		if (ShouldBroadcastAbilityTaskDelegates())
		{
			Success.Broadcast(SpawnedActor);
		}
	}

	EndTask();
}
