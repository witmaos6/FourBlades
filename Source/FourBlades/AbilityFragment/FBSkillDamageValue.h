#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "FBSkillDamageValue.generated.h"

/**
 *  FFBSkillDamageValue
 */
USTRUCT(BlueprintType)
struct FFBSkillDamageValue
{
	GENERATED_BODY()

	FFBSkillDamageValue()
	{
		DamageCoefficient = 1.0f;
		AdditionalFixedDamage = 0.0f;
	}

	UPROPERTY(EditDefaultsOnly, Category = GameplayTag, meta = (Categories = "AttackType.Attack"))
	FGameplayTag AttackType = FGameplayTag();

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	float DamageCoefficient;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	float AdditionalFixedDamage;
};

/**
 *  FFBLaunchInfo
 */
USTRUCT(BlueprintType)
struct FFBLaunchInfo
{
	GENERATED_BODY()

	FFBLaunchInfo()
	{
		ReactDuration = 1.0f;
		LaunchPower = 0.0f;
	}

	// enum LaunchDirection // To do: 밀어내는 방향은 enum 클래스로 정의하는 것으로 수정

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	float ReactDuration;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	float LaunchPower;
};

