// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ModularPlayerState.h"
#include "GameFramework/SaveGame.h"
#include "Character/FBPawnData.h"
#include "FBPlayerState.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnPostInitializeComponent);

/**
 * 
 */
UCLASS()
class FOURBLADES_API AFBPlayerState : public AModularPlayerState
{
	GENERATED_BODY()
	
public:
	AFBPlayerState(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void PostInitializeComponents() final;

	FOnPostInitializeComponent OnPostInitializeComponent;

public: // Experience
	void OnExperienceLoaded(const class UFBExperienceDefinition* CurrentExperience);	

	UPROPERTY()
	TObjectPtr<const class UFBPawnData> PawnData;

	void SetPawnData(const class UFBPawnData* InPawnData);

	template <class T>
	const T* GetPawnData() const { return Cast<T>(PawnData); }

	class UFBAbilitySet* GetAbilitySet() const { return PawnData->AbilitySets[0]; } // To do: 추후에 인덱스 조정

public: // AbilitySystem
	UPROPERTY(VisibleAnywhere, Category = "FB|PlayerState")
	TObjectPtr<class UFBAbilitySystemComponent> AbilitySystemComponent;

	class UFBAbilitySystemComponent* GetFBAbilitySystemComponent() const { return AbilitySystemComponent; }

	UPROPERTY()
	TObjectPtr<class UFBHealthSet> FBHealthSet;

	UPROPERTY()
	TObjectPtr<class UFBCombatSet> FBCombatSet;

	UPROPERTY()
	TObjectPtr<class UFBGuardSet> FBGuardSet;

	bool bOnPostInitializeComponent = false;

protected:
	UPROPERTY()
	TObjectPtr<class UFBSaveGame> SaveData;

public:
	void InitSavePoint(APawn* InPlayerPawn);

	UFBSaveGame* GetSaveData()const { return SaveData; }

protected:
	void SetDead();

	void Resurrection();
};
