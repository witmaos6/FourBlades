// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "FBAbilityDefinitionSet.generated.h"

/**
 * 
 */
UCLASS()
class FOURBLADES_API UFBAbilityDefinitionSet : public UPrimaryDataAsset // 원래 몬스터의 어빌리티를 정의 했으나 플레이어 전용으로 교체할 수도 있음, DeadMenu처럼 InputTag가 필요없는 어빌리티 정의
{
	GENERATED_BODY()
public:
	UFBAbilityDefinitionSet();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Abilities")
	TArray<TSubclassOf<class UGameplayAbility>> Abilities;
};
