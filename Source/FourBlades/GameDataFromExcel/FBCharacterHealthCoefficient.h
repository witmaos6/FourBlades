
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "SaveGame/FBSaveGameTypes.h"
#include "FBCharacterHealthCoefficient.generated.h"


// To do: 캐릭터 체력 계수, 방어력 계수, 공격력 계수 등 추가 예정
USTRUCT(BlueprintType)
struct FFBCharacterHealthCoefficient : public FTableRowBase
{
	GENERATED_BODY()
	
	FFBCharacterHealthCoefficient() : HealthCoefficient(0.0f) {}

	UPROPERTY(VisibleAnywhere, Category = Stat)
	float HealthCoefficient;
};
