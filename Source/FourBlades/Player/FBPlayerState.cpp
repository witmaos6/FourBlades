// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/FBPlayerState.h"
#include "GameModes/FBExperienceManagerComponent.h"
#include "AbilitySystem/FBAbilitySystemComponent.h"
#include "GameModes/FBGameModeBase.h"
#include "Character/FBPawnData.h"
#include "AbilitySystem/FBAbilitySet.h"
#include "AbilitySystem/Attributes/FBHealthSet.h"
#include "AbilitySystem/Attributes/FBCombatSet.h"
#include "AbilitySystem/Attributes/FBGuardSet.h"
#include "System/FBGameSingleton.h"
#include "System/FBSaveGameSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "SaveGame/Interface/FBSavePointInterface.h"
#include "SaveGame/FBSaveGame.h"
#include "System/FBSaveGameSubsystem.h"
#include "FourBlades.h"

AFBPlayerState::AFBPlayerState(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	AbilitySystemComponent = ObjectInitializer.CreateDefaultSubobject<UFBAbilitySystemComponent>(this, TEXT("AbilitySystemComponent"));

	FBHealthSet = CreateDefaultSubobject<UFBHealthSet>(TEXT("HealthSet"));
	FBCombatSet = CreateDefaultSubobject<UFBCombatSet>(TEXT("CombatSet"));
	FBGuardSet = CreateDefaultSubobject<UFBGuardSet>(TEXT("GuardSet"));
}

void AFBPlayerState::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	bOnPostInitializeComponent = true;

	check(AbilitySystemComponent);
	{
		FGameplayAbilityActorInfo* ActorInfo = AbilitySystemComponent->AbilityActorInfo.Get();
		check(ActorInfo->OwnerActor == this);
		check(ActorInfo->OwnerActor == ActorInfo->AvatarActor);
	}
	AbilitySystemComponent->InitAbilityActorInfo(this, GetPawn()); // 이 때는 GetPawn() == nullptr 이다.

	AbilitySystemComponent->AddLooseGameplayTag(FBTAG_CHARACTER_STATE_NORMAL);

	if (FBHealthSet)
	{
		FBHealthSet->OnOutOfHealth.AddUObject(this, &AFBPlayerState::SetDead);
	}

	UFBSaveGameSubsystem* FBSaveGameSubsystem = GetGameInstance()->GetSubsystem<UFBSaveGameSubsystem>();
	if (FBSaveGameSubsystem)
	{
		UFBSaveGame* SaveGame = FBSaveGameSubsystem->GetRecentLoadSaveGame();
		if (SaveGame)
		{
			SaveData = SaveGame;
		}
	}

	AGameStateBase* GameState = GetWorld()->GetGameState();
	check(GameState);
	UFBExperienceManagerComponent* ExperienceManagerComponent = GameState->FindComponentByClass<UFBExperienceManagerComponent>();
	check(ExperienceManagerComponent);
	ExperienceManagerComponent->CallOrRegister_OnExperienceLoaded(FOnFBExperienceLoaded::FDelegate::CreateUObject(this, &ThisClass::OnExperienceLoaded));
	if (OnPostInitializeComponent.IsBound())
	{
		OnPostInitializeComponent.Broadcast();
		OnPostInitializeComponent.Clear();
	}

	if (AFBGameModeBase* GameMode = Cast<AFBGameModeBase>(UGameplayStatics::GetGameMode(this)))
	{
		GameMode->OnPlayerResurrectionDelegate.AddUObject(this, &AFBPlayerState::Resurrection);
	}
}

void AFBPlayerState::OnExperienceLoaded(const UFBExperienceDefinition* CurrentExperience)
{
	if (AFBGameModeBase* GameMode = GetWorld()->GetAuthGameMode<AFBGameModeBase>())
	{
		const UFBPawnData* NewPawnData = GameMode->GetPawnDataForController(GetOwningController());
		check(NewPawnData);

		SetPawnData(NewPawnData);
	}
}

void AFBPlayerState::SetPawnData(const UFBPawnData* InPawnData)
{
	check(InPawnData);
	check(!PawnData);

	PawnData = InPawnData;

	for (UFBAbilitySet* AbilitySet : PawnData->AbilitySets)
	{
		if (AbilitySet)
		{
			AbilitySet->GiveToAbilitySystem(AbilitySystemComponent, nullptr);
		}
	}
}

void AFBPlayerState::InitSavePoint(APawn* InPlayerPawn)
{
	if (SaveData && InPlayerPawn)
	{
		if (IFBSavePointInterface* SavePointInterfacfe = Cast<IFBSavePointInterface>(SaveData))
		{
			const FFBSavePoint& SavePoint = SavePointInterfacfe->GetSavePoint();
			if (SavePoint.Location == FVector::ZeroVector && SavePoint.Rotator == FRotator::ZeroRotator)
			{
				SavePointInterfacfe->SetSavePoint(InPlayerPawn->GetActorLocation(), InPlayerPawn->GetActorRotation());

				if (UFBSaveGameSubsystem* FBSaveGameSubsystem = UGameplayStatics::GetGameInstance(GetWorld())->GetSubsystem<UFBSaveGameSubsystem>())
				{
					if (FBSaveGameSubsystem->SaveGameSlot(SaveData))
					{
						FB_FUNC_LOG(LogTemp, Log, TEXT("New Game Start Save Logic Success"));
					}
					else
					{
						FB_FUNC_LOG(LogTemp, Log, TEXT("New Game Start Save Logic Failed"));
					}
				}
			}
		}
	}
}

void AFBPlayerState::SetDead()
{
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->AddLooseGameplayTag(FBTAG_CHARACTER_STATE_DEAD);
		AbilitySystemComponent->CancelAllAbilities();

		FGameplayTagContainer TagContainer;
		TagContainer.AddTag(FBTAG_CHARACTER_STATE_DEAD);
		AbilitySystemComponent->TryActivateAbilitiesByTag(TagContainer);
	}
}

void AFBPlayerState::Resurrection()
{
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->RemoveLooseGameplayTag(FBTAG_CHARACTER_STATE_DEAD);
	}

	if (FBHealthSet)
	{
		FBHealthSet->OnOutOfHealth.AddUObject(this, &AFBPlayerState::SetDead);
	}

	if (SaveData)
	{
		if (IFBSavePointInterface* SavePointInterfacfe = Cast<IFBSavePointInterface>(SaveData))
		{
			const FFBSavePoint& SavePoint = SavePointInterfacfe->GetSavePoint();
			if (APawn* Pawn = GetPawn()) // FBCharacter로 이전할 수도 있다.
			{
				Pawn->SetActorLocation(SavePoint.Location);
				Pawn->SetActorRotation(SavePoint.Rotator);
			}
		}
	}
}
