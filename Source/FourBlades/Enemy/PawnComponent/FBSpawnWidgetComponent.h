// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/PawnComponent.h"
#include "FBSpawnWidgetComponent.generated.h"

/**
 *
 */
UCLASS()
class FOURBLADES_API UFBSpawnWidgetComponent : public UPawnComponent
{
	GENERATED_BODY()
public:
	UFBSpawnWidgetComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void OnRegister() override;

public:
	void OnInitAbilityActorInfo(class UAbilitySystemComponent* ASC);

	void SpawnDamageWidget(AActor* Instigator, AActor* EffectCauser, const struct FFBTargetPayload& TargetPayload);

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	TSubclassOf<UUserWidget> DamageWidgetClass;
};
