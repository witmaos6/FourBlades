// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/FBEnemyNormalBase.h"
#include "System/EnemyDataSubsystem.h"
#include "System/FBAssetManager.h"
#include "EnemyData/EnemyPawnData.h"
#include "GameDataFromExcel/FBEnemyStatFromLevelRow.h"
#include "AbilitySystem/FBEnemyAbilitySystemComponent.h"
#include "AI/FBAIController.h"
#include "GameplayTags/FBDefineGameplayTags.h"
#include "Enemy/EnemyData/FBEnemyAbilitySet.h"
#include "AbilitySystem/Attributes/FBHealthSet.h"
#include "AbilitySystem/Attributes/FBCombatSet.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AI/FBAIKey.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AbilityFragment/FBAbilityFragmentManager.h"
#include "FourBlades.h"

AFBEnemyNormalBase::AFBEnemyNormalBase()
{
	Grade = EEnemyGrade::Normal;

	bOutOfHealth = false;

	GetCharacterMovement()->MaxWalkSpeed = 300.f;
	PlayerDetectKey = FBlackboard::InvalidKey;
}

void AFBEnemyNormalBase::PostInitializeComponents()
{
	if (UGameInstance* GameInstance = GetGameInstance())
	{
		if (UEnemyDataSubsystem* EnemyDataSubsystem = GetGameInstance()->GetSubsystem<UEnemyDataSubsystem>())
		{
			if (EnemyKey.IsValid())
			{
				FPrimaryAssetId PawnAssetId = EnemyDataSubsystem->GetEnemyPawnDataAssetIdFromEnemyKey(EnemyKey);
				if (PawnAssetId.IsValid())
				{
					UFBAssetManager& AssetManager = UFBAssetManager::Get();
					UEnemyPawnData* LoadEnemyPawnData = AssetManager.LoadPrimaryAssetSync<UEnemyPawnData>(PawnAssetId);
					if (LoadEnemyPawnData)
					{
						EnemyPawnData = LoadEnemyPawnData;

						SettingPawnFromData();
					}
				}
			}
		}
	}

	Super::PostInitializeComponents();
}

void AFBEnemyNormalBase::SettingPawnFromData()
{
	if (EnemyPawnData == nullptr)
	{
		FB_FUNC_LOG(LogTemp, Warning, TEXT("EnemyPawnData is null"));
		return;
	}
	USkeletalMeshComponent* SkeletalMesh = GetMesh();
	if (SkeletalMesh == nullptr)
	{
		return;
	}
	SkeletalMesh->SetSkeletalMesh(EnemyPawnData->Mesh);
	SkeletalMesh->SetAnimInstanceClass(EnemyPawnData->AnimInstance);

	for (const FAttachEquipActor& EquipActorSubclass : EnemyPawnData->EquipActors)
	{
		AActor* EquipActor = GetWorld()->SpawnActor<AActor>(EquipActorSubclass.EquipActor, GetTransform());
		if (EquipActor)
		{
			EquipActor->AttachToComponent(SkeletalMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, EquipActorSubclass.SocketName);
			AttachedActor.Add(EquipActor);
		}
	}
}

void AFBEnemyNormalBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	AbilitySystemComponent->AddLooseGameplayTag(FBTAG_ENEMY_STATE_IDLE);

	if (AFBAIController* AIController = Cast<AFBAIController>(NewController))
	{
		AIController->SetBehaviorTree(EnemyPawnData->BehaviorTree);

		AIController->RunAI();
	}

	GiveAbilities();

	SetOnCombatEvent();

	InitializeStat();
}

