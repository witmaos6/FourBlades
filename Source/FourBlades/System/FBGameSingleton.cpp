// Fill out your copyright notice in the Description page of Project Settings.


#include "System/FBGameSingleton.h"
#include "Log/FBLogChannels.h"
#include "GameDataFromExcel/FBGameDataTable.h"
#include "GameplayTagContainer.h"

UFBGameSingleton::UFBGameSingleton()
{
	
}

UFBGameSingleton& UFBGameSingleton::Get()
{
	UFBGameSingleton* Singleton = CastChecked<UFBGameSingleton>(GEngine->GameSingleton);
	if (Singleton)
	{
		return *Singleton;
	}

	UE_LOG(LogFB, Fatal, TEXT("Invalid AssetManager"));
	return *NewObject<UFBGameSingleton>(); // 컴파일을 위한 return
}

void UFBGameSingleton::InitializeTagToDataTable(const TArray<UFBGameDataTable*>& InGameDataTable)
{
	for (UFBGameDataTable* GameDataTable : InGameDataTable)
	{
		TagToDataTable.Add(GameDataTable->DataTableTag, GameDataTable->DataTable);
	}

	FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateLambda([this](float DeltaTime)
		{
			OnDataTableInitComplete.Broadcast();
			OnDataTableInitComplete.Clear();
			return false;
		}));
}

void UFBGameSingleton::CallOrRegister_OnDataTableInitComplete(FOnDataTableInitComplete::FDelegate&& Delegate, const FGameplayTagContainer& TagContainer)
{
	if (HasAllTags(TagContainer))
	{
		Delegate.Execute();
	}
	else
	{
		OnDataTableInitComplete.Add(MoveTemp(Delegate));
	}
}

bool UFBGameSingleton::HasAllTags(const FGameplayTagContainer& TagContainer)
{
	for (const FGameplayTag& Tag : TagContainer)
	{
		if (!TagToDataTable.Contains(Tag))
		{
			return false;
		}
	}
	return true;
}
