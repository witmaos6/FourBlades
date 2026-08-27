
#include "FBGameplayEffectContext.h"
#include "GameplayTags/FBDefineGameplayTags.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FBGameplayEffectContext)

/*
* FBGameplayEffectContext
*/
FFBGameplayEffectContext::FFBGameplayEffectContext() : FGameplayEffectContext()
{
}

FFBGameplayEffectContext::FFBGameplayEffectContext(AActor* InInstigator, AActor* InEffectCauser) : FGameplayEffectContext(InInstigator, InEffectCauser)
{
}

FFBGameplayEffectContext* FFBGameplayEffectContext::ExtractEffectContext(struct FGameplayEffectContextHandle Handle)
{
	FGameplayEffectContext* BaseEffectContext = Handle.Get();
	if ((BaseEffectContext != nullptr) && BaseEffectContext->GetScriptStruct()->IsChildOf(FFBGameplayEffectContext::StaticStruct()))
	{
		return (FFBGameplayEffectContext*)BaseEffectContext;
	}

	return nullptr;
}

bool FFBGameplayEffectContext::NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess)
{
	bool bResult = Super::NetSerialize(Ar, Map, bOutSuccess);
	bResult |= TargetPayload.AttackType.NetSerialize(Ar, Map, bOutSuccess);

	Ar << DamageExecutionParms.DamageCoefficient;
	Ar << DamageExecutionParms.AdditionalFixedDamage;

	TargetPayload.AttackType.NetSerialize(Ar, Map, bOutSuccess);
	Ar << TargetPayload.ReactDurationTime;
	Ar << TargetPayload.LaunchStrength;
	Ar << TargetPayload.DirectionType;

	float FinalDamage = TargetPayload.GetFinalDamage();
	Ar << FinalDamage;
	if (Ar.IsLoading())
	{
		TargetPayload.SetFinalDamage(FinalDamage);
	}

	return bResult;
}

/*
* FFBDamageExecutionParams
*/
FFBDamageExecutionParams::FFBDamageExecutionParams()
{
	DamageCoefficient = 0.0f;
	AdditionalFixedDamage = 0.0f;
}

FFBDamageExecutionParams::FFBDamageExecutionParams(float InDamageCoefficient, float InAdditionalFixedDamage)
	: DamageCoefficient(InDamageCoefficient),
	AdditionalFixedDamage(InAdditionalFixedDamage)
{
}

/*
* FBTargetPayload
*/
FFBTargetPayload::FFBTargetPayload()
{
	AttackType = FBTAG_ATTACKTYPE_ATTACK_NORMAL;
	FinalDamage = 0.0f;
	ReactDurationTime = 0.0f;
	LaunchStrength = 0.0f;
}

FFBTargetPayload::FFBTargetPayload(FGameplayTag InAttackType, float InReactDurationTime, float InLaunchStrength, EAttackedLaunchDirectionType InDirectionType)
	: AttackType(InAttackType),
	ReactDurationTime(InReactDurationTime),
	LaunchStrength(InLaunchStrength),
	DirectionType(InDirectionType)
{
}
