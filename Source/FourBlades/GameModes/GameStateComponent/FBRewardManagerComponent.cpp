// Fill out your copyright notice in the Description page of Project Settings.


#include "GameModes/GameStateComponent/FBRewardManagerComponent.h"
#include "Enemy/FBEnemyCharacterBase.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/Attributes/FBHealthSet.h"
#include "GameFramework/PlayerState.h"
#include "Player/PlayerComponent/FBCharacterStatComponent.h"
#include "System/EnemyDataSubsystem.h"
#include "FourBlades.h"

UFBRewardManagerComponent::UFBRewardManagerComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{

}

void UFBRewardManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	APawn::OnPawnBeginPlay.AddUObject(this, &UFBRewardManagerComponent::EnemyBeginPlay);
}

void UFBRewardManagerComponent::EnemyBeginPlay(APawn* Pawn)
{
	AFBEnemyCharacterBase* EnemyBase = Cast<AFBEnemyCharacterBase>(Pawn);
	if (EnemyBase == nullptr)
		return;

	EnemyBase->CallOrRegister_OnInitAbilityActorInfo(FOnInitAbilityActorInfo::FDelegate::CreateUObject(this, &UFBRewardManagerComponent::EnemyInitAbilityActorInfo));
}

void UFBRewardManagerComponent::EnemyInitAbilityActorInfo(UAbilitySystemComponent* ASC)
{
	if (ASC == nullptr)
	{
		return;
	}

	const UFBHealthSet* HealthSet = ASC->GetSet<UFBHealthSet>();
	if (HealthSet)
	{
		HealthSet->OnOutOfHealthWithInstigator.AddUObject(this, &UFBRewardManagerComponent::XPReward);
	}
}

void UFBRewardManagerComponent::XPReward(const FGameplayTag KilledPawnTag, AActor* Instigator, AActor* EffectCauser)
{
	if (Instigator == nullptr)
		return;

	APlayerState* PS = Cast<APlayerState>(Instigator); // AFBPlayerState의 함수를 호출하고 AFBPlayerState가 델리게이트를 실행시켜 컴포넌트가 받을 수 있도록 설정할 수도 있다.
	if (PS)
	{
		UFBCharacterStatComponent* CharacterStatComponent = PS->FindComponentByClass<UFBCharacterStatComponent>(); // 컴포넌트의 종속성이 생겨 수정할 수도 있다.
		if (CharacterStatComponent)
		{
			if (UEnemyDataSubsystem* EnemyDataSubsystem = GetOwner()->GetGameInstance()->GetSubsystem<UEnemyDataSubsystem>())
			{
				uint32 EnemyXP = EnemyDataSubsystem->GetEnemyXP(KilledPawnTag);
				FB_FUNC_LOG(LogTemp, Log, TEXT("GetXP : %u"), EnemyXP);
				CharacterStatComponent->AddXP(EnemyXP);
			}
		}
	}
}
