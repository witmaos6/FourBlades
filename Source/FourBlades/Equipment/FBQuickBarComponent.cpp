// Fill out your copyright notice in the Description page of Project Settings.


#include "Equipment/FBQuickBarComponent.h"
#include "FBEquipmentManagerComponent.h"
#include "FBEquipmentInstance.h"
#include "FBEquipmentDefinition.h"
#include "Inventory/FBInventoryItemInstance.h"
#include "Inventory/FBInventoryItemFragment_EquippableItem.h"

UFBQuickBarComponent::UFBQuickBarComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UFBQuickBarComponent::BeginPlay()
{
	if (Slots.Num() < NumSlots)
	{
		Slots.AddDefaulted(NumSlots - Slots.Num());
	}

	Super::BeginPlay();
}

void UFBQuickBarComponent::AddItemToSlot(int32 SlotIndex, UFBInventoryItemInstance* Item)
{
	if (Slots.IsValidIndex(SlotIndex) && (Item != nullptr))
	{
		if(Slots[SlotIndex] == nullptr)
		{
			Slots[SlotIndex] = Item;
		}
	}
}

void UFBQuickBarComponent::SetActiveSlotIndex(int32 NewIndex)
{
	if (Slots.IsValidIndex(NewIndex) && (ActiveSlotIndex != NewIndex))
	{
		UnequipItemInSlot();
		ActiveSlotIndex = NewIndex;
		EquipItemInSlot();
	}
}

void UFBQuickBarComponent::UnequipItemInSlot()
{
	if (UFBEquipmentManagerComponent* EquipmentManager = FindEquipmentManager())
	{
		if (EquippedItem)
		{
			EquipmentManager->UnequipItem(EquippedItem);

			EquippedItem = nullptr;
		}
	}
}

UFBEquipmentManagerComponent* UFBQuickBarComponent::FindEquipmentManager() const
{
	if (AController* OwnerController = Cast<AController>(GetOwner()))
	{
		if (APawn* Pawn = OwnerController->GetPawn())
		{
			return Pawn->FindComponentByClass<UFBEquipmentManagerComponent>();
		}
	}
	return nullptr;
}

void UFBQuickBarComponent::EquipItemInSlot()
{
	check(Slots.IsValidIndex(ActiveSlotIndex));
	check(EquippedItem == nullptr);

	if (UFBInventoryItemInstance* SlotItem = Slots[ActiveSlotIndex])
	{
		if (const UFBInventoryItemFragment_EquippableItem* EquipInfo = SlotItem->FindFragmentByClass<UFBInventoryItemFragment_EquippableItem>())
		{
			TSubclassOf<UFBEquipmentDefinition> EquipDef = EquipInfo->EquipmentDefinition;
			if (EquipDef)
			{
				if (UFBEquipmentManagerComponent* EquipmentManager = FindEquipmentManager())
				{
					EquippedItem = EquipmentManager->EquipItem(EquipDef);
					if (EquippedItem)
					{
						EquippedItem->Instigator = SlotItem;
					}
				}
			}
		}
	}
}
