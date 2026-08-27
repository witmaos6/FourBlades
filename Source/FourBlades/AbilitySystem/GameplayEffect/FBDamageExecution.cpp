// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/GameplayEffect/FBDamageExecution.h"
#include "AbilitySystem/Attributes/FBCombatSet.h"
#include "AbilitySystem/Attributes/FBHealthSet.h"
#include "AbilitySystem/GameplayEffect/FBDamageEffect.h"
#include "AbilitySystem/FBGameplayEffectContext.h"

/*
* DamageStatics
*/
struct FDamageStatics
{
	FGameplayEffectAttributeCaptureDefinition BaseDamageDef;

	FGameplayEffectAttributeCaptureDefinition DefenseDef;

	FDamageStatics()
	{
		BaseDamageDef = FGameplayEffectAttributeCaptureDefinition(UFBCombatSet::GetAttackPowerAttribute(), EGameplayEffectAttributeCaptureSource::Source, true);

		DefenseDef = FGameplayEffectAttributeCaptureDefinition(UFBHealthSet::GetDefenseAttribute(), EGameplayEffectAttributeCaptureSource::Target, true);
	}
};

static FDamageStatics& DamageStatics()
{
	static FDamageStatics Statics;
	return Statics;
}

/*
* DamageExecution
*/
UFBDamageExecution::UFBDamageExecution() : Super()
{
	RelevantAttributesToCapture.Add(DamageStatics().BaseDamageDef);

	RelevantAttributesToCapture.Add(DamageStatics().DefenseDef);
}

void UFBDamageExecution::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	FGameplayEffectContextHandle EffectContextHandle = Spec.GetContext();
	FGameplayEffectContext* EffectContext = EffectContextHandle.Get();
	FFBGameplayEffectContext* FBEffectContext = static_cast<FFBGameplayEffectContext*>(EffectContext); // static_cast를 시도하기 전 검증절차를 추가해야 할 수도 있다.

	float Damage = 0.0f;
	{
		FAggregatorEvaluateParameters EvaluateParameters;
		float BaseDamage = 0.0f;
		{
			float AttackPower = 0.0f;
			ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().BaseDamageDef, EvaluateParameters, AttackPower);

			if (FBEffectContext)
			{
				const FFBDamageExecutionParams& DamageExecutionParams = FBEffectContext->GetDamageExecutionParams();
				BaseDamage = (AttackPower * DamageExecutionParams.DamageCoefficient) + DamageExecutionParams.AdditionalFixedDamage;
			}
		}
		
		float Defense = 0.0f;
		{
			bool bHasDefense = ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().DefenseDef, EvaluateParameters, Defense);
			if (!bHasDefense)
			{
				Defense = 0.0f;
			}
			Defense = FMath::Max(0.0f, Defense);
		}

		Damage = ApplyDefenseReduction(BaseDamage, Defense);
	}
	if (Damage >= 0.0f)
	{
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UFBHealthSet::GetMetaDamageAttribute(), EGameplayModOp::Override, Damage));
	}
}

float UFBDamageExecution::ApplyDefenseReduction(float BaseDamage, float Defense) const
{
	const float DefenseConstant = 100.f;
	float DefnseReduction = Defense / (Defense + DefenseConstant);

	float Damage = BaseDamage * (1.0f - DefnseReduction);
	return FMath::RoundToFloat(Damage);
}
