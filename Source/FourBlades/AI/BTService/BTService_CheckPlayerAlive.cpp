// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTService/BTService_CheckPlayerAlive.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "GameplayTags/FBDefineGameplayTags.h"
#include "GameFramework/Character.h"
#include "FourBlades.h"

UBTService_CheckPlayerAlive::UBTService_CheckPlayerAlive()
{
	NodeName = TEXT("Check Player Alive");

	Interval = 1.0f;
	RandomDeviation = 0.0f;

	bCallTickOnSearchStart = false;
	bRestartTimerOnEachActivation = true;
}

void UBTService_CheckPlayerAlive::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp)
	{
		FB_FUNC_LOG(LogTemp, Warning, TEXT("BlackboardComponent is null"));
		return;
	}

	if (ACharacter* Character = Cast<ACharacter>(BlackboardComp->GetValueAsObject(PlayerTargetKey.SelectedKeyName)))
	{
		if (IAbilitySystemInterface* ASCInterface = Cast<IAbilitySystemInterface>(Character))
		{
			if (UAbilitySystemComponent* ASC = ASCInterface->GetAbilitySystemComponent())
			{
				if (ASC->HasMatchingGameplayTag(FBTAG_CHARACTER_STATE_DEAD))
				{
					BlackboardComp->ClearValue(PlayerTargetKey.SelectedKeyName);
				}
			}
		}
	}
}
