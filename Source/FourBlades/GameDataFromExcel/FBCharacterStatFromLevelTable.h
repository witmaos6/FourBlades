
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "FBCharacterStatFromLevelTable.generated.h"

USTRUCT(BlueprintType)
struct FFBCharacterStatFromLevelTable : public FTableRowBase // 레벨 별 기본 스탯 이름을 바꿔야 할 수도 있다.
{
	GENERATED_BODY()
	
	FFBCharacterStatFromLevelTable() : MaxAddHealth(0.0f), AttackPower(0.0f), Defense(0.0f) {}

	UPROPERTY(VisibleAnywhere, Category = Stat)
	float MaxAddHealth;

	UPROPERTY(VisibleAnywhere, Category = Stat)
	float AttackPower;

	UPROPERTY(VisibleAnywhere, Category = Stat)
	float Defense;
};
