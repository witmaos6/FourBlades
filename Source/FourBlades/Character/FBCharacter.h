// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/FBCharacterBase.h"
#include "AbilitySystemInterface.h"
#include "FBCharacter.generated.h"

/**
 * 
 */
UCLASS()
class FOURBLADES_API AFBCharacter : public AFBCharacterBase, public IAbilitySystemInterface 
{
	GENERATED_BODY()
	
public:
	AFBCharacter();

public:
	virtual void BeginPlay() override;

	//virtual void PostInitializeComponents() override; // 쓸일 있으면 사용

public:
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void PossessedBy(AController* NewController) override;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "FB|Character")
	TObjectPtr<class UFBPawnExtensionComponent> PawnExtComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "FB|Character")
	TObjectPtr<class UFBCameraComponent> CameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "FB|Character")
	TObjectPtr<class UFBHealthComponent> HealthComponent;

public:
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	void OnAbilitySystemInitialized();

	void OnAbilitySystemUninitialized();

protected:
	void SetDead();

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	TObjectPtr<UAnimMontage> DeadMontage;

	void Resurrection();

	// virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
};
