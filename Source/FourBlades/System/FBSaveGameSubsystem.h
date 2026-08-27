// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SaveGame/FBSaveGameTypes.h"
#include "FBSaveGameSubsystem.generated.h"

/*
* SaveSlot
*/
USTRUCT(BlueprintType)
struct FFBSaveSlot
{
	GENERATED_BODY()

	FFBSaveSlot() { SaveGameInstance = nullptr; }

	FFBSaveSlot(class USaveGame* InSaveGameInstance) : SaveGameInstance(InSaveGameInstance) {}

	UPROPERTY()
	TObjectPtr<class USaveGame> SaveGameInstance;

	// SaveDescription, LastSaveTime 등등 은 이후에 추가
};

/*
* ExperienceSlot
*/
USTRUCT(BlueprintType)
struct FFBSaveGameSlots
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<FFBSaveSlot> Slots;

	void AddSlot(const FFBSaveSlot& SaveSlot)
	{
		Slots.Add(SaveSlot);
	}
};

DECLARE_MULTICAST_DELEGATE_OneParam(FOnAsyncSaveGameCompleted, bool bSuccess);
/**
 * 
 */
UCLASS()
class FOURBLADES_API UFBSaveGameSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

private:
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SaveGame", Meta = (AllowPrivateAccess = "true"))
	//TMap<EFBPlayMode, FFBSaveGameSlots> SaveGameRegistry; // UI출력등을 위한 메타데이터 저장용 테스트모드에서는 사용안함

// Index Manager
private:
	TMap<EFBPlayMode, TArray<uint8>> CachedSlotIndicesMap;

	TArray<uint8> ScanExistingSlotIndices(EFBPlayMode PlayMode);

	uint8 FindNextAvailableSlotIndex(EFBPlayMode PlayMode);

	uint8 GetMostRecentlyPlayedSlotIndex(EFBPlayMode PlayMode);

	FString GetPlayModePrefix(EFBPlayMode PlayMode) const;
// Index Manager

// Save File Manager
protected:
	UFUNCTION(BlueprintCallable, Category = "SaveGame")
	bool CreateSaveSlotAndStart(const EFBPlayMode PlayMode);

	UFUNCTION(BlueprintCallable, Category = "SaveGame")
	UFBSaveGame* CreateSaveSlot(const EFBPlayMode PlayMode);

	UFUNCTION(BlueprintCallable, Category = "SaveGame")
	bool LoadAndContinueLastGame(const EFBPlayMode PlayMode);

	UFUNCTION(BlueprintCallable, Category = "SaveGame")
	bool LoadAndStartGameFromSlot(const FString& SlotName);

	UFUNCTION(BlueprintCallable, Category = "SaveGame")
	bool HasRecentSaveGame(EFBPlayMode PlayMode);
// Save File Manager

// Current SavGame
private:
	UPROPERTY()
	TObjectPtr<class UFBSaveGame> RecentLoadSaveGame; // Host Session을 사용할 때 가장 최근에 로드된 세이브 파일을 사용 중이다. 세이브파일의 데이터를 호스팅하고 전달하는 시스템 필요

public:
	UFBSaveGame* GetRecentLoadSaveGame() { return (RecentLoadSaveGame != nullptr) ? RecentLoadSaveGame : nullptr; }

private:
	UFUNCTION(BlueprintCallable, Category = "SaveGame")
	void ResetRecentLoadSaveGame() { RecentLoadSaveGame = nullptr; }
// Current SaveGame

// 내부 인터페이스
private:
	UFBSaveGame* CreateSaveGameInstance(const EFBPlayMode PlayMode);

	FString GenerateSlotName(const EFBPlayMode PlayMode, uint8 SlotIndex);

	bool IsExperiencePrimaryAssetId(const FPrimaryAssetId& AssetId);

	const int32 UserIndex = 0; // 유저를 구분하기위한 인덱스이므로 싱글플레이에서는 0만 써도 문제 없다.
// 내부 인터페이스
// 
// Auto Save 
private:
	FTimerHandle AutoSaveTimer;

	TSet<UFBSaveGame*> PendingSaveGames;

	UPROPERTY(EditDefaultsOnly, Category = "Save Settings")
	float AutoSaveInterval = 30.0f;

	void StartAutoSave();

	void StopAutoSave();

	void PerformBatchAutoSave();

public:
	void RequestSave(UFBSaveGame* SaveGame);

	UFUNCTION(BlueprintCallable, Category = "SaveGame")
	bool SaveGameSlot(UFBSaveGame* SaveGame);

	void AsyncSaveGameSlot(UFBSaveGame* SaveGame);

	FOnAsyncSaveGameCompleted OnAsyncSaveGameCompleted;
	// Auto Save

private:
	void OnSaveCompleted(const FString& SlotName, const int32 InUserIndex, bool bSuccess);

public:
	virtual void Deinitialize() override;
};