void AFBEnemyNormalBase::GiveAbilities()
{
	if (!EnemyKey.IsValid())
	{
		FB_FUNC_LOG(LogTemp, Warning, TEXT("EnemyKey is null"));
		return;
	}
	if (UEnemyDataSubsystem* EnemyDataSubsystem = GetGameInstance()->GetSubsystem<UEnemyDataSubsystem>())
	{
		FPrimaryAssetId AbilityAssetId = EnemyDataSubsystem->GetEnemyAbilitiesDataAssetIdFromEnemyKey(EnemyKey);
		if (AbilityAssetId.IsValid())
		{
			UFBAssetManager& AssetManager = UFBAssetManager::Get();
			UFBEnemyAbilitySet* LoadEnemyAbilities = AssetManager.LoadPrimaryAssetSync<UFBEnemyAbilitySet>(AbilityAssetId);
			if (LoadEnemyAbilities)
			{
				EnemyAbilitySet = LoadEnemyAbilities;
				for (const FFBConditionAbilities& ConditionToAbilities : LoadEnemyAbilities->AbilityList)
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
			}
		}
	}
}

void AFBEnemyNormalBase::SetOnCombatEvent()
{
	if (FBHealthSet)
	{
		FBHealthSet->OnOutOfHealth.AddUObject(this, &AFBEnemyNormalBase::SetDead);
	}
}

void AFBEnemyNormalBase::SetDead()
{
	bOutOfHealth = true;

	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
	SetActorEnableCollision(false);

	AbilitySystemComponent->CancelAllAbilities();

	FGameplayTagContainer DeadTag(FBTAG_ENEMY_ABILITY_DEATH);
	AbilitySystemComponent->TryActivateAbilitiesByTag(DeadTag);

	if (AFBAIController* AIController = Cast<AFBAIController>(GetController()))
	{
		AIController->StopAI();

		if (UBlackboardComponent* BlackboardComponent = AIController->GetBlackboardComponent())
		{
			if (PlayerDetectKey != FBlackboard::InvalidKey && PlayerDetectHandle.IsValid())
			{
				BlackboardComponent->UnregisterObserver(PlayerDetectKey, PlayerDetectHandle);
			}
		}
	}

	for (AActor* EquipActor : AttachedActor)
	{
		if (IsValid(EquipActor))
		{
			EquipActor->Destroy();
		}
	}

	Destroy();
}

void AFBEnemyNormalBase::BeginPlay()
{
	Super::BeginPlay();

	if (AFBAIController* AIController = Cast<AFBAIController>(GetController()))
	{
		if (UBlackboardComponent* BlackboardComponent = AIController->GetBlackboardComponent())
		{
			PlayerDetectKey = BlackboardComponent->GetKeyID(FName(BBKey_DetectPlayer));
			if (PlayerDetectKey != FBlackboard::InvalidKey)
			{
				PlayerDetectHandle = BlackboardComponent->RegisterObserver(PlayerDetectKey, this, FOnBlackboardChangeNotification::CreateUObject(this, &AFBEnemyNormalBase::PlayerDetected));
			}
		}
	}
}

EBlackboardNotificationResult AFBEnemyNormalBase::PlayerDetected(const UBlackboardComponent& BlackboardComp, FBlackboard::FKey KeyID)
{
	if (PlayerDetectKey == KeyID)
	{
		AActor* Target = Cast<AActor>(BlackboardComp.GetValueAsObject(FName(BBKey_DetectPlayer)));
		if (Target)
		{
			GetCharacterMovement()->MaxWalkSpeed = 600.f;
		}
		else
		{
			GetCharacterMovement()->MaxWalkSpeed = 300.f;
		}
	}
	return EBlackboardNotificationResult::ContinueObserving;
}

