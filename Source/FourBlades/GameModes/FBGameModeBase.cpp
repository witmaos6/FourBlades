// Fill out your copyright notice in the Description page of Project Settings.


#include "GameModes/FBGameModeBase.h"
#include "GameModes/FBGameState.h"
#include "Player/FBPlayerController.h"
#include "Player/FBPlayerState.h"
#include "Character/FBCharacterBase.h"
#include "FBExperienceManagerComponent.h"
#include "Character/FBPawnData.h"
#include "FBExperienceDefinition.h"
#include "Character/PawnComponent/FBPawnExtensionComponent.h"
#include "Kismet/GameplayStatics.h"
#include "UI/FBHUD.h"
#include "System/FBGameSingleton.h"
#include "System/FBSaveGameSubsystem.h"
#include "SaveGame/Interface/FBSavePointInterface.h"
#include "SaveGame/FBSaveGame.h"

AFBGameModeBase::AFBGameModeBase()
{
	GameStateClass = AFBGameState::StaticClass();
	PlayerControllerClass = AFBPlayerController::StaticClass();
	PlayerStateClass = AFBPlayerState::StaticClass();
	DefaultPawnClass = AFBCharacterBase::StaticClass();
	HUDClass = AFBHUD::StaticClass();
}

void AFBGameModeBase::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ThisClass::HandleMatchAssignmentIfNotExpectingOne);
}

void AFBGameModeBase::InitGameState()
{
	Super::InitGameState();

	UFBExperienceManagerComponent* ExperienceManagerComponent = GameState->FindComponentByClass<UFBExperienceManagerComponent>();
	check(ExperienceManagerComponent);

	ExperienceManagerComponent->CallOrRegister_OnExperienceLoaded(FOnFBExperienceLoaded::FDelegate::CreateUObject(this, &ThisClass::OnExperienceLoaded));
}

UClass* AFBGameModeBase::GetDefaultPawnClassForController_Implementation(AController* InController)
{
	if (const UFBPawnData* PawnData = GetPawnDataForController(InController))
	{
		if (PawnData->PawnClass)
		{
			return PawnData->PawnClass;
		}
	}
	return Super::GetDefaultPawnClassForController_Implementation(InController);
}

void AFBGameModeBase::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	if (IsExperienceLoaded())
	{
		Super::HandleStartingNewPlayer_Implementation(NewPlayer);
	}
}

APawn* AFBGameModeBase::SpawnDefaultPawnAtTransform_Implementation(AController* NewPlayer, const FTransform& SpawnTransform)
{
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.Instigator = GetInstigator();
	SpawnInfo.ObjectFlags |= RF_Transient;
	SpawnInfo.bDeferConstruction = true;

	if (UClass* PawnClass = GetDefaultPawnClassForController(NewPlayer))
	{
		if (APawn* SpawnedPawn = GetWorld()->SpawnActor<APawn>(PawnClass, SpawnTransform, SpawnInfo))
		{
			if (UFBPawnExtensionComponent* PawnExtComp = UFBPawnExtensionComponent::FindPawnExtensionComponent(SpawnedPawn))
			{
				if (const UFBPawnData* PawnData = GetPawnDataForController(NewPlayer))
				{
					PawnExtComp->SetPawnData(PawnData);
				}
			}

			SpawnedPawn->FinishSpawning(SpawnTransform);
			return SpawnedPawn;
		}
	}

	return nullptr;
}

void AFBGameModeBase::HandleMatchAssignmentIfNotExpectingOne() // Lyra에서는 여러 가지 경우의 수에 따라 하나의 ExperienceId를 할당한다.
{
	FPrimaryAssetId ExperienceId;

	UWorld* World = GetWorld();

	if (!ExperienceId.IsValid() && UGameplayStatics::HasOption(OptionsString, TEXT("Experience")))
	{
		const FString ExperienceFromOptions = UGameplayStatics::ParseOption(OptionsString, TEXT("Experience"));
		FPrimaryAssetType AssetType = FPrimaryAssetType(UFBExperienceDefinition::StaticClass()->GetFName());
		ExperienceId = FPrimaryAssetId(AssetType, FName(*ExperienceFromOptions));
	}

	if (!ExperienceId.IsValid())
	{
		ExperienceId = FPrimaryAssetId(FPrimaryAssetType("FBExperienceDefinition"), FName("BP_FBLobbyExperience"));
	}

	OnMatchAssignmentGiven(ExperienceId);
}

