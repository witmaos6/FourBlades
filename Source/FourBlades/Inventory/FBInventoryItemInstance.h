// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "FBInventoryItemInstance.generated.h"

/**
 * 
 */
UCLASS()
class FOURBLADES_API UFBInventoryItemInstance : public UObject
{
	GENERATED_BODY()
public:
	UFBInventoryItemInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	const class UFBInventoryItemFragment* FindFragmentByClass(TSubclassOf<UFBInventoryItemFragment> FragmentClass) const;

	template <typename ResultClass>
	const ResultClass* FindFragmentByClass() const
	{
		return (ResultClass*)FindFragmentByClass(ResultClass::StaticClass());
	}

	UPROPERTY()
	TSubclassOf<class UFBInventoryItemDefinition> ItemDef;
};
