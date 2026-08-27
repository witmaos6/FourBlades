// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GASActor/GASActorBase.h"
#include "Interface/FBGameplayEffectApplicator.h"
#include "FBReflectableProjectile.generated.h"

/**
 * 
 */
UCLASS()
class FOURBLADES_API AFBReflectableProjectile : public AGASActorBase, public IFBGameplayEffectApplicator
{
	GENERATED_BODY()
public:
	AFBReflectableProjectile();

protected:
	virtual void PostInitializeComponents() override;

	void InitializeStat();

	virtual void BeginPlay() override;

	void BeginShot();

protected:
	UPROPERTY(VisibleAnywhere, Category = "Collision")
	TObjectPtr<class USphereComponent> CollisionSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movement")
	TObjectPtr<class UProjectileMovementComponent> ProjectileMovementComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Lifetime")
	TObjectPtr<class ULifetimeMonitorComponent> LifetimeMonitorComponent;

protected:
	UPROPERTY(EditAnywhere, Category = "Effect")
	TSubclassOf<class UGameplayEffect> GameplayEffectClass;

	virtual void InitDamageExecutionParams(FFBDamageExecutionParams&& InDamageExecutionParams) override { DamageExecutionParms = MoveTemp(InDamageExecutionParams); }

	virtual void InitTargetPayload(FFBTargetPayload&& InTargetPayload) override { TargetPayload = MoveTemp(InTargetPayload); }

	FFBDamageExecutionParams DamageExecutionParms;

	FFBTargetPayload TargetPayload;

protected:

	UPROPERTY()
	TObjectPtr<class UFBHealthSet> FBHealthSet;

	UPROPERTY(EditDefaultsOnly, Category = "Health")
	float MaxHealth;

protected:
	void OnAttacked(AActor* ASCInstigator, AActor* EffectCauser, const struct FFBTargetPayload& InTargetPayload);

	void ApplyTemporaryTimeDilation(float TimeScale, float DurationInRealSeconds);

	void ResetGlobalTimeDilation();

	UPROPERTY(EditDefaultsOnly, Category = GameplayCue, meta = (Categories = "GameplayCue"))
	FGameplayTag ReflectSuccessCue;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	uint8 bIsDelay : 1;

	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	float DelayTime;

	bool bReflectable = false; // 플레이어가 튕겨낸 상태임을 나타내는 변수 이후에 수정할 수 있음

protected:
	UFUNCTION()
	void OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void LifetimeDestroy();
protected:
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
};
