// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ModularAIController.h"
#include "FBAIController.generated.h"

/**
 * 
 */
UCLASS()
class FOURBLADES_API AFBAIController : public AModularAIController
{
	GENERATED_BODY()
public:
	AFBAIController();

protected:
	virtual void OnPossess(APawn* InPawn) override;

public:
	void SetBehaviorTree(class UBehaviorTree* InBehaviorTree);

	void RunAI();

	void StopAI();

	void ContinueAI();

	void ReStartAI();

	UPROPERTY(EditDefaultsOnly, Category = "Blackboard")
	TObjectPtr<class UBlackboardData> BlackboardData;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI")
	TObjectPtr<class UBehaviorTree> BehaviorTree;
};
