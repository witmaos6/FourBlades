
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "FBEnemyStatFromLevelRow.generated.h"

USTRUCT(BlueprintType)
struct FFBEnemyStatFromLevelRow : public FTableRowBase
{
	GENERATED_BODY()

	FFBEnemyStatFromLevelRow() : MaxHealth(0.0f), AttackPower(0.0f), Defense(0.0f) {}

	UPROPERTY(VisibleAnywhere, Category = Stat)
	float MaxHealth;

	UPROPERTY(VisibleAnywhere, Category = Stat)
	float AttackPower;

	UPROPERTY(VisibleAnywhere, Category = Stat)
	float Defense;
};
