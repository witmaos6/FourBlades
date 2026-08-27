// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/GameStateComponent.h"
#include "GameplayTagContainer.h"
#include "EnemySpawnManagerComponent.generated.h"

UENUM(BlueprintType)
enum class EEnemySpawnType : uint8
{
	RangeSpawn,
	TargetPointSpawn,
};

/**
 * 
 */
UCLASS()
class FOURBLADES_API UEnemySpawnManagerComponent : public UGameStateComponent
{
	GENERATED_BODY()
public:
	UEnemySpawnManagerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void InitializeComponent() override;

	virtual void BeginPlay() override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "SpawnType")
	EEnemySpawnType SpawnType;

	void SpawnEnemyByRange();

	void SpawnEnemyByTargetPoint();

	TArray<TObjectPtr<class AFBTargetPoint>> TargetPoints;

	UPROPERTY(EditDefaultsOnly, Category = "SpawnEnemy", Meta = (Categories = "EnemyKey"))
	FGameplayTagContainer SpawnEnemyKeys;

public:
	void AddTargetPoint(class AFBTargetPoint* InTargetPoint) { TargetPoints.AddUnique(InTargetPoint); }

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Enemy")
	TSubclassOf<class AFBEnemyCharacterBase> EnemyBase;

	UPROPERTY(EditDefaultsOnly, Category = "Enemy")
	TSubclassOf<class AFBEnemyCharacterBase> BossEnemy;

	UPROPERTY()
	TMap<FGameplayTag, TObjectPtr<class UEnemyPawnData>> EnemyPawnDataMap;

	UPROPERTY()
	TMap<FGameplayTag, TObjectPtr<class UFBEnemyAbilitySet>> EnemyAbilitiesMap;

	UPROPERTY(EditDefaultsOnly, Category = "Enemy")
	uint32 NrOfSpawn;

	uint32 CurrentSpawn;

	uint32 NrOfCurrentEnemy;

	FTimerHandle SpawnTimer;

	UFUNCTION()
	void DestroyCountEnemy(AActor* EnemyCharacter);

public:
	void RequestRespawn(class AFBTargetPoint* InTargetPoint);
};
