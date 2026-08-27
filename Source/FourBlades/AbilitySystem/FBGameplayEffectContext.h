#pragma once

#include "GameplayEffectTypes.h"
#include "GameplayTagContainer.h"
#include "GameplayTags/FBDefineGameplayTags.h"
#include "FBGameplayEffectContext.generated.h"

/*
* FBDamageExecutionParams
*/
USTRUCT(BlueprintType)
struct FFBDamageExecutionParams
{
	GENERATED_BODY()

	FFBDamageExecutionParams();

	FFBDamageExecutionParams(float InDamageCoefficient, float InAdditionalFixedDamage);

	UPROPERTY()
	float DamageCoefficient;

	UPROPERTY()
	float AdditionalFixedDamage;
};

UENUM()
enum class EAttackedLaunchDirectionType
{
	TowardTarget, // 공격자 -> 피격자 방향
	AttackerForward, // 공격자의 ForwardVector 방향 Pitch값은 공격자 -> 피격자 방향
	SelfBack, // 자신의 뒤 방향
	/*  추가 예정
	TargetUp // 피격자의 UpVector
	CustomAngle // 특정 Pitch값
	*/
};

/*
* FBTargetPayload
*/
USTRUCT(BlueprintType)
struct FFBTargetPayload
{
	GENERATED_BODY()

	FFBTargetPayload();

	FFBTargetPayload(FGameplayTag InAttackType, float InReactDurationTime, float InLaunchStrength, EAttackedLaunchDirectionType InDirectionType = EAttackedLaunchDirectionType::TowardTarget);

	FGameplayTag AttackType;

	float ReactDurationTime;

	float LaunchStrength;

	// To do: LaunchDuration 추가 할 수 있음

	EAttackedLaunchDirectionType DirectionType;

private:
	float FinalDamage;

public:
	void SetFinalDamage(float InFinalDamage) { FinalDamage = InFinalDamage; }
	const float GetFinalDamage()const { return FinalDamage; }
};

/*
* FBGameplayEffectContext
*/
USTRUCT(BlueprintType)
struct FFBGameplayEffectContext : public FGameplayEffectContext
{
	GENERATED_BODY()

	FFBGameplayEffectContext();

	FFBGameplayEffectContext(AActor* InInstigator, AActor* InEffectCauser);

	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> SourceASC;

	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> TargetASC;

	static FFBGameplayEffectContext* ExtractEffectContext(struct FGameplayEffectContextHandle Handle);

	/*
	* GameplayEffectContext를 상속받아 사용하려면 필수로 있어야하는 함수
	*/
	virtual UScriptStruct* GetScriptStruct() const override
	{
		return StaticStruct();
	}

	virtual bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess) override;
	/*
	* GameplayEffectContext를 상속받아 사용하려면 필수로 있어야하는 함수
	*/
	virtual FFBGameplayEffectContext* Duplicate() const
	{
		FFBGameplayEffectContext* NewContext = new FFBGameplayEffectContext();
		*NewContext = *this;
		if (GetHitResult())
		{
			// Does a deep copy of the hit result
			NewContext->AddHitResult(*GetHitResult(), true);
		}
		return NewContext;
	}

private:
	FFBDamageExecutionParams DamageExecutionParms;

	FFBTargetPayload TargetPayload;

public:
	void SetDamageExecutionParams(FFBDamageExecutionParams&& InDamageExecutionParams) { DamageExecutionParms = MoveTemp(InDamageExecutionParams); }

	const FFBDamageExecutionParams& GetDamageExecutionParams() const { return DamageExecutionParms; }

	void SetFinalDamage(float InFinalDamage) { TargetPayload.SetFinalDamage(InFinalDamage); }

	void SetTargetPayload(FFBTargetPayload&& InTargetPayload) { TargetPayload = MoveTemp(InTargetPayload); }

	const FFBTargetPayload& GetTargetPayload()const { return TargetPayload; }
};

template<>
struct TStructOpsTypeTraits<FFBGameplayEffectContext> : public TStructOpsTypeTraitsBase2<FFBGameplayEffectContext>
{
	enum
	{
		WithNetSerializer = true, 
		WithCopy = true 
	};
};