void AFBEnemyNormalBase::InitializeStat()
{
	if (!EnemyKey.IsValid())
	{
		FB_FUNC_LOG(LogTemp, Warning, TEXT("EnemyKey invalid"));
		return;
	}
	UEnemyDataSubsystem* EnemyDataSubsystem = GetGameInstance()->GetSubsystem<UEnemyDataSubsystem>();
	if (EnemyDataSubsystem == nullptr)
	{
		return;
	}
	uint8 EnemyLevel = EnemyDataSubsystem->GetEnemyLevel(EnemyKey);
	if (EnemyLevel == -1)
	{
		FB_FUNC_LOG(LogTemp, Warning, TEXT("Invalid Level"));
		return;
	}
	FFBEnemyStatFromLevelRow* EnemyStatRow = EnemyDataSubsystem->GetEnemyStatFromLevelRow(EnemyLevel);
	if (EnemyStatRow == nullptr)
	{
		return;
	}

	if (AbilitySystemComponent)
	{
		UGameplayEffect* StatInitEffect = NewObject<UGameplayEffect>(GetTransientPackage(), FName("StatInitEffect"));
		StatInitEffect->DurationPolicy = EGameplayEffectDurationType::Instant;

		StatInitEffect->Modifiers.SetNum(4);

		StatInitEffect->Modifiers[0].Attribute = UFBHealthSet::GetMaxHealthAttribute();
		StatInitEffect->Modifiers[0].ModifierOp = EGameplayModOp::Override;
		StatInitEffect->Modifiers[0].ModifierMagnitude = FScalableFloat(EnemyStatRow->MaxHealth);

		StatInitEffect->Modifiers[1].Attribute = UFBHealthSet::GetHealthAttribute();
		StatInitEffect->Modifiers[1].ModifierOp = EGameplayModOp::Override;
		StatInitEffect->Modifiers[1].ModifierMagnitude = FScalableFloat(EnemyStatRow->MaxHealth);

		StatInitEffect->Modifiers[2].Attribute = UFBHealthSet::GetDefenseAttribute();
		StatInitEffect->Modifiers[2].ModifierOp = EGameplayModOp::Override;
		StatInitEffect->Modifiers[2].ModifierMagnitude = FScalableFloat(EnemyStatRow->Defense);

		StatInitEffect->Modifiers[3].Attribute = UFBCombatSet::GetAttackPowerAttribute();
		StatInitEffect->Modifiers[3].ModifierOp = EGameplayModOp::Override;
		StatInitEffect->Modifiers[3].ModifierMagnitude = FScalableFloat(EnemyStatRow->AttackPower);

		FGameplayEffectContextHandle EffectContextHandle = AbilitySystemComponent->MakeEffectContext();
		EffectContextHandle.AddSourceObject(this);
		AbilitySystemComponent->ApplyGameplayEffectToSelf(StatInitEffect, 1.0f, EffectContextHandle);
	}
}

UFBAbilityFragmentManager* AFBEnemyNormalBase::GetAbilityFragmentManager(const FGameplayTag& InAbilityTag)
{
	return EnemyAbilitySet->GetAbilityFragmentManager(InAbilityTag);
}

const float AFBEnemyNormalBase::GetAbilityActivateMinRange(EAbilityConditionType InConditionType)
{
	if (EnemyAbilitySet == nullptr)
		return -1.0f;

	return EnemyAbilitySet->GetMinRange(InConditionType);
}

const float AFBEnemyNormalBase::GetAbilityActivateMaxRange(EAbilityConditionType InConditionType)
{
	if (EnemyAbilitySet == nullptr)
		return -1.0f;

	return EnemyAbilitySet->GetMaxRange(InConditionType);
}

const float AFBEnemyNormalBase::GetAbilityActivateValidRadius(EAbilityConditionType InConditionType)
{
	if (EnemyAbilitySet == nullptr)
		return -1.0f;

	return EnemyAbilitySet->GetValidRadius(InConditionType);
}

FGameplayTag AFBEnemyNormalBase::GetAbilityTagWithWeight(EAbilityConditionType InConditionType)
{
	if (EnemyAbilitySet == nullptr)
		return FGameplayTag::EmptyTag;

	return EnemyAbilitySet->GetAbilityTagWithWeight(InConditionType);
}

UFBAbilityFragmentManager* AFBEnemyNormalBase::GetReactAbilityFragmentManager(const FGameplayTag& InAbilityTag)
{
	if (EnemyAbilitySet == nullptr)
		return nullptr;

	return EnemyAbilitySet->GetReactAbilityFragmentManager(InAbilityTag);
}
