// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "FBInventoryItemDefinition.generated.h"

/*
* InventoryItemFragment
*/
UCLASS(Abstract, DefaultToInstanced, EditInlineNew)
class UFBInventoryItemFragment : public UObject
{
	GENERATED_BODY()
public:
};

/*
 * InventoryItemDefinition
 */
UCLASS(Blueprintable)
class FOURBLADES_API UFBInventoryItemDefinition : public UObject
{
	GENERATED_BODY()
public:
	UFBInventoryItemDefinition(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	const UFBInventoryItemFragment* FindFragmentByClass(TSubclassOf<UFBInventoryItemFragment> FragmentClass) const;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Display)
	FText DisplayName;

	UPROPERTY(EditDefaultsOnly, Instanced, BlueprintReadOnly, Category = Display)
	TArray<TObjectPtr<UFBInventoryItemFragment>> Fragments;
};
