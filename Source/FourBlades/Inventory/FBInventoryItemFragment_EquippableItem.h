#pragma once

#include "FBInventoryItemDefinition.h"
#include "Templates/SubclassOf.h"
#include "FBInventoryItemFragment_EquippableItem.generated.h"

UCLASS()
class UFBInventoryItemFragment_EquippableItem : public UFBInventoryItemFragment
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, Category = "EquipmentDefinition")
	TSubclassOf<class UFBEquipmentDefinition> EquipmentDefinition;
};
