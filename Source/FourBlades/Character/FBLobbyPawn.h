// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/FBCharacterBase.h"
#include "AbilitySystemInterface.h"
#include "FBLobbyPawn.generated.h"

/**
 * 
 */
UCLASS()
class FOURBLADES_API AFBLobbyPawn : public AFBCharacterBase, public IAbilitySystemInterface
{
	GENERATED_BODY()
public:
	AFBLobbyPawn();

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "FB|Character")
	TObjectPtr<class UFBPawnExtensionComponent> PawnExtComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "FB|Character")
	TObjectPtr<class UFBCameraComponent> CameraComponent;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
};
