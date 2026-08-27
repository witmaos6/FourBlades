// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask/BTTask_ActivateAbilityByTag.h"
#include "AIController.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayTags/FBDefineGameplayTags.h"
#include "Enemy/Interface/FBEnemyDataInterface.h"
#include "FourBlades.h"

UBTTask_ActivateAbilityByTag::UBTTask_ActivateAbilityByTag()
{
	NodeName = TEXT("ActivateAbility");

	AbilityConditionType = EAbilityConditionType::MeleeForwardAttack;
}

EBTNodeResult::Type UBTTask_ActivateAbilityByTag::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	APawn* ControllingPawn = Cast<APawn>(OwnerComp.GetAIOwner()->GetPawn());
	if (ControllingPawn == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(ControllingPawn);
	if (ASC == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	IFBEnemyDataInterface* AIPawn = Cast<IFBEnemyDataInterface>(ControllingPawn);
	if (AIPawn == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	FGameplayTag RandomAbilityTag = AIPawn->GetAbilityTagWithWeight(AbilityConditionType);
	if(!RandomAbilityTag.IsValid())
		return EBTNodeResult::Failed;

	FGameplayTagContainer ActivateTags(RandomAbilityTag);
	if (ASC->TryActivateAbilitiesByTag(ActivateTags))
	{
		TSharedPtr<FDelegateHandle> HandlePtr = MakeShared<FDelegateHandle>();
		UBehaviorTreeComponent* BehaviorTreeComp = &OwnerComp;

		*HandlePtr = ASC->OnAbilityEnded.AddLambda([this, BehaviorTreeComp, RandomAbilityTag, ASC, HandlePtr](const FAbilityEndedData& AbilityEndedData)
			{
				if (AbilityEndedData.AbilityThatEnded->AbilityTags.HasTag(RandomAbilityTag))
				{
					this->FinishLatentTask(*BehaviorTreeComp, EBTNodeResult::Succeeded);
					ASC->OnAbilityEnded.Remove(*HandlePtr);
				}
			});
		return EBTNodeResult::InProgress;
	}

	return EBTNodeResult::Failed;
}
