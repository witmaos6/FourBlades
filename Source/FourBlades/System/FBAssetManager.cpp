// Fill out your copyright notice in the Description page of Project Settings.

#include "FBAssetManager.h"
#include "Log/FBLogChannels.h"
#include "GameplayTags/FBGameplayTags.h"
#include "FBGameSingleton.h"

UFBAssetManager::UFBAssetManager()
{
}

UFBAssetManager& UFBAssetManager::Get()
{
	check(GEngine);

	if (UFBAssetManager* FBAssetManager = Cast<UFBAssetManager>(GEngine->AssetManager))
	{
		return *FBAssetManager;
	}

	UE_LOG(LogFB, Fatal, TEXT("Invalid AssetManager")); // 로그와 함께 크래시 발생
	return *NewObject<UFBAssetManager>(); // 컴파일을 위한 리턴
}

void UFBAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();

	FFBGameplayTags::InitializeNativeTags();
}

bool UFBAssetManager::ShouldLogAssetLoads()
{
	const TCHAR* CommanLineContent = FCommandLine::Get();
	static bool bLogAssetLoads = FParse::Param(CommanLineContent, TEXT("LogAssetLoads"));
	return bLogAssetLoads;
}

UObject* UFBAssetManager::SynchronousLoadAsset(const FSoftObjectPath& AssetPath)
{
	if (AssetPath.IsValid())
	{
		TUniquePtr<FScopeLogTime> LogTimePtr;
		if (ShouldLogAssetLoads())
		{
			LogTimePtr = MakeUnique<FScopeLogTime>(*FString::Printf(TEXT("synchronous loaded assets [%s]"), *AssetPath.ToString()), nullptr, FScopeLogTime::ScopeLog_Seconds);
		}

		if (UAssetManager::IsInitialized())
		{
			return UAssetManager::GetStreamableManager().LoadSynchronous(AssetPath);
		}

		return AssetPath.TryLoad();
	}
	return nullptr;
}

void UFBAssetManager::AddLoadedAsset(const UObject* Asset)
{
	if (ensureAlways(Asset))
	{
		FScopeLock Lock(&SyncObject); // 에디터가 켜지기 전에 멀티쓰레드로 안전하게 실행하기 위해 Lock을 건다.
		LoadedAssets.Add(Asset);
	}
}
