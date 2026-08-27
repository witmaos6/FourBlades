// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "FBAbilityFragmentManager.generated.h"

/*
*/
UCLASS(Abstract, DefaultToInstanced, EditInlineNew)
class UFBAbilityFragment : public UObject
{
	GENERATED_BODY()
public:
};

/**
 * 
 */
UCLASS(Blueprintable, EditInlineNew)
class FOURBLADES_API UFBAbilityFragmentManager : public UObject
{
	GENERATED_BODY()

public:
	UFBAbilityFragmentManager(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	const UFBAbilityFragment* FindFragmentByClass(TSubclassOf<UFBAbilityFragment> FragmentClass) const;

	template<class T>
	T* FindFragmentByClass() const
	{
		for (UFBAbilityFragment* Fragment : Fragments)
		{
			if (Fragment->IsA(T::StaticClass()))
			{
				return Cast<T>(Fragment);
			}
		}
		return nullptr;
	}

protected:
	UPROPERTY(EditDefaultsOnly, Instanced, Category = Display)
	TArray<TObjectPtr<UFBAbilityFragment>> Fragments;
};
