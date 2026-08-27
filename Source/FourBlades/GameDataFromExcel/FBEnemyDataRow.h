
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "FBEnemyDataRow.generated.h"

/*
*
*/
USTRUCT(BlueprintType)
struct FFBEnemyDataRow : public FTableRowBase
{
	GENERATED_BODY()

	FFBEnemyDataRow() : EnemyLevel(0), XPReward(0) {}

	UPROPERTY(VisibleAnywhere, Category = Data)
	FName EnemyPawnDataKey;

	UPROPERTY(VisibleAnywhere, Category = Data)
	uint8 EnemyLevel;

	UPROPERTY(VisibleAnywhere, Category = Data)
	uint32 XPReward;

	UPROPERTY(VisibleAnywhere, Category = Data)
	FName AbilitiesKey;
};