void AFBGameModeBase::OnMatchAssignmentGiven(FPrimaryAssetId ExperienceId)
{
	check(ExperienceId.IsValid());

	UFBExperienceManagerComponent* ExperienceManagerComponent = GameState->FindComponentByClass<UFBExperienceManagerComponent>();
	check(ExperienceManagerComponent);
	ExperienceManagerComponent->ServerSetCurrentExperience(ExperienceId);
}

bool AFBGameModeBase::IsExperienceLoaded() const
{
	check(GameState);
	UFBExperienceManagerComponent* ExperienceManagerComponent = GameState->FindComponentByClass<UFBExperienceManagerComponent>();
	check(ExperienceManagerComponent);

	return ExperienceManagerComponent->IsExperienceLoaded();
}

void AFBGameModeBase::OnExperienceLoaded(const UFBExperienceDefinition* CurrentExperience)
{
	UFBGameSingleton::Get().InitializeTagToDataTable(CurrentExperience->GameData);

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		APlayerController* PC = Cast<APlayerController>(*It);

		if (PC && PC->GetPawn() == nullptr)
		{
			if (PlayerCanRestart(PC))
			{
				FFBSavePoint SavePoint;
				if (TryGetSavePointFromRecentSave((SavePoint)))
				{
					FTransform SpawnTransform(SavePoint.Rotator, SavePoint.Location);
					RestartPlayerAtTransform(PC, SpawnTransform);
				}
				else
				{
					RestartPlayer(PC);
				}
			}
		}
	}
}

bool AFBGameModeBase::TryGetSavePointFromRecentSave(FFBSavePoint& InSaveGame)
{
	if (UFBSaveGameSubsystem* SaveGameSubsystem = GetGameInstance()->GetSubsystem<UFBSaveGameSubsystem>())
	{
		if (UFBSaveGame* SaveGame = SaveGameSubsystem->GetRecentLoadSaveGame())
		{
			if (IFBSavePointInterface* Interface = Cast<IFBSavePointInterface>(SaveGame))
			{
				const FFBSavePoint& SavePoint = Interface->GetSavePoint();
				if (IsValidSavePoint(SavePoint))
				{
					InSaveGame = SavePoint;
					return true;
				}
			}
		}
	}
	return false;
}

bool AFBGameModeBase::IsValidSavePoint(const FFBSavePoint& InSavePoint) const
{
	return (InSavePoint.Location != FVector::ZeroVector && InSavePoint.Rotator != FRotator::ZeroRotator);
}

const UFBPawnData* AFBGameModeBase::GetPawnDataForController(const AController* InController) const
{
	if (InController)
	{
		if (const AFBPlayerState* PS = InController->GetPlayerState<AFBPlayerState>())
		{
			if (const UFBPawnData* PawnData = PS->GetPawnData<UFBPawnData>())
			{
				return PawnData;
			}
		}
	}

	check(GameState);
	UFBExperienceManagerComponent* ExperienceManagerComponent = GameState->FindComponentByClass<UFBExperienceManagerComponent>();
	check(ExperienceManagerComponent);

	if (ExperienceManagerComponent->IsExperienceLoaded())
	{
		const UFBExperienceDefinition* Experience = ExperienceManagerComponent->GetCurrentExperienceChecked();
		if (Experience->DefaultPawnData)
		{
			return Experience->DefaultPawnData;
		}
	}

	return nullptr;
}

void AFBGameModeBase::BeginResurrection()
{
	if (OnPlayerResurrectionDelegate.IsBound())
	{
		OnPlayerResurrectionDelegate.Broadcast();
	}
}

void AFBGameModeBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	OnPlayerResurrectionDelegate.Clear();
}
