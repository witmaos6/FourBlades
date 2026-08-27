// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/FBEnemyCharacterBase.h"
#include "Interface/AI/FBAICharacterInterface.h"
#include "Enemy/Interface/FBEnemyDataInterface.h"
#include "MadSlasherCharacter.generated.h"

/**
 * 
 */
UCLASS()
class FOURBLADES_API AMadSlasherCharacter : public AFBEnemyCharacterBase, public IFBAICharacterInterface, public IFBEnemyDataInterface
{
	GENERATED_BODY()
public:
	AMadSlasherCharacter();

	virtual void PossessedBy(AController* NewController) override;

protected:
	virtual void BeginPlay() override;

// To do: 캐릭터의 데이터들을 DataAsset으로 관리할 수도 있음
	UPROPERTY(EditDefaultsOnly, Category = "Stat")
	TSubclassOf<class UGameplayEffect> StatInit;

	UPROPERTY(EditDefaultsOnly, Category = "Widget")
	TSubclassOf<class UUserWidget> HealthWidgetClass;

	UPROPERTY()
	TObjectPtr<class UFBGASUserWidget> HealthWidget;

	bool bOutOfHealth = false;

public: // IFBAICharacterInterface
	virtual float GetPatrolRadius() override { return 500.f; }
	virtual float GetPlayerDetectDistance() override { return 0.0f; } // 감지하는 방식이 아니기 때문에 이후에 인터페이스 분리

public: // IFBEnemyDataInterface

	virtual class UFBAbilityFragmentManager* GetAbilityFragmentManager(const FGameplayTag& InAbilityTag) override;

	virtual const float GetAbilityActivateMinRange(EAbilityConditionType InConditionType) override;

	virtual const float GetAbilityActivateMaxRange(EAbilityConditionType InConditionType) override;

	virtual const float GetAbilityActivateValidRadius(EAbilityConditionType InConditionType) override;

	virtual FGameplayTag GetAbilityTagWithWeight(EAbilityConditionType InConditionType) override;

	virtual class UFBAbilityFragmentManager* GetReactAbilityFragmentManager(const FGameplayTag& InAbilityTag) override;

public:
	void SetDead();

protected:
	void ApplyTagEffect(TSubclassOf<UGameplayEffect>& EffectClass);

	void PlayerResurrection();

protected:
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
};
