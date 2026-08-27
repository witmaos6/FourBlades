// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/DataTable.h"
#include "GameplayTagContainer.h"
#include "GameplayTagDataTable.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class FOURBLADES_API UGameplayTagDataTable : public UObject
{
	GENERATED_BODY()
	
protected:
    UPROPERTY(EditDefaultsOnly, Category = "DataTable")
    TObjectPtr<UDataTable> SourceDataTable;

public:
    template<typename T>
    T* FindRowByGameplayTag(const FGameplayTag& Tag, const FString& ContextString = TEXT(""))
    {
        if (!SourceDataTable)
            return nullptr;

        FName TagAsName = Tag.GetTagName();
        return SourceDataTable->FindRow<T>(TagAsName, ContextString);
    }
};
