// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AbilityTask_Trace.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTraceResultDelegate, const FGameplayAbilityTargetDataHandle&, TargetDataHandle);

/**
 * 
 */
UCLASS()
class FOURBLADES_API UAbilityTask_Trace : public UAbilityTask
{
	GENERATED_BODY()
public:
	UAbilityTask_Trace();

	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (DisplayName = "WaitForTrace", HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static UAbilityTask_Trace* CreateTask(UGameplayAbility* OwningAbility, TSubclassOf<class ATargetActor_Sweep> TargetActorClass);

	virtual void Activate() override;

	void SpawnAndInitializeTargetActor();

	void FinalizeTargetActor();

protected:
	void OnTargetDataReadyCallback(const FGameplayAbilityTargetDataHandle& TargetDataHandle);

public:
	UPROPERTY(BlueprintAssignable)
	FTraceResultDelegate OnComplete;

public:
	virtual void OnDestroy(bool AbilityEnded) override;

protected:
	UPROPERTY()
	TSubclassOf<class ATargetActor_Sweep> TargetActor;

	UPROPERTY()
	TObjectPtr<class ATargetActor_Sweep> SpawnedTargetActor;
};
