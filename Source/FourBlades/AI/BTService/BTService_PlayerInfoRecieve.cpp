// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTService/BTService_PlayerInfoRecieve.h"
#include "GameModes/FBGameState.h"
#include "GameModes/GameStateComponent/PlayerInfoCachedComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "AI/FBAIKey.h"
#include "FourBlades.h"
#include "Character/FBCharacter.h"
#include "AbilitySystemComponent.h"
#include "GameplayTags/FBDefineGameplayTags.h"

UBTService_PlayerInfoRecieve::UBTService_PlayerInfoRecieve()
{
	NodeName = TEXT("PlayerInfoRecieve");
	Interval = 1.0f;
}

void UBTService_PlayerInfoRecieve::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (ControllingPawn == nullptr)
		return;

	UWorld* World = ControllingPawn->GetWorld();
	if (World == nullptr)
		return;

	AFBGameState* GameState = Cast<AFBGameState>(World->GetGameState());
	if (GameState == nullptr)
		return;

	UPlayerInfoCachedComponent* PlayerInfoCached = GameState->FindComponentByClass<UPlayerInfoCachedComponent>();
	if (PlayerInfoCached == nullptr)
	{
		FB_FUNC_LOG(LogTemp, Warning, TEXT("PlayerInfoCached is InValid"));
		return;
	}

	if (!PlayerInfoCached->PlayerCharacters.IsEmpty())
	{
		AFBCharacter* PlayerPawn = PlayerInfoCached->PlayerCharacters[0]; // To do: 멀티플레이로 바꿀 때 랜덤으로 타겟 지정하는 로직 추가
		if (IAbilitySystemInterface* ASCInterface = Cast<IAbilitySystemInterface>(PlayerPawn))
		{
			if (UAbilitySystemComponent* ASC = ASCInterface->GetAbilitySystemComponent())
			{
				if (!ASC->HasMatchingGameplayTag(FBTAG_CHARACTER_STATE_DEAD))
				{
					OwnerComp.GetBlackboardComponent()->SetValueAsObject(BBKey_AlivePlayer, PlayerPawn);
				}
			}
		}
	}	
}
