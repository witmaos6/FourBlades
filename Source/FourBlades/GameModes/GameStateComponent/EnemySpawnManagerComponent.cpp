// Fill out your copyright notice in the Description page of Project Settings.


#include "GameModes/GameStateComponent/EnemySpawnManagerComponent.h"
#include "Enemy/FBEnemyCharacterBase.h"
#include "AbilitySystemComponent.h"
#include "GameModes/GameStateComponent/FBTargetPoint.h"
#include "EngineUtils.h"
#include "System/EnemyDataSubsystem.h"
#include "System/FBAssetManager.h"
#include "Enemy/EnemyData/EnemyPawnData.h"
#include "Enemy/EnemyData/FBEnemyAbilitySet.h"
#include "FourBlades.h"

UEnemySpawnManagerComponent::UEnemySpawnManagerComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	NrOfSpawn = 5;
	CurrentSpawn = 0;
	NrOfCurrentEnemy = 0;
	SpawnType = EEnemySpawnType::RangeSpawn;

	bWantsInitializeComponent = true;
}

void UEnemySpawnManagerComponent::InitializeComponent()
{
	if (UEnemyDataSubsystem* EnemyDataSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UEnemyDataSubsystem>())
	{
		if (!SpawnEnemyKeys.IsEmpty())
		{
			UFBAssetManager& AssetManager = UFBAssetManager::Get();
			
			for (const FGameplayTag& EnemyKey : SpawnEnemyKeys)
			{
				FPrimaryAssetId EnemyPawnAssetId = EnemyDataSubsystem->GetEnemyPawnDataAssetIdFromEnemyKey(EnemyKey);
				if (EnemyPawnAssetId.IsValid())
				{
					UEnemyPawnData* EnemyPawnData = AssetManager.LoadPrimaryAssetSync<UEnemyPawnData>(EnemyPawnAssetId);
					if (EnemyPawnData)
					{
						EnemyPawnDataMap.Add(EnemyKey, EnemyPawnData);
					}
				}

				FPrimaryAssetId EnemyAbilitiesAssetId = EnemyDataSubsystem->GetEnemyAbilitiesDataAssetIdFromEnemyKey(EnemyKey);
				if (EnemyAbilitiesAssetId.IsValid())
				{
					UFBEnemyAbilitySet* EnemyAbilities = AssetManager.LoadPrimaryAssetSync<UFBEnemyAbilitySet>(EnemyAbilitiesAssetId);
					if (EnemyAbilities)
					{
						EnemyAbilitiesMap.Add(EnemyKey, EnemyAbilities);
					}
				}
			}
		}
	}

	Super::InitializeComponent();
}

void UEnemySpawnManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	if (SpawnType == EEnemySpawnType::RangeSpawn)
	{
		GetWorld()->GetTimerManager().SetTimer(SpawnTimer, this, &UEnemySpawnManagerComponent::SpawnEnemyByRange, 1.0f, true);
	}
	else if (SpawnType == EEnemySpawnType::TargetPointSpawn)
	{
		for (AFBTargetPoint* TargetPoint : TActorRange<AFBTargetPoint>(GetWorld()))
		{
			if (TargetPoint)
			{
				AddTargetPoint(TargetPoint);
			}
		}
		GetWorld()->GetTimerManager().SetTimer(SpawnTimer, this, &UEnemySpawnManagerComponent::SpawnEnemyByTargetPoint, 0.2f, true, 1.0f);
	}
}

