
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "FBEnemyStatTable.generated.h"

USTRUCT(BlueprintType)
struct FFBEnemyStatTable : public FTableRowBase
{
	GENERATED_BODY()
	
	FFBEnemyStatTable() : MaxHealth(0.0f), AttackPower(0.0f), Defense(0.0f) {}

	UPROPERTY(VisibleAnywhere, Category = Stat)
	float MaxHealth;

	UPROPERTY(VisibleAnywhere, Category = Stat)
	float AttackPower;

	UPROPERTY(VisibleAnywhere, Category = Stat)
	float Defense;
};
