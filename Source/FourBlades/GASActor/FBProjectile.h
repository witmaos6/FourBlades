// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GASActor/GASActorBase.h"
#include "Interface/FBGameplayEffectApplicator.h"
#include "FBProjectile.generated.h"

/**
 * 
 */
UCLASS()
class FOURBLADES_API AFBProjectile : public AGASActorBase, public IFBGameplayEffectApplicator
{
	GENERATED_BODY()

public:
	AFBProjectile();

protected:
	virtual void PostInitializeComponents() override;

	void InitializeStat();

	virtual void BeginPlay() override;

protected:
	UPROPERTY(VisibleAnywhere, Category = "Collision")
	TObjectPtr<class UBoxComponent> CollisionBox;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movement")
	TObjectPtr<class UProjectileMovementComponent> ProjectileMovementComp;	

protected:
	UPROPERTY()
	TObjectPtr<class UFBHealthSet> FBHealthSet;

	UPROPERTY(EditDefaultsOnly, Category = "ActorHealth")
	float MaxHealth;

protected:
	UPROPERTY(EditAnywhere, Category = "Effect")
	TSubclassOf<class UGameplayEffect> GameplayEffectClass;

	virtual void InitDamageExecutionParams(FFBDamageExecutionParams&& InDamageExecutionParams) override { DamageExecutionParms = MoveTemp(InDamageExecutionParams); }

	virtual void InitTargetPayload(FFBTargetPayload&& InTargetPayload) override { TargetPayload = MoveTemp(InTargetPayload); } 

	FFBDamageExecutionParams DamageExecutionParms;

	FFBTargetPayload TargetPayload;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	uint8 bIsDelay : 1;

	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	float DelayTime;

	void BeginShot();

public:
	void OnOutOfHealth();

protected:
	UPROPERTY(EditDefaultsOnly, Category = GameplayCue, meta = (Categories = "GameplayCue"))
	FGameplayTag DestructionParticleTag;

public:
	UFUNCTION()
	void OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnBoxBlock(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
};
