// Fill out your copyright notice in the Description page of Project Settings.


#include "GameModes/FBExperienceManagerComponent.h"
#include "System/FBAssetManager.h"
#include "FBExperienceDefinition.h"
#include "FBExperienceActionSet.h"
#include "GameFeaturesSubsystemSettings.h"
#include "GameFeaturesSubsystem.h"
#include "GameFeatureAction.h"
#include "FBUserFacingExperience.h"
#include "FourBlades.h"

void UFBExperienceManagerComponent::CallOrRegister_OnExperienceLoaded(FOnFBExperienceLoaded::FDelegate&& Delegate)
{
	if (IsExperienceLoaded())
	{
		Delegate.Execute(CurrentExperience);
	}
	else
	{
		OnExperienceLoaded.Add(MoveTemp(Delegate));
	}
}

void UFBExperienceManagerComponent::ServerSetCurrentExperience(FPrimaryAssetId ExperienceId)
{
	UFBAssetManager& AssetManager = UFBAssetManager::Get();

	TSubclassOf<UFBExperienceDefinition> AssetClass;
	{
		FSoftObjectPath AssetPath = AssetManager.GetPrimaryAssetPath(ExperienceId);
		AssetClass = Cast<UClass>(AssetPath.TryLoad());
	}
	if (AssetClass == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("AssetClass is null"));
		return;
	}

	const UFBExperienceDefinition* Experience = GetDefault<UFBExperienceDefinition>(AssetClass);
	if (Experience)
	{
		CurrentExperience = Experience;
		StartExperienceLoad();
	}
	/*check(Experience != nullptr); // 원본 코드
	check(CurrentExperience == nullptr);
	{
		CurrentExperience = Experience;
	}

	StartExperienceLoad();*/
}

void UFBExperienceManagerComponent::StartExperienceLoad()
{
	check(CurrentExperience);
	check(LoadState == EFBExperienceLoadState::Unloaded);

	LoadState = EFBExperienceLoadState::Loading;

	UFBAssetManager& AssetManager = UFBAssetManager::Get();

	TSet<FPrimaryAssetId> BundleAssetList;
	BundleAssetList.Add(CurrentExperience->GetPrimaryAssetId());

	for (const TObjectPtr<UFBExperienceActionSet>& ActionSet : CurrentExperience->ActionSets)
	{
		if (ActionSet)
		{
			BundleAssetList.Add(ActionSet->GetPrimaryAssetId());
		}
	}

	TArray<FName> BundlesToLoad;
	{
		const ENetMode OwnerNetMode = GetOwner()->GetNetMode();
		bool bLoadClient = GIsEditor || (OwnerNetMode != NM_DedicatedServer);
		bool bLoadServer = GIsEditor || (OwnerNetMode != NM_Client);
		if (bLoadClient)
		{
			BundlesToLoad.Add(UGameFeaturesSubsystemSettings::LoadStateClient);
		}
		if (bLoadServer)
		{
			BundlesToLoad.Add(UGameFeaturesSubsystemSettings::LoadStateServer);
		}
	}

	FStreamableDelegate OnAssetLoadedDelegate = FStreamableDelegate::CreateUObject(this, &ThisClass::OnExperienceLoadComplete);

	TSharedPtr<FStreamableHandle> Handle = AssetManager.ChangeBundleStateForPrimaryAssets(BundleAssetList.Array(), BundlesToLoad, {}, false, FStreamableDelegate(), FStreamableManager::AsyncLoadHighPriority);

	if (!Handle.IsValid() || Handle->HasLoadCompleted())
	{
		FStreamableHandle::ExecuteDelegate(OnAssetLoadedDelegate);
	}
	else
	{
		Handle->BindCompleteDelegate(OnAssetLoadedDelegate);
		Handle->BindCancelDelegate(FStreamableDelegate::CreateLambda([OnAssetLoadedDelegate]
			{
				OnAssetLoadedDelegate.ExecuteIfBound();
			}));
	}

	static int32 StartExperienceLoad_FrameNumber = GFrameNumber;
}

void UFBExperienceManagerComponent::OnExperienceLoadComplete() // 추후에 로딩할 데이터가 많아질 경우를 대비한 함수다.
{
	static int32 OnExperienceLoadComplete_FrameNumber = GFrameNumber;

	check(LoadState == EFBExperienceLoadState::Loading);
	check(CurrentExperience);

	GameFeaturePluginURLs.Reset();

	auto CollectGameFeaturePluginURLs = [This = this](const UPrimaryDataAsset* Context, const TArray<FString>& FeaturePluginList)
		{
			for (const FString& PluginName : FeaturePluginList)
			{
				FString PluginURL;
				if (UGameFeaturesSubsystem::Get().GetPluginURLByName(PluginName, PluginURL))
				{
					This->GameFeaturePluginURLs.AddUnique(PluginURL);
				}
			}
		};

	CollectGameFeaturePluginURLs(CurrentExperience, CurrentExperience->GameFeaturesToEnable);

	NumGameFeaturePluginsLoading = GameFeaturePluginURLs.Num();
	if (NumGameFeaturePluginsLoading)
	{
		LoadState = EFBExperienceLoadState::LoadingGameFeatures;
		for (const FString& PluginURL : GameFeaturePluginURLs)
		{
			UGameFeaturesSubsystem::Get().LoadAndActivateGameFeaturePlugin(PluginURL, FGameFeaturePluginLoadComplete::CreateUObject(this, &ThisClass::OnGameFeaturePluginLoadComplete));
		}
	}
	else
	{
		OnExperienceFullLoadComplete();
	}
}

