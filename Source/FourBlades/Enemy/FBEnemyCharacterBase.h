// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/FBCharacterBase.h"
#include "GameplayTagContainer.h"
#include "EnemyData/EnemyPawnData.h"
#include "FBEnemyCharacterBase.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnInitAbilityActorInfo, class UAbilitySystemComponent* ASC);

UENUM(BlueprintType)
enum class EEnemyGrade : uint8
{
	Normal,
	Boss,
};

/**
 * 
 */
UCLASS()
class FOURBLADES_API AFBEnemyCharacterBase : public AFBCharacterBase
{
	GENERATED_BODY()
public:
	AFBEnemyCharacterBase();

	void SetEnemyKey(const FGameplayTag& InEnemyKey);

	virtual void PostInitializeComponents() override;

	virtual void PossessedBy(AController* NewController) override;

	FOnInitAbilityActorInfo OnInitAbilityActorInfo;

	void CallOrRegister_OnInitAbilityActorInfo(FOnInitAbilityActorInfo::FDelegate&& Delegate);

	bool bInitAbilityComplete = false;

protected:
	virtual void BeginPlay() override;

public: // ASC
	class UFBEnemyAbilitySystemComponent* GetAbilitySystemComponent() const { return AbilitySystemComponent; }

protected:
	UPROPERTY(VisibleAnywhere, Category = "ASC")
	TObjectPtr<class UFBEnemyAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<class UFBHealthSet> FBHealthSet;

	UPROPERTY()
	TObjectPtr<class UFBCombatSet> FBCombatSet;

// === EnemyData구조체를 만들어 래핑할 수 있는 변수들
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EnemyKey", Meta = (Categories = "EnemyKey"))
	FGameplayTag EnemyKey;

	UPROPERTY(EditDefaultsOnly, Category = "EnemyPawnData")
	TObjectPtr<UEnemyPawnData> EnemyPawnData;

	UPROPERTY(EditDefaultsOnly, Category = "EnemyAbilitiesData")
	TObjectPtr<class UFBEnemyAbilitySet> EnemyAbilitySet;
// === EnemyData 구조체를 만들어 래핑할 수 있는 변수들

public:
	const FGameplayTag GetEnemyKey() { return EnemyKey; }

protected:
	EEnemyGrade Grade;

public:
	EEnemyGrade GetGrade() const { return Grade; }

public:
	void Destroy();
};
