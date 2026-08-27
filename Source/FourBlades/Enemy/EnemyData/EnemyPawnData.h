// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "EnemyPawnData.generated.h"

/*
* EquipActor
*/
USTRUCT(BlueprintType)
struct FAttachEquipActor
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EquipActor")
	TSubclassOf<AActor> EquipActor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EquipName")
	FName SocketName;
};
/**
 * 
 */
UCLASS()
class FOURBLADES_API UEnemyPawnData : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UEnemyPawnData(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy|Mesh")
	TObjectPtr<USkeletalMesh> Mesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy|AnimInstance")
	TSubclassOf<UAnimInstance> AnimInstance;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equip")
	TArray<FAttachEquipActor> EquipActors;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Detect")
	float DetectDistance;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Detect")
	float PatrolRadius;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI")
	TObjectPtr<class UBehaviorTree> BehaviorTree;
};
