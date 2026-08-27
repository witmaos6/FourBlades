// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "FBAssetManager.generated.h"

/**
 * 
 */
UCLASS()
class FOURBLADES_API UFBAssetManager : public UAssetManager
{
	GENERATED_BODY()
	
public:
	UFBAssetManager();

	static UFBAssetManager& Get();

	virtual void StartInitialLoading() final;

	// command line에서 넣은 인자를 가져오는 함수
	static bool ShouldLogAssetLoads();

	static UObject* SynchronousLoadAsset(const FSoftObjectPath& AssetPath);

	template <typename AssetType>
	static AssetType* GetAsset(const TSoftObjectPtr<AssetType>& AssetPointer, bool bKeepInMemory = true);

	template <typename AssetType>
	static TSubclassOf<AssetType> GetSubclass(const TSoftObjectPtr<AssetType>& AssetPointer, bool bKeepInMemory = true);

	void AddLoadedAsset(const UObject* Asset);

	template<typename T>
	T* LoadPrimaryAssetSync(const FPrimaryAssetId& InAssetId);

	UPROPERTY()
	TSet<TObjectPtr<const UObject>> LoadedAssets;

	FCriticalSection SyncObject;
};

template<typename AssetType>
inline AssetType* UFBAssetManager::GetAsset(const TSoftObjectPtr<AssetType>& AssetPointer, bool bKeepInMemory)
{
	AssetType* LoadedAsset = nullptr;
	const FSoftObjectPath& AssetPath = AssetPointer.ToSoftObjectPath();
	if (AssetPath.IsValid())
	{
		LoadedAsset = AssetPointer.Get();
		if (!LoadedAsset)
		{
			LoadedAsset = Cast<AssetType>(SynchronousLoadAsset(AssetPath));
			ensureAlwaysMsgf(LoadedAsset, TEXT("Failed to Load Asset [%s]"), *AssetPointer.ToString());
		}

		if (LoadedAsset && bKeepInMemory)
		{
			Get().AddLoadedAsset(Cast<UObject>(LoadedAsset));
		}
	}

	return LoadedAsset;
}

template<typename AssetType>
inline TSubclassOf<AssetType> UFBAssetManager::GetSubclass(const TSoftObjectPtr<AssetType>& AssetPointer, bool bKeepInMemory)
{
	TSubclassOf<AssetType> LoadedAsset;
	const FSoftObjectPath& AssetPath = AssetPointer.ToSoftObjectPath();
	if (AssetPath.IsValid())
	{
		LoadedAsset = AssetPointer.Get();
		if (!LoadedAsset)
		{
			LoadedAsset = Cast<UClass>(SynchronousLoadAsset(AssetPath));
			ensureAlwaysMsgf(LoadedAsset, TEXT("Failed to Load Asset [%s]"), *AssetPointer.ToString());
		}

		if (LoadedAsset && bKeepInMemory)
		{
			Get().AddLoadedAsset(Cast<UObject>(LoadedAsset));
		}
	}

	return LoadedAsset;
}

template<typename T>
T* UFBAssetManager::LoadPrimaryAssetSync(const FPrimaryAssetId& InAssetId)
{
	static_assert(TIsDerivedFrom<T, UObject>::IsDerived, "T must be derived from UObject");

	T* LoadedAsset = GetPrimaryAssetObject<T>(InAssetId);
	if (LoadedAsset == nullptr)
	{
		FSoftObjectPath AssetIdPath = GetPrimaryAssetPath(InAssetId);
		if (AssetIdPath.IsValid())
		{
			LoadedAsset = StreamableManager.LoadSynchronous<T>(AssetIdPath);
		}
	}
	return LoadedAsset;
}
