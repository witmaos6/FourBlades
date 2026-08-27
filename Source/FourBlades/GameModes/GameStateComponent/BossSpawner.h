// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BossSpawner.generated.h"

UCLASS()
class FOURBLADES_API ABossSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	ABossSpawner();

protected:
	virtual void PostInitializeComponents() override;

	virtual void BeginPlay() override;

protected:
	UPROPERTY(VisibleAnywhere, Category = "PlayerDetect")
	TObjectPtr<class UBoxComponent> PlayerDetectBox;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<USceneComponent> BossSpawnPoint;

	UPROPERTY(EditAnywhere, Category = "Enemy")
	TSubclassOf<class AFBEnemyCharacterBase> BossClass;

protected:
	void CollectWallComponents();

	void InitializeWallState();

private:
	UPROPERTY()
	TArray<TObjectPtr<UStaticMeshComponent>> WallComponents;

	UPROPERTY(EditDefaultsOnly, Category = "WallTag")
	FName WallTag;

protected:
	UFUNCTION()
	void OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

protected:
	void BossSpawn();

	void ActivateWalls();

	UFUNCTION()
	void DeactivateWalls(AActor* BossCharacter);

	void BossAbilityInit(class UAbilitySystemComponent* BossASC);

	void BossDead();
};
