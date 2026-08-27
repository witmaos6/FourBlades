// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/PawnComponent.h"
#include "Components/GameFrameworkInitStateInterface.h"
#include "Input/FBMappableConfigPair.h"
#include "InputActionValue.h"
#include "FBCharacterComponent.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, Meta = (BlueprintSpawnableComponent))
class FOURBLADES_API UFBCharacterComponent : public UPawnComponent , public IGameFrameworkInitStateInterface
{
	GENERATED_BODY()
	// 카메라, 입력 등 플레이어가 제어하는 시스템의 초기화를 처리하는 컴포넌트, Lyra에서는 ULyraHeroComponent라는 이름으로 사용되고 있다. 더 직관적인 이름을 생각해 볼 필요가 있다.	
public:
	UFBCharacterComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	static const FName NAME_ActorFeatureName;

	static const FName NAME_BindInputsNow;

public: // Interface
	virtual void OnRegister() final;

	virtual void BeginPlay() final;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) final;

public: // IGameFrameworkInitStateInterface
	virtual FName GetFeatureName() const final { return NAME_ActorFeatureName; }

	virtual void OnActorInitStateChanged(const FActorInitStateChangedParams& Params) final;

	virtual bool CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) const final;

	virtual void HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) final;

	virtual void CheckDefaultInitialization() final;

public: // Settings Input, Camera
	TSubclassOf<class UFBCameraMode> DetermineCameraMode() const;

	void InitializePlayerInput(UInputComponent* PlayerInputComponent);

	UPROPERTY(EditAnywhere, Category = "Input")
	TArray<FFBMappableConfigPair> InputConfigs;

public: // Input Function
	void Input_Move(const FInputActionValue& InputActionValue);

	void Input_Look_Mouse(const FInputActionValue& InputActionValue);

	void Input_AbilityInputTagPressed(FGameplayTag InputTag);

	void Input_AbilityInputTagReleased(FGameplayTag InputTag);

	void Input_AbilityInputConfirm();

public:
	void ActivateInputConfig(FName ConfigName);

	void DeactivateInputConfig(FName ConfigName);

	UFUNCTION(BlueprintCallable, Category = "Input")
	void SwitchInputConfig(FName DeactivateConfig, FName ActivateConfig);

	bool IsInputConfigActive(FName ConfigName);

private:
	void ApplyInputConfigs();

	void ActivateIMC(const FFBMappableConfigPair& ConfigPair);

	void DeactivateIMC(const FFBMappableConfigPair& ConfigPair);

	class UEnhancedInputLocalPlayerSubsystem* GetEnhancedInputSubsystem() const;

	UPROPERTY()
	TMap<FName, FFBMappableConfigPair> ActiveConfigs;

	// 모든 Config 맵 (빠른 검색용)
	UPROPERTY()
	TMap<FName, FFBMappableConfigPair> AllConfigs;
};
