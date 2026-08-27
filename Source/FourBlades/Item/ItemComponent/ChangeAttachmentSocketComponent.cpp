// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/ItemComponent/ChangeAttachmentSocketComponent.h"
#include "GameFramework/Character.h"
#include "Equipment/FBEquipmentManagerComponent.h"
#include "Equipment/FBEquipmentInstance.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GameplayTagContainer.h"

UChangeAttachmentSocketComponent::UChangeAttachmentSocketComponent()
{
	bWhenAddedTag = true;
}

void UChangeAttachmentSocketComponent::OnRegister()
{
	Super::OnRegister();
}

void UChangeAttachmentSocketComponent::BindChangeSocket()
{
	if (AActor* Item = GetOwner())
	{
		if (ACharacter* Character = Cast<ACharacter>(Item->GetAttachParentActor()))
		{
			if (UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Character))
			{
				if (TriggerSocketChangeTag.IsValid())
				{
					if (bWhenAddedTag)
					{
						ASC->RegisterGameplayTagEvent(TriggerSocketChangeTag, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &UChangeAttachmentSocketComponent::TagAddedChangeAttachmentSocket);
					}
					else
					{
						ASC->RegisterGameplayTagEvent(TriggerSocketChangeTag, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &UChangeAttachmentSocketComponent::TagRemovedChangeAttachmentSocket);
					}
				}
			}
		}
	}
}

void UChangeAttachmentSocketComponent::TagAddedChangeAttachmentSocket(const FGameplayTag Tag, int32 NewCount)
{
	AActor* Item = GetOwner();
	if (Item)
	{
		if (ACharacter* Character = Cast<ACharacter>(Item->GetAttachParentActor()))
		{
			if (USkeletalMeshComponent* SkeletalMesh = Character->GetMesh())
			{
				if (NewCount > 0)
				{
					Item->AttachToComponent(SkeletalMesh, FAttachmentTransformRules::KeepRelativeTransform, ChangeSocketName);
				}
				else
				{
					Item->AttachToComponent(SkeletalMesh, FAttachmentTransformRules::KeepRelativeTransform, OriginSocketName);
				}
			}
		}
	}
}

void UChangeAttachmentSocketComponent::TagRemovedChangeAttachmentSocket(const FGameplayTag Tag, int32 NewCount)
{
	AActor* Item = GetOwner();
	if (Item)
	{
		if (ACharacter* Character = Cast<ACharacter>(Item->GetAttachParentActor()))
		{
			if (USkeletalMeshComponent* SkeletalMesh = Character->GetMesh())
			{
				if (NewCount > 0)
				{
					Item->AttachToComponent(SkeletalMesh, FAttachmentTransformRules::KeepRelativeTransform, OriginSocketName);
				}
				else
				{
					Item->AttachToComponent(SkeletalMesh, FAttachmentTransformRules::KeepRelativeTransform, ChangeSocketName);
				}
			}
		}
	}
}
