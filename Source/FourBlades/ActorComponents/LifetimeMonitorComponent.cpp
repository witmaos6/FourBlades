// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorComponents/LifetimeMonitorComponent.h"
#include "FourBlades.h"

ULifetimeMonitorComponent::ULifetimeMonitorComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 1.0f;

	ConditionType = ELifetimeConditionType::TimeBase;

	Lifetime = 0.0f;

	ConditionDistance = 1000.f;
	CenterLocation = FVector::ZeroVector;
}

void ULifetimeMonitorComponent::BeginPlay()
{
	Super::BeginPlay();

	if (ConditionType == ELifetimeConditionType::TimeBase || ConditionType == ELifetimeConditionType::Both)
	{
		if (Lifetime > 0.0f)
		{
			GetWorld()->GetTimerManager().SetTimer(LifeTimer, this, &ULifetimeMonitorComponent::OnLifeTimeExpired, Lifetime, false);
		}
	}
	if (ConditionType == ELifetimeConditionType::TravelDistance || ConditionType == ELifetimeConditionType::Both)
	{
		if (GetOwner())
		{
			CenterLocation = GetOwner()->GetActorLocation();
		}
		else
		{
			FB_FUNC_LOG(LogTemp, Warning, TEXT("Owner is null"));
		}
	}
}


// Called every frame
void ULifetimeMonitorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (ConditionType == ELifetimeConditionType::TravelDistance || ConditionType == ELifetimeConditionType::Both)
	{
		CheckTravelDistance();
	}
}

void ULifetimeMonitorComponent::ResetLifeTimer()
{
	FTimerManager& TimerManager = GetWorld()->GetTimerManager();
	if (TimerManager.IsTimerActive(LifeTimer))
	{
		TimerManager.ClearTimer(LifeTimer);

		TimerManager.SetTimer(LifeTimer, this, &ULifetimeMonitorComponent::OnLifeTimeExpired, Lifetime, false);
	}
}

void ULifetimeMonitorComponent::OnLifeTimeExpired()
{
	NotifyConditionMet();
}

void ULifetimeMonitorComponent::CheckTravelDistance()
{
	if (ConditionType == ELifetimeConditionType::TravelDistance || ConditionType == ELifetimeConditionType::Both)
	{
		if (GetOwner() && IsValid(GetOwner()))
		{
			FVector CurrentLocation = GetOwner()->GetActorLocation();

			float CurrentDistance = FVector::Dist(CenterLocation, CurrentLocation);

			if (CurrentDistance >= ConditionDistance)
			{
				NotifyConditionMet();
			}
		}
	}
}

void ULifetimeMonitorComponent::NotifyConditionMet()
{
	if (OnLifetimeConditionMet.IsBound())
	{
		OnLifetimeConditionMet.Broadcast();
		OnLifetimeConditionMet.Clear();
	}
}

void ULifetimeMonitorComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (OnLifetimeConditionMet.IsBound())
	{
		OnLifetimeConditionMet.Clear();
	}

	Super::EndPlay(EndPlayReason);
}
