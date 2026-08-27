// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameplayTagContainer.h"
#include "FBGameSingleton.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnDataTableInitComplete);

/**
 * .csv 파일의 DataTable들을 모아 놓은 용도의 싱글톤이다.
 */
UCLASS()
class FOURBLADES_API UFBGameSingleton : public UObject // To do: GameInstance Subsystem으로 이전하는 것이 좋을 것 같다. CharacterDataSubsystem
{
	GENERATED_BODY()
	
public:
	UFBGameSingleton();

	static UFBGameSingleton& Get();

	UPROPERTY()
	TMap<FGameplayTag, TObjectPtr<UDataTable>> TagToDataTable;

	void InitializeTagToDataTable(const TArray<class UFBGameDataTable*>& InGameDataTable);
	// Experience Definition Load가 완료되면 호출 된다.

	template<typename T>
	T* GetDataTableRow(const FGameplayTag InTag, uint32 RowIndex);

	FOnDataTableInitComplete OnDataTableInitComplete;

	void CallOrRegister_OnDataTableInitComplete(FOnDataTableInitComplete::FDelegate&& Delegate, const FGameplayTagContainer& TagContainer);

private:
	bool HasAllTags(const FGameplayTagContainer& TagContainer);
};

template<typename T>
T* UFBGameSingleton::GetDataTableRow(const FGameplayTag InTag, uint32 RowIndex)
{
	check(TagToDataTable.Contains(InTag));
	UDataTable* DataTable = TagToDataTable[InTag];

	TArray<FName> RowNames = DataTable->GetRowNames();
	if (!RowNames.IsValidIndex(RowIndex))
	{
		UE_LOG(LogTemp, Warning, TEXT("UFBGameSingleton::GetDataTableRow: Is Not Valid Index"));
		return nullptr;
	}

	return DataTable->FindRow<T>(RowNames[RowIndex], TEXT(""));
}