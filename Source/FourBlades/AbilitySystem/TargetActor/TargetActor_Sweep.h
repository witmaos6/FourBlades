// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetActor.h"
#include "TargetActor_Sweep.generated.h"

/**
 * 
 */
UCLASS()
class FOURBLADES_API ATargetActor_Sweep : public AGameplayAbilityTargetActor
{
	GENERATED_BODY()
	
public:
	ATargetActor_Sweep();

	virtual void StartTargeting(UGameplayAbility* Ability) override;

	virtual void ConfirmTargetingAndContinue() override;

protected:
	virtual FGameplayAbilityTargetDataHandle MakeTargetData() const;

	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	float Radius; // Radius와 Range는 추후에 캐릭터의 정보로 이관 될 수 있음

	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	float Range;

	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	TEnumAsByte<ECollisionChannel> TraceChannel;

	bool bShowDebug = true;
};
