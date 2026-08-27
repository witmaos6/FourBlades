// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Attributes/FBAttributeSet.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/FBGameplayEffectContext.h"
#include "FBHealthSet.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnOutOfHealth);
DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnOutOfHealthWithInstigator, const FGameplayTag KilledPawnTag, AActor* Instigator, AActor* EffectCauser);
DECLARE_DELEGATE_RetVal_OneParam(float, FOnCalculateDamageReduction, float Damage);
DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnApplyDamageToHealth, AActor* Instigator, AActor* EffectCauser, const FFBTargetPayload& TargetPayload);


/**
 * 
 */
UCLASS()
class FOURBLADES_API UFBHealthSet : public UFBAttributeSet
{
	GENERATED_BODY()
public:
	UFBHealthSet();

	ATTRIBUTE_ACCESSORS(UFBHealthSet, Health);
	ATTRIBUTE_ACCESSORS(UFBHealthSet, MaxHealth);
	ATTRIBUTE_ACCESSORS(UFBHealthSet, Defense);
	ATTRIBUTE_ACCESSORS(UFBHealthSet, Healing);
	ATTRIBUTE_ACCESSORS(UFBHealthSet, MetaDamage);

	UPROPERTY(BlueprintReadOnly, Category = "Health")
	FGameplayAttributeData Health;

	UPROPERTY(BlueprintReadOnly, Category = "Health")
	FGameplayAttributeData MaxHealth;

	UPROPERTY(BlueprintReadOnly, Category = "Health")
	FGameplayAttributeData Defense;

	UPROPERTY(BlueprintReadOnly, Category = "Health")
	FGameplayAttributeData Healing;

	UPROPERTY(BlueprintReadOnly, Category = "Health")
	FGameplayAttributeData MetaDamage;

public:
	mutable FOnCalculateDamageReduction OnCalculateDamageReduction;

	mutable FOnApplyDamageToHealth OnApplyDamageToHealth;

	mutable FOnOutOfHealth OnOutOfHealth;

	mutable FOnOutOfHealthWithInstigator OnOutOfHealthWithInstigator;

private:
	void HealthClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const;

	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;

	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

	virtual bool PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data) override;

	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	bool bOutOfHealth;

	const FGameplayTag GetPawnTag(); // 팩토리로 사용
};
