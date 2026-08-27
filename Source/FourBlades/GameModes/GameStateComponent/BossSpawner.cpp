// Fill out your copyright notice in the Description page of Project Settings.


#include "GameModes/GameStateComponent/BossSpawner.h"
#include "Components/BoxComponent.h"
#include "Enemy/FBEnemyCharacterBase.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/Attributes/FBHealthSet.h"
#include "FourBlades.h"

ABossSpawner::ABossSpawner()
{
	PrimaryActorTick.bStartWithTickEnabled = false;
	PrimaryActorTick.bCanEverTick = false;

	PlayerDetectBox = CreateDefaultSubobject<UBoxComponent>(TEXT("PlayerDetectBox"));
	PlayerDetectBox->SetCollisionProfileName(TEXT("FBInteractionQuery")); // Player Only
	PlayerDetectBox->SetBoxExtent(FVector(100.f, 100.f, 100.f));
	SetRootComponent(PlayerDetectBox);
	
	BossSpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("BossSpawnPoint"));
	BossSpawnPoint->SetupAttachment(RootComponent);
	BossSpawnPoint->SetRelativeLocation(FVector(0.0f, 0.0f, 100.f));

	WallTag = TEXT("Wall");
}

void ABossSpawner::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (PlayerDetectBox)
	{
		PlayerDetectBox->OnComponentBeginOverlap.AddDynamic(this, &ABossSpawner::OnBoxBeginOverlap);
	}
}

void ABossSpawner::BeginPlay()
{
	Super::BeginPlay();

	CollectWallComponents();

	InitializeWallState();
}

void ABossSpawner::CollectWallComponents()
{
	WallComponents.Empty();

	TArray<UStaticMeshComponent*> AllStaticMeshes;
	GetComponents<UStaticMeshComponent>(AllStaticMeshes);

	for (UStaticMeshComponent* Component : AllStaticMeshes)
	{
		if (Component && Component->ComponentTags.Contains(WallTag))
		{
			WallComponents.Add(Component);
		}
	}

	if (WallComponents.Num() == 0)
	{
		FB_FUNC_LOG(LogTemp, Warning, TEXT("No barrier components found with tag '%s'"), *WallTag.ToString());
	}
}

void ABossSpawner::InitializeWallState()
{
	for (UStaticMeshComponent* Wall : WallComponents)
	{
		if (IsValid(Wall))
		{
			Wall->SetVisibility(false);
			Wall->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
	}
}

void ABossSpawner::OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (APawn* Pawn = Cast<APawn>(OtherActor))
	{
		if (Pawn->IsLocallyControlled())
		{
			BossSpawn();

			ActivateWalls();

			PlayerDetectBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
	}
}

void ABossSpawner::BossSpawn()
{
	if (!BossClass)
	{
		FB_FUNC_LOG(LogTemp, Error, TEXT("BossClass is not set"));
		return;
	}
	if (!BossSpawnPoint)
	{
		FB_FUNC_LOG(LogTemp, Error, TEXT("BossSpawnPoint is not set"));
		return;
	}

	FTransform SpawnTransform = FTransform(BossSpawnPoint->GetComponentRotation(), BossSpawnPoint->GetComponentLocation());

	AFBEnemyCharacterBase* SpawnBoss = GetWorld()->SpawnActorDeferred<AFBEnemyCharacterBase>(BossClass, SpawnTransform, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);
	if (SpawnBoss)
	{
		// To do: Set Spawned Boss Info EX) EnemyTag, AbilitySet...

		SpawnBoss->FinishSpawning(SpawnTransform);
		SpawnBoss->CallOrRegister_OnInitAbilityActorInfo(FOnInitAbilityActorInfo::FDelegate::CreateUObject(this, &ABossSpawner::BossAbilityInit));

		SpawnBoss->OnDestroyed.AddDynamic(this, &ABossSpawner::DeactivateWalls);
	}
}

void ABossSpawner::ActivateWalls()
{
	for (UStaticMeshComponent* Wall : WallComponents)
	{
		if (IsValid(Wall))
		{
			Wall->SetVisibility(true);
			Wall->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		}
	}
}

void ABossSpawner::DeactivateWalls(AActor* BossCharacter)
{
	for (UStaticMeshComponent* Wall : WallComponents)
	{
		if (IsValid(Wall))
		{
			Wall->SetVisibility(false);
			Wall->SetCollisionEnabled(ECollisionEnabled::NoCollision);

			PlayerDetectBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		}
	}
}

void ABossSpawner::BossAbilityInit(UAbilitySystemComponent* BossASC)
{
	if (BossASC)
	{
		if (const UFBHealthSet* HealthSet = BossASC->GetSetChecked<UFBHealthSet>())
		{
			HealthSet->OnOutOfHealth.AddUObject(this, &ABossSpawner::BossDead);
		}
	}
}

void ABossSpawner::BossDead()
{
	SetLifeSpan(1.0f);
}
