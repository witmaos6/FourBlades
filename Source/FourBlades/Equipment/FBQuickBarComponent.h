// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ControllerComponent.h"
#include "FBQuickBarComponent.generated.h"

/**
 * 
 */
UCLASS()
class FOURBLADES_API UFBQuickBarComponent : public UControllerComponent
{
	GENERATED_BODY()
public:
	UFBQuickBarComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void AddItemToSlot(int32 SlotIndex, UFBInventoryItemInstance* Item);

	UFUNCTION(BlueprintCallable, Category = "FB")
	void SetActiveSlotIndex(int32 NewIndex);

	void UnequipItemInSlot();

	class UFBEquipmentManagerComponent* FindEquipmentManager() const;
	
	void EquipItemInSlot();

	UPROPERTY()
	int32 NumSlots = 3;

	UPROPERTY()
	TArray<TObjectPtr<class UFBInventoryItemInstance>> Slots;

	UPROPERTY()
	int32 ActiveSlotIndex = -1;

	UPROPERTY()
	TObjectPtr<class UFBEquipmentInstance> EquippedItem;
};
