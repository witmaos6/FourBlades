// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Attributes/FBGuardSet.h"
#include "GameplayEffectExtension.h"

UFBGuardSet::UFBGuardSet() : Super(), MaxGuardGauge(100.0f), MetaGuardDamage(0.0f)
{
	GuardGauge = MaxGuardGauge;
}

float UFBGuardSet::AbsorbDamage(float IncomingDamage)
{
	const float CurrentGauge = GetGuardGauge();

	if (CurrentGauge <= 0.0f)
	{
		return IncomingDamage;
	}

	const float AbsorbedDamage = FMath::Min(CurrentGauge, IncomingDamage);
	const float RemainingDamage = IncomingDamage - AbsorbedDamage;

	SetGuardGauge(CurrentGauge - AbsorbedDamage);

	return RemainingDamage;
}

float UFBGuardSet::GetGuardGaugePercent() const
{
	const float MaxGauge = GetMaxGuardGauge();
	if (MaxGauge <= 0.0f)
	{
		return 0.0f;
	}
	return GetGuardGauge() / MaxGauge;
}

bool UFBGuardSet::HasGuardGauge() const
{
	return GetGuardGauge() > 0.0f;
}

void UFBGuardSet::ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const
{
	if (Attribute == GetGuardGaugeAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxGuardGauge());
	}
	else if (Attribute == GetMaxGuardGaugeAttribute())
	{
		NewValue = FMath::Max(NewValue, 1.0f);
	}
}

void UFBGuardSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::PreAttributeBaseChange(Attribute, NewValue);
	ClampAttribute(Attribute, NewValue);
}

void UFBGuardSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
	ClampAttribute(Attribute, NewValue);
}

void UFBGuardSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	if (Data.EvaluatedData.Attribute == GetMetaGuardDamageAttribute())
	{
		const float DamageToGauge = GetMetaGuardDamage();
		SetGuardGauge(FMath::Max(0.0f, GetGuardGauge() - DamageToGauge));

		SetMetaGuardDamage(0.0f);
	}
	else if (Data.EvaluatedData.Attribute == GetGuardGaugeAttribute())
	{
		SetGuardGauge(FMath::Clamp(GetGuardGauge(), 0.0f, GetMaxGuardGauge()));
	}
	else if (Data.EvaluatedData.Attribute == GetMaxGuardGaugeAttribute())
	{
		SetMaxGuardGauge(FMath::Max(1.0f, GetMaxGuardGauge()));
	}
}
