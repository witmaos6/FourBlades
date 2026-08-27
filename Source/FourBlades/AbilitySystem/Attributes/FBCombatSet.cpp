// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Attributes/FBCombatSet.h"
#include "GameplayEffectExtension.h"

UFBCombatSet::UFBCombatSet() : Super(), AttackPower(0.0f), ResistLaunch(0.0f)
{

}

void UFBCombatSet::AttackPowerClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const
{
	if (Attribute == GetAttackPowerAttribute())
	{
		NewValue = (NewValue < 0.0f) ? 0.0f : NewValue;
	}
}

void UFBCombatSet::ResistLaunchClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const
{
	if (Attribute == GetResistLaunchAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, 1.0f);
	}
}

void UFBCombatSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::PreAttributeBaseChange(Attribute, NewValue);

	AttackPowerClampAttribute(Attribute, NewValue);
	ResistLaunchClampAttribute(Attribute, NewValue);
}

void UFBCombatSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	AttackPowerClampAttribute(Attribute, NewValue);
	ResistLaunchClampAttribute(Attribute, NewValue);
}

void UFBCombatSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	if (Data.EvaluatedData.Attribute == GetAttackPowerAttribute())
	{
		SetAttackPower(FMath::Max(0.0f, GetAttackPower()));
	}
	else if (Data.EvaluatedData.Attribute == GetResistLaunchAttribute())
	{
		SetResistLaunch(FMath::Clamp(GetResistLaunch(), 0.0f, 1.0f));
		/*float NewValue = Data.EvaluatedData.Magnitude;
		if (Data.EvaluatedData.ModifierOp == EGameplayModOp::Additive)
		{
			SetResistLaunch(FMath::Clamp(GetResistLaunch() + NewValue, 0.0f, 1.0f));
		}
		else if (Data.EvaluatedData.ModifierOp == EGameplayModOp::Division)
		{
			SetResistLaunch(FMath::Clamp(GetResistLaunch() / NewValue, 0.0f, 1.0f));
		}*/
	}
}
