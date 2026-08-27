// Fill out your copyright notice in the Description page of Project Settings.


#include "GASActor/GASActorBase.h"
#include "AbilitySystemComponent.h"

// Sets default values
AGASActorBase::AGASActorBase()
{
	PrimaryActorTick.bCanEverTick = true;

	ASC = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("ASC"));
}

void AGASActorBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	ASC->InitAbilityActorInfo(this, this);

	if (GetInstigator())
	{
		CachedInstigator = GetInstigator();
	}
}

UAbilitySystemComponent* AGASActorBase::GetAbilitySystemComponent() const
{
	return (ASC) ? ASC : nullptr;
}

float AGASActorBase::ApplyCountDamage(float Damage)
{
	return Damage - 1.0f;
}