void UEnemySpawnManagerComponent::SpawnEnemyByRange()
{
	float XBoundary = FMath::FRandRange(-2000.f, 2000.f);
	float YBoundary = FMath::FRandRange(-2000.f, 2000.f);
	float ZVelocity = 100.f;
	FTransform SpawnTransform = FTransform(FVector(XBoundary, YBoundary, ZVelocity));

	if (EnemyBase)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		AFBEnemyCharacterBase* Enemy = GetWorld()->SpawnActor<AFBEnemyCharacterBase>(EnemyBase, SpawnTransform, SpawnParams);
		if (Enemy)
		{
			Enemy->OnDestroyed.AddDynamic(this, &UEnemySpawnManagerComponent::DestroyCountEnemy);
			NrOfCurrentEnemy++;
		}
	}

	CurrentSpawn++;
	if (CurrentSpawn >= NrOfSpawn)
	{
		GetWorld()->GetTimerManager().ClearTimer(SpawnTimer);
	}
}

void UEnemySpawnManagerComponent::SpawnEnemyByTargetPoint()
{
	if (CurrentSpawn < static_cast<uint32>(TargetPoints.Num()))
	{
		FVector SpawnLocation = TargetPoints[CurrentSpawn]->GetActorLocation();
		FTransform SpawnTransform = FTransform(SpawnLocation);

		if (EnemyBase)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
			AFBEnemyCharacterBase* Enemy = GetWorld()->SpawnActorDeferred<AFBEnemyCharacterBase>(EnemyBase, SpawnTransform, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);
			if (Enemy)
			{
				if (!SpawnEnemyKeys.IsEmpty())
				{
					int32 RandomIndex = FMath::RandRange(0, SpawnEnemyKeys.Num() - 1);
					if (SpawnEnemyKeys.IsValidIndex(RandomIndex))
					{
						Enemy->SetEnemyKey(SpawnEnemyKeys.GetByIndex(RandomIndex));
					}
				}
				TargetPoints[CurrentSpawn]->SpawnedEnemyDestroyBind(Enemy);

				Enemy->FinishSpawning(SpawnTransform);
				// 현재는 EnemyKey설정에 문제가 없다 문제가 생긴다면 다음틱에 스폰하는 방식으로 전환해야 한다.
			}
		}
		CurrentSpawn++;
	}
	else
	{
		GetWorld()->GetTimerManager().ClearTimer(SpawnTimer);
	}
}

void UEnemySpawnManagerComponent::DestroyCountEnemy(AActor* EnemyCharacter)
{
	if (SpawnType == EEnemySpawnType::RangeSpawn)
	{
		NrOfCurrentEnemy--;

		if (NrOfCurrentEnemy == 0 && CurrentSpawn >= NrOfSpawn)
		{
			if (BossEnemy)
			{
				float XBoundary = FMath::FRandRange(-2000.f, 2000.f);
				float YBoundary = FMath::FRandRange(-2000.f, 2000.f);
				float ZVelocity = 100.f;
				FTransform SpawnTransform = FTransform(FVector(XBoundary, YBoundary, ZVelocity));
				GetWorld()->SpawnActor<AFBEnemyCharacterBase>(BossEnemy, SpawnTransform);
			}
		}
	}
}

void UEnemySpawnManagerComponent::RequestRespawn(AFBTargetPoint* InTargetPoint)
{
	if (InTargetPoint == nullptr)
	{
		return;
	}

	FVector SpawnLocation = InTargetPoint->GetActorLocation();
	FTransform SpawnTransform = FTransform(SpawnLocation);

	if (EnemyBase)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		AFBEnemyCharacterBase* Enemy = GetWorld()->SpawnActorDeferred<AFBEnemyCharacterBase>(EnemyBase, SpawnTransform, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);
		if (Enemy)
		{
			if (!SpawnEnemyKeys.IsEmpty())
			{
				int32 RandomIndex = FMath::RandRange(0, SpawnEnemyKeys.Num() - 1);
				if (SpawnEnemyKeys.IsValidIndex(RandomIndex))
				{
					Enemy->SetEnemyKey(SpawnEnemyKeys.GetByIndex(RandomIndex));
				}
			}

			Enemy->FinishSpawning(SpawnTransform);

			InTargetPoint->SpawnedEnemyDestroyBind(Enemy);
		}
	}
}
