// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Abilities/FBEnemyGameplayAbility.h"
#include "GameFramework/Character.h"
#include "AIController.h"
#include "AI/FBAIKey.h"
#include "BehaviorTree/BlackboardComponent.h"

UFBEnemyGameplayAbility::UFBEnemyGameplayAbility(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	RequiredTagsAnyOne.AddTag(FGameplayTag::RequestGameplayTag(FName("Enemy.State.Idle")));
}

AActor* UFBEnemyGameplayAbility::GetCombatTargetActor() const
{
	if (ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo())) // To do: 이 부분은 결합도를 낮추기 위해 바뀔 수 있음
	{
		if (AAIController* AIController = Cast<AAIController>(Character->GetController()))
		{
			if (UBlackboardComponent* BlackboardPtr = AIController->GetBlackboardComponent())
			{
				APawn* TargetPawn = Cast<APawn>(BlackboardPtr->GetValueAsObject(BBKey_DetectPlayer));
				if (TargetPawn == nullptr)
				{
					TargetPawn = Cast<APawn>(BlackboardPtr->GetValueAsObject(BBKey_AlivePlayer));
				}

				return (IsValid(TargetPawn)) ? TargetPawn : nullptr;
			}
		}
	}
	return nullptr;
}

APawn* UFBEnemyGameplayAbility::GetCombatTargetPawn() const
{
	return Cast<APawn>(GetCombatTargetActor());
}
