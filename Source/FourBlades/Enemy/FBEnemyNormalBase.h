// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/FBEnemyCharacterBase.h"
#include "GameplayTagContainer.h"
#include "Interface/AI/FBAICharacterInterface.h"
#include "Interface/FBEnemyDataInterface.h"
#include "BehaviorTree/Blackboard/BlackboardKey.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType.h"
#include "FBEnemyNormalBase.generated.h"


/**
 * 
 */
UCLASS()
class FOURBLADES_API AFBEnemyNormalBase : public AFBEnemyCharacterBase, public IFBAICharacterInterface, public IFBEnemyDataInterface
{
	GENERATED_BODY()
public:
	AFBEnemyNormalBase();

	virtual void PostInitializeComponents() override;

protected:
	void SettingPawnFromData();

protected:
	TArray<TObjectPtr<AActor>> AttachedActor;

public:
	virtual void PossessedBy(AController* NewController) override;

protected:
	void GiveAbilities();

	void SetOnCombatEvent();

	void SetDead();

	bool bOutOfHealth;

protected:
	virtual void BeginPlay() override;

	FBlackboard::FKey PlayerDetectKey;

	FDelegateHandle PlayerDetectHandle;

public:
	EBlackboardNotificationResult PlayerDetected(const class UBlackboardComponent& BlackboardComp, FBlackboard::FKey KeyID);

protected:
	void InitializeStat();

public: // IFBAICharacterInterface
	virtual float GetPatrolRadius() override { return EnemyPawnData->PatrolRadius; }
	virtual float GetPlayerDetectDistance() override { return EnemyPawnData->DetectDistance; }

public: // IFBEnemyDataInterface
	virtual class UFBAbilityFragmentManager* GetAbilityFragmentManager(const FGameplayTag& InAbilityTag) override;

	virtual const float GetAbilityActivateMinRange(EAbilityConditionType InConditionType) override;

	virtual const float GetAbilityActivateMaxRange(EAbilityConditionType InConditionType) override;

	virtual const float GetAbilityActivateValidRadius(EAbilityConditionType InConditionType) override;

	virtual FGameplayTag GetAbilityTagWithWeight(EAbilityConditionType InConditionType) override;

	virtual class UFBAbilityFragmentManager* GetReactAbilityFragmentManager(const FGameplayTag& InAbilityTag) override;
};
