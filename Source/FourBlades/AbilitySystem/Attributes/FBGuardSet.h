// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Attributes/FBAttributeSet.h"
#include "AbilitySystemComponent.h"
#include "FBGuardSet.generated.h"

/**
 * 
 */
UCLASS()
class FOURBLADES_API UFBGuardSet : public UFBAttributeSet
{
	GENERATED_BODY()
public:
	UFBGuardSet();

	ATTRIBUTE_ACCESSORS(UFBGuardSet, GuardGauge);
	ATTRIBUTE_ACCESSORS(UFBGuardSet, MaxGuardGauge);
	ATTRIBUTE_ACCESSORS(UFBGuardSet, MetaGuardDamage);

	UPROPERTY(BlueprintReadOnly, Category = "Guard")
	FGameplayAttributeData GuardGauge;

	UPROPERTY(BlueprintReadOnly, Category = "Guard")
	FGameplayAttributeData MaxGuardGauge;

	UPROPERTY(BlueprintReadOnly, Category = "Guard")
	FGameplayAttributeData MetaGuardDamage;
	// 추후에 가드에 직접 데미지를 가하는 패턴이 추가될 수 있어 만들어 놓았다.

	float AbsorbDamage(float IncomingDamage);

	/** 현재 가드 게이지 비율 (0.0 ~ 1.0) */
	float GetGuardGaugePercent() const;

	/** 가드 게이지가 남아있는지 여부 */
	bool HasGuardGauge() const;

private:
	void ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const;

	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;

	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
};
