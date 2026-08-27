// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/GameStateComponent.h"
#include "GameplayTagContainer.h"
#include "FBRewardManagerComponent.generated.h"

/**
 * 
 */
UCLASS()
class FOURBLADES_API UFBRewardManagerComponent : public UGameStateComponent // 몬스터 처치 시 보상을 주는 컴포넌트이기 때문에 UFBCombat(Defeat)RewardManagerComponent이라는  이름으로 변경할 수 있음
{
	GENERATED_BODY()
	
public:
	UFBRewardManagerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void BeginPlay() override;

public:
	void EnemyBeginPlay(APawn* Pawn);

	void EnemyInitAbilityActorInfo(class UAbilitySystemComponent* ASC);

	void XPReward(const FGameplayTag KilledPawnTag, AActor* Instigator, AActor* EffectCauser);
};
