// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

//#include "Containers/UnrealString.h"
//#include "Containers/Maps.h"
#include "GameplayTagContainer.h"

struct FFBGameplayTags
{
	// To do: 생성자와 소멸자 private으로 지정, 복사생성자 delete
	static const FFBGameplayTags& Get() { return GameplayTags; }

	static void InitializeNativeTags();

	void AddTag(FGameplayTag& OutTag, const ANSICHAR* TagName, const ANSICHAR* TagComment);

	void AddAllTags(class UGameplayTagsManager& Manager);

	// GameInstance의 초기화 과정에 UGameFrameworkComponentManager의 RegisterInitState로 등록되어 선형적으로 업데이트 된다.
	FGameplayTag InitState_Spawned;
	FGameplayTag InitState_DataAvailable;
	FGameplayTag InitState_DataInitialized;
	FGameplayTag InitState_GameplayReady;

	// Enhanced Input Tags
	FGameplayTag InputTag_Move;
	FGameplayTag InputTag_Look_Mouse;
	FGameplayTag InputTag_Ability_Confirm;

private:
	static FFBGameplayTags GameplayTags;
};
