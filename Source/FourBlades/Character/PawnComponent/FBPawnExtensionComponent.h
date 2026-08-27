// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/PawnComponent.h"
#include "Components/GameFrameworkInitStateInterface.h"
#include "FBPawnExtensionComponent.generated.h"

/**
 * 컴포넌트들의 초기화 순서를 조정하는 컴포넌트
 */
UCLASS()
class FOURBLADES_API UFBPawnExtensionComponent : public UPawnComponent , public IGameFrameworkInitStateInterface
{
	GENERATED_BODY()
	
public:
	UFBPawnExtensionComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	static const FName NAME_ActorFeatureName;

public: // Method
	static UFBPawnExtensionComponent* FindPawnExtensionComponent(const AActor* Actor) { return (Actor ? Actor->FindComponentByClass<UFBPawnExtensionComponent>() : nullptr); }

	template<class T>
	const T* GetPawnData() const { return Cast<T>(PawnData); }
	void SetPawnData(const class UFBPawnData* InPawnData);
	void SetupPlayerInputComponent();

public: // Cache Data
	UPROPERTY(EditInstanceOnly, Category = "FB|Pawn")
	TObjectPtr<const class UFBPawnData> PawnData;

	UPROPERTY()
	TObjectPtr<class UFBAbilitySystemComponent> AbilitySystemComponent;

public: // Interface
	virtual void OnRegister() final;

	virtual void BeginPlay() final;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) final;

public: // IGameFrameworkInitStateInterface
	virtual FName GetFeatureName() const final { return NAME_ActorFeatureName; }

	virtual void OnActorInitStateChanged(const FActorInitStateChangedParams& Params) final;

	virtual bool CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) const final;

	virtual void CheckDefaultInitialization() final;

public: // AbilitySystem
	void InitializeAbilitySystem(class UFBAbilitySystemComponent* InASC, AActor* InOwnerActor);

	void OnAbilitySystemInitialized_RegisterAndCall(FSimpleMulticastDelegate::FDelegate Delegate);

	void UninitializeAbilitySystem();

	void OnAbilitySystemUninitialized_Register(FSimpleMulticastDelegate::FDelegate Delegate);

	UFBAbilitySystemComponent* GetFBAbilitySystemComponent() const { return AbilitySystemComponent; }

	FSimpleMulticastDelegate OnAbilitySystemInitialized;

	FSimpleMulticastDelegate OnAbilitySystemUninitialized;
};
