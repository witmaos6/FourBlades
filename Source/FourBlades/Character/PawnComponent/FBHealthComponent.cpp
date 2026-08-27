// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/PawnComponent/FBHealthComponent.h"
#include "AbilitySystem//Attributes/FBHealthSet.h"
#include "AbilitySystem/FBAbilitySystemComponent.h"
#include "GameplayEffect.h"
#include "GameplayEffectExtension.h"

static AActor* GetInstigatorFromAttrChangeData(const FOnAttributeChangeData& ChangeData);

UFBHealthComponent::UFBHealthComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = false;

	AbilitySystemComponent = nullptr;
	HealthSet = nullptr;
}

void UFBHealthComponent::InitializeWithAbilitySystem(UFBAbilitySystemComponent* InASC)
{
	AActor* Owner = GetOwner();
	check(Owner);

	if (AbilitySystemComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("HealthComponent: Health component for owner [%s] has already been initialized with an ability system."), *GetNameSafe(Owner));
		return;
	}

	AbilitySystemComponent = InASC;
	if (!AbilitySystemComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("HealthComponent: Cannot initialize health component for owner [%s] with NULL ability system."), *GetNameSafe(Owner));
		return;
	}

	HealthSet = AbilitySystemComponent->GetSet<UFBHealthSet>();
	if (!HealthSet)
	{
		UE_LOG(LogTemp, Error, TEXT("HealthComponent: Cannot initialize health component for owner [%s] with NULL health set on the ability system."), *GetNameSafe(Owner));
		return;
	}

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UFBHealthSet::GetHealthAttribute()).AddUObject(this, &UFBHealthComponent::HandleHealthChanged);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UFBHealthSet::GetMaxHealthAttribute()).AddUObject(this, &UFBHealthComponent::HandleMaxHealthChanged);

	OnHealthChanged.Broadcast(this, 0, HealthSet->GetHealth(), nullptr);
}

void UFBHealthComponent::UninitializeWithAbilitySystem()
{
	AbilitySystemComponent = nullptr;
	HealthSet = nullptr;
}

void UFBHealthComponent::HandleHealthChanged(const FOnAttributeChangeData& ChangeData)
{
	OnHealthChanged.Broadcast(this, ChangeData.OldValue, ChangeData.NewValue, GetInstigatorFromAttrChangeData(ChangeData));
}

void UFBHealthComponent::HandleMaxHealthChanged(const FOnAttributeChangeData& ChangeData)
{
	OnMaxHealthChanged.Broadcast(this, ChangeData.OldValue, ChangeData.NewValue);
}

UFBHealthComponent* UFBHealthComponent::FindHealthComponent(const AActor* Actor)
{
	if (!Actor)
	{
		return nullptr;
	}

	UFBHealthComponent* HealthComponent = Actor->FindComponentByClass<UFBHealthComponent>();
	return HealthComponent;
}

float UFBHealthComponent::GetHealth() const
{
	return (HealthSet ? HealthSet->GetHealth() : 0.0f);
}

float UFBHealthComponent::GetMaxHealth() const
{
	return (HealthSet ? HealthSet->GetMaxHealth() : 0.0f);
}

float UFBHealthComponent::GetHealthNormalized() const
{
	if (HealthSet)
	{
		const float Health = HealthSet->GetHealth();
		const float MaxHealth = HealthSet->GetMaxHealth();
		return ((MaxHealth > 0.0f) ? (Health / MaxHealth) : 0.0f);
	}
	return 0.0f;
}

// ¸â¹ö º¯¼ö ¾Æ´Ô
static AActor* GetInstigatorFromAttrChangeData(const FOnAttributeChangeData& ChangeData)
{
	if (ChangeData.GEModData != nullptr)
	{
		const FGameplayEffectContextHandle& EffectContext = ChangeData.GEModData->EffectSpec.GetEffectContext();
		return EffectContext.GetOriginalInstigator();
	}
	return nullptr;
}
