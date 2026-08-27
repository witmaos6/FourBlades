// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "FBGameModeBase.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnPlayerResurrectionDelegate);

/**
 * 
 */
UCLASS()
class FOURBLADES_API AFBGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	AFBGameModeBase();

public: // override Method
	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;

	virtual void InitGameState() final;

	virtual UClass* GetDefaultPawnClassForController_Implementation(AController* InController) final;

	virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) final;

	virtual APawn* SpawnDefaultPawnAtTransform_Implementation(AController* NewPlayer, const FTransform& SpawnTransform) final;

public: // Experience Method
	void HandleMatchAssignmentIfNotExpectingOne();

	void OnMatchAssignmentGiven(FPrimaryAssetId ExperienceId);

	bool IsExperienceLoaded() const;

	void OnExperienceLoaded(const class UFBExperienceDefinition* CurrentExperience);

	bool TryGetSavePointFromRecentSave(struct FFBSavePoint& InSaveGame);

	bool IsValidSavePoint(const FFBSavePoint& InSavePoint) const;

public: // Pawn Data Method
	const class UFBPawnData* GetPawnDataForController(const AController* InController) const;

public: // Resurrection System
	FOnPlayerResurrectionDelegate OnPlayerResurrectionDelegate;

	void BeginResurrection();

protected:
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
};
