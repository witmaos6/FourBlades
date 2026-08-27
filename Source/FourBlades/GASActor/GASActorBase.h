// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AbilitySystemInterface.h"
#include "GASActorBase.generated.h"

UCLASS()
class FOURBLADES_API AGASActorBase : public AActor, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:	
	AGASActorBase();

public:
	virtual void PostInitializeComponents() override;

	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override;

protected:
	UPROPERTY(VisibleAnywhere, Category = GAS)
	TObjectPtr<class UAbilitySystemComponent> ASC;

	UPROPERTY()
	TWeakObjectPtr<AActor> CachedInstigator;

public:
	float ApplyCountDamage(float Damage);
};
