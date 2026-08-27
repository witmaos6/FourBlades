// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LifetimeMonitorComponent.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnLifetimeConditionMet);

UENUM(BlueprintType)
enum class ELifetimeConditionType : uint8
{
	TimeBase,
	TravelDistance,
	Both
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FOURBLADES_API ULifetimeMonitorComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	ULifetimeMonitorComponent();

	FOnLifetimeConditionMet OnLifetimeConditionMet;

	UPROPERTY(EditDefaultsOnly, Category = "Condition")
	ELifetimeConditionType ConditionType;

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public: // TimeBase
	UPROPERTY(EditDefaultsOnly, Category = "TimeBase")
	float Lifetime;

protected:
	FTimerHandle LifeTimer;

public:
	void ResetLifeTimer();

protected:
	void OnLifeTimeExpired();

protected: // TravelDistance
	UPROPERTY(EditDefaultsOnly, Category = "TravelDistance")
	float ConditionDistance;

	FVector CenterLocation;

public:
	void SetCenterLocation(FVector NewLocation) { CenterLocation = NewLocation; }

protected:
	void CheckTravelDistance();

protected:
	void NotifyConditionMet();

public:
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
};
