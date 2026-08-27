// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/PawnComponent.h"
#include "AbilitySystem/FBAbilitySet.h"
#include "FBInteractionManagerComponent.generated.h"

/*
* AppliedInteractionEntry
*/
USTRUCT(BlueprintType)
struct FFBAppliedInteractionEntry
{
    GENERATED_BODY()

    UPROPERTY()
    TObjectPtr<UFBAbilitySet> AbilitySet = nullptr;

    UPROPERTY()
    FFBAbilitySet_GrantedHandles GrantedHandles;

    UPROPERTY()
    int32 ReferenceCount = 0;
};

/*
* InteractionList
*/
USTRUCT(BlueprintType)
struct FFBInteractionList
{
    GENERATED_BODY()

    FFBInteractionList(UActorComponent* InOwnerComponent = nullptr)
        : OwnerComponent(InOwnerComponent) {
    }

    void AddEntry(UFBAbilitySet* AbilitySet);

    void RemoveEntry(UFBAbilitySet* AbilitySet);

    class UFBAbilitySystemComponent* GetAbilitySystemComponent() const;

    UPROPERTY()
    TArray<FFBAppliedInteractionEntry> Entries;

    UPROPERTY()
    TObjectPtr<UActorComponent> OwnerComponent;
};

/**
 * InteractionManagerComponent
 */
UCLASS()
class FOURBLADES_API UFBInteractionManagerComponent : public UPawnComponent
{
	GENERATED_BODY()
public:
	UFBInteractionManagerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void BeginPlay() override;

	UFUNCTION()
	void InteractionActorBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void InteractionActorEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

protected:
    UPROPERTY()
    FFBInteractionList InteractionList;
};
