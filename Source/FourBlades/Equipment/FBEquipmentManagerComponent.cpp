// Fill out your copyright notice in the Description page of Project Settings.


#include "Equipment/FBEquipmentManagerComponent.h"
#include "FBEquipmentInstance.h"
#include "FBEquipmentDefinition.h"
#include "AbilitySystem/FBAbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"

/*
 * EquipmentManagerComponent
 */
UFBEquipmentManagerComponent::UFBEquipmentManagerComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
	, EquipmentList(this)
{
}

UFBEquipmentInstance* UFBEquipmentManagerComponent::EquipItem(TSubclassOf<UFBEquipmentDefinition> EquipmentDefinition)
{
	UFBEquipmentInstance* Result = nullptr;
	if (EquipmentDefinition)
	{
		Result = EquipmentList.AddEntry(EquipmentDefinition);
		if (Result)
		{
			Result->OnEquipped();
		}
	}
	return Result;
}

void UFBEquipmentManagerComponent::UnequipItem(UFBEquipmentInstance* ItemInstance)
{
	if (ItemInstance)
	{
		ItemInstance->OnUnequipped();

		EquipmentList.RemoveEntry(ItemInstance);
	}
}

TArray<UFBEquipmentInstance*> UFBEquipmentManagerComponent::GetEquipmentInstancesOfType(TSubclassOf<UFBEquipmentInstance> InstanceType) const
{
	TArray<UFBEquipmentInstance*> Results;

	for (const FFBAppliedEquipmentEntry& Entry : EquipmentList.Entries)
	{
		if (UFBEquipmentInstance* Instance = Entry.Instance)
		{
			if (Instance->IsA(InstanceType))
			{
				Results.Add(Instance);
			}
		}
	}
	return Results;
}

/*
* EquipmentList
*/
UFBEquipmentInstance* FFBEquipmentList::AddEntry(TSubclassOf<UFBEquipmentDefinition> EquipmentDefinition)
{
	UFBEquipmentInstance* Result = nullptr;
	check(EquipmentDefinition != nullptr);
	check(OwnerComponent);
	check(OwnerComponent->GetOwner()->HasAuthority());

	const UFBEquipmentDefinition* EquipmentCDO = GetDefault<UFBEquipmentDefinition>(EquipmentDefinition);

	TSubclassOf<UFBEquipmentInstance> InstanceType = EquipmentCDO->InstanceType;
	if (!InstanceType)
	{
		InstanceType = UFBEquipmentInstance::StaticClass();
	}

	FFBAppliedEquipmentEntry& NewEntry = Entries.AddDefaulted_GetRef();
	NewEntry.EquipmentDefinition = EquipmentDefinition;
	NewEntry.Instance = NewObject<UFBEquipmentInstance>(OwnerComponent->GetOwner(), InstanceType);
	Result = NewEntry.Instance;

	UFBAbilitySystemComponent* ASC = GetAbilitySystemComponent();
	check(ASC);
	{
		for (const TObjectPtr<UFBAbilitySet> AbilitySet : EquipmentCDO->AbilitySetsToGrant)
		{
			AbilitySet->GiveToAbilitySystem(ASC, &NewEntry.GrantedHandles, Result);
		}
	}

	Result->SpawnEquipmentActors(EquipmentCDO->ActorsToSpawn);
	return Result;
}

void FFBEquipmentList::RemoveEntry(UFBEquipmentInstance* Instance)
{
	for (auto EntryIt = Entries.CreateIterator(); EntryIt; ++EntryIt)
	{
		FFBAppliedEquipmentEntry& Entry = *EntryIt;
		if (Entry.Instance == Instance)
		{
			UFBAbilitySystemComponent* ASC = GetAbilitySystemComponent();
			check(ASC);
			{
				Entry.GrantedHandles.TakeFromAbilitySystem(ASC);
			}

			Instance->DestroyEquipmentActors();
			EntryIt.RemoveCurrent();
		}
	}
}

UFBAbilitySystemComponent* FFBEquipmentList::GetAbilitySystemComponent() const
{
	check(OwnerComponent);
	AActor* OwningActor = OwnerComponent->GetOwner();

	return Cast<UFBAbilitySystemComponent>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OwningActor));
}
