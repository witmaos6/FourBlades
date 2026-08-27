// Fill out your copyright notice in the Description page of Project Settings.


#include "Cosmetics/FBPawnComponent_CharacterParts.h"
#include "GameFramework/Character.h"
#include "GameplayTagAssetInterface.h"

/*
* PawnComponent_CharacterParts
*/
UFBPawnComponent_CharacterParts::UFBPawnComponent_CharacterParts(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
, CharacterPartList(this)
{
}

FFBCharacterPartHandle UFBPawnComponent_CharacterParts::AddCharacterPart(const FFBCharacterPart& NewPart)
{
	return CharacterPartList.AddEntry(NewPart);
}

void UFBPawnComponent_CharacterParts::RemoveCharacterPart(FFBCharacterPartHandle Handle)
{
	CharacterPartList.RemoveEntry(Handle);
}

void UFBPawnComponent_CharacterParts::BroadcastChanged()
{
	const bool bReinitPose = true;

	if (USkeletalMeshComponent* MeshComponent = GetParentMeshComponent())
	{
		const FGameplayTagContainer MergedTags = GetCombinedTag(FGameplayTag());
		USkeletalMesh* DesiredMesh = BodyMeshes.SelectBestBodyStyle(MergedTags);

		MeshComponent->SetSkeletalMesh(DesiredMesh, bReinitPose);

		if (UPhysicsAsset* PhysicsAsset = BodyMeshes.ForcedPhysicsAsset)
		{
			MeshComponent->SetPhysicsAsset(PhysicsAsset, bReinitPose);
		}
	}
}

FGameplayTagContainer UFBPawnComponent_CharacterParts::GetCombinedTag(FGameplayTag RequiredPrefix) const
{
	FGameplayTagContainer Result = CharacterPartList.CollectCombinedTags();
	if (RequiredPrefix.IsValid())
	{
		return Result.Filter(FGameplayTagContainer(RequiredPrefix));
	}
	return Result;
}

USkeletalMeshComponent* UFBPawnComponent_CharacterParts::GetParentMeshComponent() const
{
	if (AActor* OwnerActor = GetOwner())
	{
		if (ACharacter* OwningCharacter = Cast<ACharacter>(OwnerActor))
		{
			if (USkeletalMeshComponent* MeshComponent = OwningCharacter->GetMesh())
			{
				return MeshComponent;
			}
		}
	}
	return nullptr;
}

USceneComponent* UFBPawnComponent_CharacterParts::GetSceneComponentToAttachTo() const
{
	if (USkeletalMeshComponent* MeshComponent = GetParentMeshComponent())
	{
		return MeshComponent;
	}
	if (AActor* OwnerActor = GetOwner())
	{
		return OwnerActor->GetRootComponent();
	}
	return nullptr;
}

/*
* CharacterPartList
*/
FFBCharacterPartHandle FFBCharacterPartList::AddEntry(FFBCharacterPart NewPart)
{
	FFBCharacterPartHandle Result;
	Result.PartHandle = PartHandleCounter++;

	if (ensure(OwnerComponent && OwnerComponent->GetOwner() && OwnerComponent->GetOwner()->HasAuthority()))
	{
		FFBAppliedCharacterPartEntry& NewEntry = Entries.AddDefaulted_GetRef();
		NewEntry.Part = NewPart;
		NewEntry.PartHandle = Result.PartHandle;

		if (SpawnActorForEntry(NewEntry)) // 실제 Actor를 생성하고 Attach
		{
			OwnerComponent->BroadcastChanged();
		}
	}

	return Result;
}

bool FFBCharacterPartList::SpawnActorForEntry(FFBAppliedCharacterPartEntry& Entry)
{
	bool bCreatedAnyActor = false;

	if (Entry.Part.PartClass != nullptr)
	{
		UWorld* World = OwnerComponent->GetWorld();

		if (USceneComponent* ComponentToAttachTo = OwnerComponent->GetSceneComponentToAttachTo())
		{
			const FTransform SpawnTransform = ComponentToAttachTo->GetSocketTransform(Entry.Part.SocketName);

			UChildActorComponent* PartComponent = NewObject<UChildActorComponent>(OwnerComponent->GetOwner());
			PartComponent->SetupAttachment(ComponentToAttachTo, Entry.Part.SocketName);
			PartComponent->SetChildActorClass(Entry.Part.PartClass);
			PartComponent->RegisterComponent();

			if (AActor* SpawnedActor = PartComponent->GetChildActor())
			{
				if (USceneComponent* SpawnedRootComponent = SpawnedActor->GetRootComponent())
				{
					SpawnedRootComponent->AddTickPrerequisiteComponent(ComponentToAttachTo);
				}
			}
			Entry.SpawnedComponent = PartComponent;
			bCreatedAnyActor = true;
		}
	}
	return bCreatedAnyActor;
}

void FFBCharacterPartList::RemoveEntry(FFBCharacterPartHandle Handle)
{
	for (auto EntryIt = Entries.CreateIterator(); EntryIt; ++EntryIt)
	{
		FFBAppliedCharacterPartEntry& Entry = *EntryIt;

		if (Entry.PartHandle == Handle.PartHandle)
		{
			DestroyActorForEntry(Entry);
		}
	}
}

void FFBCharacterPartList::DestroyActorForEntry(FFBAppliedCharacterPartEntry& Entry)
{
	if (Entry.SpawnedComponent)
	{
		Entry.SpawnedComponent->DestroyComponent();
		Entry.SpawnedComponent = nullptr;
	}
}

FGameplayTagContainer FFBCharacterPartList::CollectCombinedTags() const
{
	FGameplayTagContainer Result;

	for (const FFBAppliedCharacterPartEntry& Entry : Entries)
	{
		if (Entry.SpawnedComponent)
		{
			if (IGameplayTagAssetInterface* TagInterface = Cast<IGameplayTagAssetInterface>(Entry.SpawnedComponent->GetChildActor()))
			{
				TagInterface->GetOwnedGameplayTags(Result);
			}
		}
	}
	return Result;
}
