// Fill out your copyright notice in the Description page of Project Settings.


#include "Equipment/FBEquipmentInstance.h"
#include "GameFramework/Character.h"
#include "FBEquipmentDefinition.h"
#include "Item/ItemComponent/ChangeAttachmentSocketComponent.h"

UFBEquipmentInstance::UFBEquipmentInstance(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UFBEquipmentInstance::OnEquipped()
{
    K2_OnEquipped();
}

void UFBEquipmentInstance::OnUnequipped()
{
    K2_OnUnequipped();
}

void UFBEquipmentInstance::SpawnEquipmentActors(const TArray<FFBEquipmentActorToSpawn>& ActorsToSpawn)
{
    if (APawn* OwningPawn = GetPawn())
    {
        USceneComponent* AttachTarget = OwningPawn->GetRootComponent();
        if (ACharacter* Character = Cast<ACharacter>(OwningPawn))
        {
            AttachTarget = Character->GetMesh();
        }

        for (const FFBEquipmentActorToSpawn& SpawnInfo : ActorsToSpawn)
        {
            AActor* NewActor = GetWorld()->SpawnActorDeferred<AActor>(SpawnInfo.ActorToSpawn, FTransform::Identity, OwningPawn);
            NewActor->FinishSpawning(FTransform::Identity, true);

            NewActor->SetActorRelativeTransform(SpawnInfo.AttachTransform);

            NewActor->AttachToComponent(AttachTarget, FAttachmentTransformRules::KeepRelativeTransform, SpawnInfo.AttachSocket);

            if (UChangeAttachmentSocketComponent* ChangeAttachmentSocketComponent = NewActor->FindComponentByClass<UChangeAttachmentSocketComponent>())
            {
                ChangeAttachmentSocketComponent->SetOriginSocketName(SpawnInfo.AttachSocket);
                ChangeAttachmentSocketComponent->BindChangeSocket();
                // To do: ItemComponent의 종류가 늘어난다면 FBItemComponent를 순회하며 FBItemComponent::Init()을 호출하는 것으로 변경할 수도 있음
            }

            SpawnedActors.Add(NewActor);
        }
    }
}

void UFBEquipmentInstance::DestroyEquipmentActors()
{
    for (AActor* Actor : SpawnedActors)
    {
        if (Actor)
        {
            Actor->Destroy();
        }
    }
}

APawn* UFBEquipmentInstance::GetPawn() const
{
    return Cast<APawn>(GetOuter());
}

APawn* UFBEquipmentInstance::GetTypedPawn(TSubclassOf<APawn> PawnType) const
{
    APawn* Result = nullptr;
    if (UClass* ActualPawnType = PawnType)
    {
        if (GetOuter()->IsA(ActualPawnType))
        {
            Result = Cast<APawn>(GetOuter());
        }
    }
    return Result;
}