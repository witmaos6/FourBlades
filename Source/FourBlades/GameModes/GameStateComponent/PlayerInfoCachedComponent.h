// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/GameStateComponent.h"
#include "PlayerInfoCachedComponent.generated.h"

/**
 * 
 */
UCLASS()
class FOURBLADES_API UPlayerInfoCachedComponent : public UGameStateComponent
{
	GENERATED_BODY()
public:
	UPlayerInfoCachedComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void BeginPlay() override;

	void AddUniquePlayerCharacters(class AFBCharacter* Character);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PlayerPawn")
	TArray<TObjectPtr<class AFBCharacter>> PlayerCharacters;
};
