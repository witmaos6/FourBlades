// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTService/BTService_PlayerDetect.h"
#include "AIController.h"
#include "Interface/AI/FBAICharacterInterface.h"
#include "Physics/FBCollisionChannels.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Engine/OverlapResult.h"
#include "AI/FBAIKey.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "GameplayTags/FBDefineGameplayTags.h"

UBTService_PlayerDetect::UBTService_PlayerDetect()
{
	NodeName = TEXT("PlayerDetect");
	Interval = 1.0f;
}

void UBTService_PlayerDetect::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	if (UObject* DetectPlayer = OwnerComp.GetBlackboardComponent()->GetValueAsObject(BBKey_DetectPlayer))
	{
		return;
	}

	APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (ControllingPawn == nullptr)
	{
		return;
	}

	UWorld* World = ControllingPawn->GetWorld();
	if (World == nullptr)
	{
		return;
	}

	IFBAICharacterInterface* FBAICharacterInterface = Cast<IFBAICharacterInterface>(ControllingPawn);
	if (FBAICharacterInterface == nullptr)
	{
		return;
	}
	
	FVector Center = ControllingPawn->GetActorLocation();
	float DetectRadius = FBAICharacterInterface->GetPlayerDetectDistance();

	TArray<FOverlapResult> OverlapResults;
	bool bResult = World->OverlapMultiByChannel(OverlapResults, Center, FQuat::Identity, FB_TraceChannel_Detect, FCollisionShape::MakeSphere(DetectRadius));

	ACharacter* ControllingCharacter = Cast<ACharacter>(ControllingPawn);

	if (bResult)
	{
		for (FOverlapResult const& OverlapResult : OverlapResults)
		{
			APawn* Pawn = Cast<APawn>(OverlapResult.GetActor());
			if (Pawn && Pawn->GetController()->IsPlayerController())
			{
				if (IAbilitySystemInterface* AbilityInterface = Cast<IAbilitySystemInterface>(Pawn))
				{
					if (UAbilitySystemComponent* ASC = AbilityInterface->GetAbilitySystemComponent())
					{
						if (!ASC->HasMatchingGameplayTag(FBTAG_CHARACTER_STATE_DEAD))
						{
							OwnerComp.GetBlackboardComponent()->SetValueAsObject(BBKey_DetectPlayer, Pawn);
							DrawDebugSphere(World, Center, DetectRadius, 16, FColor::Green, false, 0.2f);
						}
					}
				}
			}
		}
	}
	else
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsObject(BBKey_DetectPlayer, nullptr);
		DrawDebugSphere(World, Center, DetectRadius, 16, FColor::Red, false, 0.2f);
	}
}
