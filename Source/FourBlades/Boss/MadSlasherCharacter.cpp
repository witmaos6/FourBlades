// Fill out your copyright notice in the Description page of Project Settings.


#include "Boss/MadSlasherCharacter.h"
#include "AbilitySystem/FBEnemyAbilitySystemComponent.h"
#include "GameplayTags/FBDefineGameplayTags.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AI/FBAIController.h"
#include "AbilitySystem/Attributes/FBHealthSet.h"
#include "UI/FBGASUserWidget.h"
#include "AbilitySystem/FBGameplayEffectContext.h"
#include "GameModes/FBGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "FourBlades.h"

AMadSlasherCharacter::AMadSlasherCharacter()
{
	Grade = EEnemyGrade::Boss;
}

void AMadSlasherCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->AddLooseGameplayTag(FBTAG_ENEMY_STATE_IDLE);

		for (const FFBConditionAbilities& ConditionToAbilities : EnemyAbilitySet->AbilityList)
		{
			for (const FFBWeightedAbilityDefinition& AbilityDefintion : ConditionToAbilities.WeightedDefinitions)
			{
				FGameplayAbilitySpec Spec(AbilityDefintion.Definition.Ability.GetDefaultObject());
				AbilitySystemComponent->GiveAbility(Spec);
			}	
		}

		for (const FFBAbilityDefinition& AbilityDefinition : EnemyAbilitySet->HitReactAbilities)
		{
			FGameplayAbilitySpec Spec(AbilityDefinition.Ability.GetDefaultObject());
			AbilitySystemComponent->GiveAbility(Spec);
		}

		if (const UFBHealthSet* HealthSet = AbilitySystemComponent->GetSetChecked<UFBHealthSet>())
		{
			HealthSet->OnOutOfHealth.AddUObject(this, &AMadSlasherCharacter::SetDead);
		}
	}

	if (StatInit)
	{
		FGameplayEffectContextHandle EffectContextHandle = AbilitySystemComponent->MakeEffectContext();
		EffectContextHandle.AddSourceObject(this);
		FGameplayEffectSpecHandle EffectSpecHandle = AbilitySystemComponent->MakeOutgoingSpec(StatInit, 1.0f, EffectContextHandle);
		if (EffectSpecHandle.IsValid())
		{
			AbilitySystemComponent->BP_ApplyGameplayEffectSpecToSelf(EffectSpecHandle);
		}
	}
}

void AMadSlasherCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (HealthWidgetClass)
	{
		HealthWidget = CreateWidget<UFBGASUserWidget>(GetWorld(), HealthWidgetClass);
		HealthWidget->AddToPlayerScreen();
		HealthWidget->SetAbilitySystemComponent(this);
	}

	if (AFBGameModeBase* GameMode = Cast<AFBGameModeBase>(UGameplayStatics::GetGameMode(this)))
	{
		GameMode->OnPlayerResurrectionDelegate.AddUObject(this, &AMadSlasherCharacter::PlayerResurrection);
	}
}

UFBAbilityFragmentManager* AMadSlasherCharacter::GetAbilityFragmentManager(const FGameplayTag& InAbilityTag)
{
	return EnemyAbilitySet->GetAbilityFragmentManager(InAbilityTag);
}

const float AMadSlasherCharacter::GetAbilityActivateMinRange(EAbilityConditionType InConditionType)
{
	if (EnemyAbilitySet == nullptr)
		return -1.0f;

	return EnemyAbilitySet->GetMinRange(InConditionType);
}

const float AMadSlasherCharacter::GetAbilityActivateMaxRange(EAbilityConditionType InConditionType)
{
	if (EnemyAbilitySet == nullptr)
		return -1.0f;

	return EnemyAbilitySet->GetMaxRange(InConditionType);
}

const float AMadSlasherCharacter::GetAbilityActivateValidRadius(EAbilityConditionType InConditionType)
{
	if (EnemyAbilitySet == nullptr)
		return -1.0f;

	return EnemyAbilitySet->GetValidRadius(InConditionType);
}

FGameplayTag AMadSlasherCharacter::GetAbilityTagWithWeight(EAbilityConditionType InConditionType)
{
	if (EnemyAbilitySet == nullptr)
		return FGameplayTag::EmptyTag;

	return EnemyAbilitySet->GetAbilityTagWithWeight(InConditionType);
}

UFBAbilityFragmentManager* AMadSlasherCharacter::GetReactAbilityFragmentManager(const FGameplayTag& InAbilityTag)
{
	if (EnemyAbilitySet == nullptr)
		return nullptr;

	return EnemyAbilitySet->GetReactAbilityFragmentManager(InAbilityTag);
}

void AMadSlasherCharacter::SetDead()
{
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
	SetActorEnableCollision(false);

	bOutOfHealth = true;

	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->CancelAllAbilities();

		FGameplayTagContainer DeadTag(FBTAG_ENEMY_ABILITY_DEATH);
		AbilitySystemComponent->TryActivateAbilitiesByTag(DeadTag);
	}
	
	if (AFBAIController* AIController = Cast<AFBAIController>(GetController()))
	{
		AIController->StopAI();
	}
	Destroy();
}

void AMadSlasherCharacter::ApplyTagEffect(TSubclassOf<UGameplayEffect>& EffectClass)
{
	FGameplayEffectContextHandle EffectContextHandle = AbilitySystemComponent->MakeEffectContext();
	AbilitySystemComponent->BP_ApplyGameplayEffectToSelf(EffectClass, 1.0f, EffectContextHandle);
}

void AMadSlasherCharacter::PlayerResurrection()
{
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
	SetActorEnableCollision(false);

	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->CancelAllAbilities();
	}
	if (AFBAIController* AIController = Cast<AFBAIController>(GetController()))
	{
		AIController->StopAI();
	}
	SetLifeSpan(0.1f);
}

void AMadSlasherCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (HealthWidget)
	{
		HealthWidget->RemoveFromParent();
		HealthWidget->MarkAsGarbage();
		HealthWidget = nullptr;
	}
	Super::EndPlay(EndPlayReason);
}
