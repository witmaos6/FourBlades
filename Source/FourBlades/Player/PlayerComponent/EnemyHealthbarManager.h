// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ControllerComponent.h"
#include "EnemyHealthbarManager.generated.h"

/**
 * 
 */
UCLASS()
class FOURBLADES_API UEnemyHealthbarManager : public UControllerComponent
{
	GENERATED_BODY()
public:
	UEnemyHealthbarManager(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void BeginPlay() override;

protected:
	FTimerHandle EnemyDetectTimer;

	void EnemyDetect();

	UPROPERTY()
	TArray<class AFBEnemyCharacterBase*> DetectedEnemies;

	UPROPERTY()
	TMap<class AFBEnemyCharacterBase*, class UUserWidget*> EnemiesWidgets;

public:
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	void UpdateWidget();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Widget")
	float VisibleDistance;

	UPROPERTY(EditDefaultsOnly, Category = "Widget")
	TSubclassOf<class UUserWidget> HealthbarWidgetClass;

protected:

public:
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason);
};
