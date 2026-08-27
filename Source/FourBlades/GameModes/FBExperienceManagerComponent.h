// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/GameStateComponent.h"
#include "GameFeaturePluginOperationResult.h"
#include "FBExperienceManagerComponent.generated.h"

enum class EFBExperienceLoadState
{
	Unloaded,
	Loading,
	LoadingGameFeatures,
	ExecutingActions,
	Loaded,
	Deactivating
};

class UFBExperienceDefinition;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnFBExperienceLoaded, const UFBExperienceDefinition*);
/**
 * 
 */
UCLASS()
class FOURBLADES_API UFBExperienceManagerComponent : public UGameStateComponent
{
	GENERATED_BODY()
	
public: // Experience Loading
	bool IsExperienceLoaded() { return (LoadState == EFBExperienceLoadState::Loaded) && (CurrentExperience != nullptr); }

	void CallOrRegister_OnExperienceLoaded(FOnFBExperienceLoaded::FDelegate&& Delegate);

	void ServerSetCurrentExperience(FPrimaryAssetId ExperienceId);

	void StartExperienceLoad();

	void OnExperienceLoadComplete(); // GameFeafures를 로딩 후 활성화

	void OnGameFeaturePluginLoadComplete(const UE::GameFeatures::FResult& Result);

	void OnExperienceFullLoadComplete();

	const class UFBExperienceDefinition* GetCurrentExperienceChecked() const;
	
public: // Experience Loading
	UPROPERTY()
	TObjectPtr<const class UFBExperienceDefinition> CurrentExperience;

	EFBExperienceLoadState LoadState = EFBExperienceLoadState::Unloaded;

	FOnFBExperienceLoaded OnExperienceLoaded;

public: // GameFeature Loading
	int32 NumGameFeaturePluginsLoading = 0;

	TArray<FString> GameFeaturePluginURLs;

public:
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	int32 NumObservedPausers = 0;

	int32 NumExpectedPausers = 0;

	void OnActionDeactivationCompleted();

	void OnAllActionsDeactivated();
};
