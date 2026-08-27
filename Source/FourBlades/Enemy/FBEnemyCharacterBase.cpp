// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/FBEnemyCharacterBase.h"
#include "AI/FBAIController.h"
#include "AbilitySystem/Attributes/FBHealthSet.h"
#include "AbilitySystem/Attributes/FBCombatSet.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Components/CapsuleComponent.h"
#include "AbilitySystem/FBEnemyAbilitySystemComponent.h"
#include "FourBlades.h"

AFBEnemyCharacterBase::AFBEnemyCharacterBase()
{
	AbilitySystemComponent = CreateDefaultSubobject<UFBEnemyAbilitySystemComponent>(TEXT("AbilitySystemComponent"));

	FBHealthSet = CreateDefaultSubobject<UFBHealthSet>(TEXT("HealthSet"));
	FBCombatSet = CreateDefaultSubobject<UFBCombatSet>(TEXT("CombatSet"));

	GetCapsuleComponent()->SetCollisionProfileName(TEXT("FBEnemyCapsule"));

	AIControllerClass = AFBAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	Grade = EEnemyGrade::Normal;
}

void AFBEnemyCharacterBase::SetEnemyKey(const FGameplayTag& InEnemyKey)
{
	if (InEnemyKey.IsValid())
	{
		if (InEnemyKey.MatchesTag(FBTAG_ENEMYKEY))
		{
			EnemyKey = InEnemyKey;
		}
		else
		{
			FB_FUNC_LOG(LogTemp, Error, TEXT("InEnemyKey is invalid"));
		}
	}
}

void AFBEnemyCharacterBase::PostInitializeComponents()
{	
	// EnemyKey가 설정되지 않는다면 Enemy는 아무런 설정을 가지지 못하게 된다. 그러므로 SpawnActorDeffered를 이용하여 지연 스폰을 해야한다.
	// 다음틱에 스폰하지 않아도 PostInitializeComponents함수가 설정되는 타이밍에 EnemyKey는 부여된다.
	Super::PostInitializeComponents();
	// Super::PostInitializeComponents()함수가 실행되고 PossessedBy함수 실행이 완료된 후에 나머지 로직이 실행된다.	
}

void AFBEnemyCharacterBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	AbilitySystemComponent->InitAbilityActorInfo(this, this);
	AbilitySystemComponent->EnforceSingleStateTag();

	if (OnInitAbilityActorInfo.IsBound())
	{
		OnInitAbilityActorInfo.Broadcast(AbilitySystemComponent);
		OnInitAbilityActorInfo.Clear();
		bInitAbilityComplete = true;
	}
}

void AFBEnemyCharacterBase::CallOrRegister_OnInitAbilityActorInfo(FOnInitAbilityActorInfo::FDelegate&& Delegate)
{
	if (bInitAbilityComplete)
	{
		Delegate.Execute(AbilitySystemComponent);
	}
	else
	{
		OnInitAbilityActorInfo.Add(MoveTemp(Delegate));
	}
}

void AFBEnemyCharacterBase::BeginPlay()
{
	Super::BeginPlay();
}

void AFBEnemyCharacterBase::Destroy()
{
	SetLifeSpan(3.0f);
}
