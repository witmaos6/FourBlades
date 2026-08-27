// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Attributes/FBHealthSet.h"
#include "GameplayEffectExtension.h"
#include "AbilitySystem/FBGameplayEffectContext.h"
#include "Enemy/FBEnemyCharacterBase.h"
#include "FourBlades.h"

UFBHealthSet::UFBHealthSet() : Super(), MaxHealth(10.f), Defense(0.0f), Healing(0.0f), MetaDamage(0.0f)
{
	bOutOfHealth = false;
	Health = MaxHealth;
}

void UFBHealthSet::HealthClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const
{
	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxHealth());
	}
	else if(Attribute == GetMaxHealthAttribute())
	{
		NewValue = FMath::Max(NewValue, 1.0f);
	}
}

void UFBHealthSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::PreAttributeBaseChange(Attribute, NewValue);
	HealthClampAttribute(Attribute, NewValue);
}

void UFBHealthSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
	HealthClampAttribute(Attribute, NewValue);
}

bool UFBHealthSet::PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data)
{
	if (!Super::PreGameplayEffectExecute(Data))
	{
		return false;
	}
	if (Data.EvaluatedData.Attribute == GetMetaDamageAttribute())
	{
		if (OnCalculateDamageReduction.IsBound()) // 이 델리게이트 제거 후 UFBDamageExecution로 이관할 수 있음
		{
			float IncomingDamage = Data.EvaluatedData.Magnitude;
			float DamageReduction = OnCalculateDamageReduction.Execute(IncomingDamage);
			DamageReduction = FMath::Clamp(DamageReduction, 0.0f, IncomingDamage);

			Data.EvaluatedData.Magnitude = IncomingDamage - DamageReduction;
		}
	}
	return true;
}

void UFBHealthSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	const float MinimumHealth = 0.0f;

	AActor* Instigator = Data.EffectSpec.GetContext().GetInstigator();
	AActor* EffectCauser = Data.EffectSpec.GetContext().GetEffectCauser();

	if (Data.EvaluatedData.Attribute == GetHealingAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth() + GetHealing(), MinimumHealth, GetMaxHealth()));
		SetHealing(0.0f);
	}
	else if (Data.EvaluatedData.Attribute == GetMetaDamageAttribute())
	{
		FFBGameplayEffectContext* EffectContext = FFBGameplayEffectContext::ExtractEffectContext(Data.EffectSpec.GetEffectContext());
		check(EffectContext);
		EffectContext->SetFinalDamage(GetMetaDamage());
		
		SetHealth(FMath::Clamp(GetHealth() - GetMetaDamage(), MinimumHealth, GetMaxHealth()));
		if (OnApplyDamageToHealth.IsBound())
		{
			if (Instigator && EffectCauser)
			{
				OnApplyDamageToHealth.Broadcast(Instigator, EffectCauser, EffectContext->GetTargetPayload());
			}
		}
		SetMetaDamage(0.0f);
	}
	else if (Data.EvaluatedData.Attribute == GetMaxHealthAttribute())
	{
		SetMaxHealth(FMath::Max(1.0f, GetMaxHealth()));
	}
	else if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth(), 1.0f, GetMaxHealth()));
	}
	else if (Data.EvaluatedData.Attribute == GetDefenseAttribute())
	{
		SetDefense(FMath::Max(0.0f, GetDefense()));
	}

	if ((GetHealth() <= 0.0f) && !bOutOfHealth)
	{
		if (OnApplyDamageToHealth.IsBound())
		{
			OnApplyDamageToHealth.Clear();
		}
		if (OnOutOfHealth.IsBound())
		{
			OnOutOfHealth.Broadcast();
			OnOutOfHealth.Clear();
		}
		if (OnOutOfHealthWithInstigator.IsBound())
		{
			const FGameplayTag KilledPawnTag = GetPawnTag();
			if (KilledPawnTag.IsValid())
			{
				OnOutOfHealthWithInstigator.Broadcast(KilledPawnTag, Instigator, EffectCauser);
			}
			OnOutOfHealthWithInstigator.Clear();
		}
		if (OnCalculateDamageReduction.IsBound())
		{
			OnCalculateDamageReduction.Unbind();
		}
	}
}

const FGameplayTag UFBHealthSet::GetPawnTag()
{
	if (UAbilitySystemComponent* ASC = GetOwningAbilitySystemComponent())
	{
		if (AActor* Actor = ASC->GetAvatarActor())
		{
			if (AFBEnemyCharacterBase* EnemyBase = Cast<AFBEnemyCharacterBase>(Actor))
			{
				return EnemyBase->GetEnemyKey();
			}
		}
	}
	return FGameplayTag::EmptyTag;
}
