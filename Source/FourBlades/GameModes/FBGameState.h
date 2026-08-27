// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ModularGameState.h"
#include "FBGameState.generated.h"

DECLARE_DELEGATE_OneParam(FOnPlayerSpawned, class AFBCharacter* PlayerCharacter);
/**
 * 
 */
UCLASS()
class FOURBLADES_API AFBGameState : public AModularGameState
{
	GENERATED_BODY()
	
public:
	AFBGameState();

	/*virtual void PreInitializeComponents() override;
	virtual void PostInitializeComponents() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;*/

	template <typename T>
	static T* GetGameStateComponent(const UObject* WorldContextObject);

	void PlayerPossessedFinish(class AFBCharacter* NewCharacter);

	FOnPlayerSpawned OnPlayerSpawned;

public:
	UPROPERTY()
	TObjectPtr<class UFBExperienceManagerComponent> ExperienceManagerComponent;
};

template<typename T>
inline T* AFBGameState::GetGameStateComponent(const UObject* WorldContextObject)
{
	if (WorldContextObject == nullptr)
	{
		return nullptr;
	}

	AFBGameState* GameState = WorldContextObject->GetWorld()->GetGameState<AFBGameState>();
	if (GameState)
	{
		return GameState->FindComponentByClass<T>();
	}
	return nullptr;
}
