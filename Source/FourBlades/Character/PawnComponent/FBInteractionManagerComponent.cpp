// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/PawnComponent/FBInteractionManagerComponent.h"
#include "Character/FBCharacter.h"
#include "AbilitySystem/FBAbilitySystemComponent.h"
#include "Components/CapsuleComponent.h"
#include "Interaction/Interface/FBInteractionInterface.h"
#include "AbilitySystem/FBAbilitySet.h"
#include "AbilitySystemGlobals.h"

UFBInteractionManagerComponent::UFBInteractionManagerComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer), InteractionList(this)
{
}

void UFBInteractionManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	if (AFBCharacter* Character = Cast<AFBCharacter>(GetOwner()))
	{
		Character->GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &UFBInteractionManagerComponent::InteractionActorBeginOverlap);
		Character->GetCapsuleComponent()->OnComponentEndOverlap.AddDynamic(this, &UFBInteractionManagerComponent::InteractionActorEndOverlap);
	}
}

void UFBInteractionManagerComponent::InteractionActorBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (IFBInteractionInterface* Interface = Cast<IFBInteractionInterface>(OtherActor))
	{
		if (UFBAbilitySet* AbilitySet = Interface->GetAbilitySet())
		{
			InteractionList.AddEntry(AbilitySet);
		}
	}
}

void UFBInteractionManagerComponent::InteractionActorEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (IFBInteractionInterface* Interface = Cast<IFBInteractionInterface>(OtherActor))
	{
		if (UFBAbilitySet* AbilitySet = Interface->GetAbilitySet())
		{
			InteractionList.RemoveEntry(AbilitySet);
		}
	}
}

/*
* InteractionList
*/
void FFBInteractionList::AddEntry(UFBAbilitySet* AbilitySet)
{
	check(AbilitySet);
	check(OwnerComponent);

	for (FFBAppliedInteractionEntry& Entry : Entries)
	{
		if (Entry.AbilitySet == AbilitySet)
		{
			Entry.ReferenceCount++;
			return;
		}
	}

	FFBAppliedInteractionEntry& NewEntry = Entries.AddDefaulted_GetRef();
	NewEntry.AbilitySet = AbilitySet;
	NewEntry.ReferenceCount = 1;

	UFBAbilitySystemComponent* ASC = GetAbilitySystemComponent();
	if (ASC)
	{
		AbilitySet->GiveToAbilitySystem(ASC, &NewEntry.GrantedHandles, OwnerComponent->GetOwner());
	}
}

void FFBInteractionList::RemoveEntry(UFBAbilitySet* AbilitySet)
{
	for (auto EntryIt = Entries.CreateIterator(); EntryIt; ++EntryIt)
	{
		FFBAppliedInteractionEntry& Entry = *EntryIt;
		if (Entry.AbilitySet == AbilitySet)
		{
			Entry.ReferenceCount--;

			if (Entry.ReferenceCount <= 0)
			{
				UFBAbilitySystemComponent* ASC = GetAbilitySystemComponent();
				if (ASC)
				{
					Entry.GrantedHandles.TakeFromAbilitySystem(ASC);
				}
				EntryIt.RemoveCurrent();
			}
			return;
		}
	}
}

UFBAbilitySystemComponent* FFBInteractionList::GetAbilitySystemComponent() const
{
	check(OwnerComponent);
	AActor* OwningActor = OwnerComponent->GetOwner();

	return Cast<UFBAbilitySystemComponent>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OwningActor));
}