void UFBExperienceManagerComponent::OnGameFeaturePluginLoadComplete(const UE::GameFeatures::FResult& Result)
{
	NumGameFeaturePluginsLoading--;
	if (NumGameFeaturePluginsLoading == 0)
	{
		OnExperienceFullLoadComplete();
	}
}

void UFBExperienceManagerComponent::OnExperienceFullLoadComplete()
{
	check(LoadState != EFBExperienceLoadState::Loaded);

	{ // GameFeature Plugin의 로딩과 활성화 이후, GameFeature Action들을 활성화
		LoadState = EFBExperienceLoadState::ExecutingActions;

		FGameFeatureActivatingContext Context; // PIE를 구분하기 위한 Context이다.
		{
			const FWorldContext* ExistingWorldContext = GEngine->GetWorldContextFromWorld(GetWorld());
			if (ExistingWorldContext)
			{
				Context.SetRequiredWorldContextHandle(ExistingWorldContext->ContextHandle);
			}
		}

		auto ActivateListOfActions = [&Context](const TArray<UGameFeatureAction*>& ActionList)
			{
				for (UGameFeatureAction* Action : ActionList)
				{
					if (Action)
					{
						Action->OnGameFeatureRegistering();
						Action->OnGameFeatureLoading();
						Action->OnGameFeatureActivating(Context);
					}
				}
			};

		ActivateListOfActions(CurrentExperience->Actions);

		for (const TObjectPtr<UFBExperienceActionSet>& ActionSet : CurrentExperience->ActionSets)
		{
			ActivateListOfActions(ActionSet->Actions);
		}
	}

	LoadState = EFBExperienceLoadState::Loaded;
	OnExperienceLoaded.Broadcast(CurrentExperience);
	OnExperienceLoaded.Clear();
}

const UFBExperienceDefinition* UFBExperienceManagerComponent::GetCurrentExperienceChecked() const
{
	check(LoadState == EFBExperienceLoadState::Loaded);
	check(CurrentExperience != nullptr);

	return CurrentExperience;
}

void UFBExperienceManagerComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (EndPlayReason == EEndPlayReason::EndPlayInEditor)
	{
		UE_LOG(LogTemp, Warning, TEXT("PIE ending - skipping GameFeatureAction deactivation"));
		OnAllActionsDeactivated();
		return;
	}

	for (const FString& PluginURL : GameFeaturePluginURLs)
	{
		UGameFeaturesSubsystem::Get().DeactivateGameFeaturePlugin(PluginURL);
		/*if (ULyraExperienceManager::RequestToDeactivatePlugin(PluginURL)) // 원본함수
		{
			UGameFeaturesSubsystem::Get().DeactivateGameFeaturePlugin(PluginURL);
		}*/
	}

	if (LoadState == EFBExperienceLoadState::Loaded)
	{
		LoadState = EFBExperienceLoadState::Deactivating;

		NumExpectedPausers = INDEX_NONE;
		NumObservedPausers = 0;

		// GameFeatureDeactivatingContext 생성
		FGameFeatureDeactivatingContext Context(TEXT(""), [this](FStringView) { this->OnActionDeactivationCompleted(); });

		// World Context 설정
		const FWorldContext* ExistingWorldContext = GEngine->GetWorldContextFromWorld(GetWorld());
		if (ExistingWorldContext)
		{
			Context.SetRequiredWorldContextHandle(ExistingWorldContext->ContextHandle);
		}

		// GameFeatureAction들 비활성화 람다
		auto DeactivateListOfActions = [&Context](const TArray<UGameFeatureAction*>& ActionList)
			{
				for (UGameFeatureAction* Action : ActionList)
				{
					if (Action)
					{
						Action->OnGameFeatureDeactivating(Context);
						Action->OnGameFeatureUnregistering();
					}
				}
			};

		// 현재 Experience의 Actions 비활성화
		DeactivateListOfActions(CurrentExperience->Actions);

		// ActionSet들의 Actions도 비활성화 *역순으로 해야할 수도 있다.
		for (const TObjectPtr<UFBExperienceActionSet>& ActionSet : CurrentExperience->ActionSets)
		{
			if (IsValid(ActionSet))
			{
				DeactivateListOfActions(ActionSet->Actions);
			}
		}

		NumExpectedPausers = Context.GetNumPausers();

		if (NumExpectedPausers > 0)
		{
			UE_LOG(LogTemp, Error, TEXT("Actions that have asynchronous deactivation aren't fully supported yet in Lyra experiences"));
		}

		if (NumExpectedPausers == NumObservedPausers)
		{
			OnAllActionsDeactivated();
		}
	}
}

void UFBExperienceManagerComponent::OnActionDeactivationCompleted()
{
	check(IsInGameThread());
	
	++NumObservedPausers;

	if (NumObservedPausers == NumExpectedPausers)
	{
		OnAllActionsDeactivated();
	}
}

void UFBExperienceManagerComponent::OnAllActionsDeactivated()
{
	LoadState = EFBExperienceLoadState::Unloaded;
	CurrentExperience = nullptr;
	// GEngine->ForceGarbageCollection(true); // Lyra 주석처리 된 코드

	//GameFeaturePluginURLs.Reset(); // Claude 추천 코드
}
