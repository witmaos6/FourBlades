// Fill out your copyright notice in the Description page of Project Settings.


#include "System/FBSaveGameSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "SaveGame/FBCombatModeSaveData.h"
#include "FBAssetManager.h"
#include "GameModes/FBUserFacingExperience.h"
#include "CommonSessionSubsystem.h"
#include "FourBlades.h"

void UFBSaveGameSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	CachedSlotIndicesMap.Add(EFBPlayMode::CombatMode, ScanExistingSlotIndices(EFBPlayMode::CombatMode));

	StartAutoSave();
}

TArray<uint8> UFBSaveGameSubsystem::ScanExistingSlotIndices(EFBPlayMode PlayMode)
{
	TArray<uint8> ExistingIndices;

	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	FString SaveDir = FPaths::ProjectSavedDir() / TEXT("SaveGames");

	if (!PlatformFile.DirectoryExists(*SaveDir))
	{
		FB_FUNC_LOG(LogTemp, Log, TEXT("SaveGames directory does not exist: %s"), *SaveDir);
		return ExistingIndices;
	}

	TArray<FString> FoundFiles;
	PlatformFile.FindFilesRecursively(FoundFiles, *SaveDir, TEXT(".sav"));

	FString Prefix = GetPlayModePrefix(PlayMode);

	for (const FString& FilePath : FoundFiles)
	{
		FString FileName = FPaths::GetBaseFilename(FilePath);

		if (FileName.StartsWith(Prefix))
		{
			FString IndexStr = FileName.RightChop(Prefix.Len());

			if (IndexStr.Len() == 2 && IndexStr.IsNumeric())
			{
				uint8 Index = FCString::Atoi(*IndexStr);
				ExistingIndices.Add(Index);
			}
		}
	}
	ExistingIndices.Sort();

	FB_FUNC_LOG(LogTemp, Log, TEXT("[%s] Found %d existing save slots"), *UEnum::GetValueAsString(PlayMode), ExistingIndices.Num());

	return ExistingIndices;
}

uint8 UFBSaveGameSubsystem::FindNextAvailableSlotIndex(EFBPlayMode PlayMode)
{
	TArray<uint8>& CachedIndices = CachedSlotIndicesMap.FindOrAdd(PlayMode);
	if (CachedIndices.Num() == 0)
	{
		CachedIndices = ScanExistingSlotIndices(PlayMode);
	}

	for (uint8 i = 0; i < 255; ++i)
	{
		if (!CachedIndices.Contains(i))
		{
			return i;
		}
	}
	FB_FUNC_LOG(LogTemp, Error, TEXT("All 255 save slots are in use for PlayMode: %s"), *UEnum::GetValueAsString(PlayMode));
	return 255;
}

uint8 UFBSaveGameSubsystem::GetMostRecentlyPlayedSlotIndex(EFBPlayMode PlayMode)
{
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	FString SaveDir = FPaths::ProjectSavedDir() / TEXT("SaveGames");

	if (!PlatformFile.DirectoryExists(*SaveDir))
	{
		return 255;
	}

	TArray<FString> FoundFiles;
	PlatformFile.FindFilesRecursively(FoundFiles, *SaveDir, TEXT(".sav"));

	FString Prefix = GetPlayModePrefix(PlayMode);

	FDateTime LatestTime = FDateTime::MinValue();
	uint8 LatestSlotIndex = 255;
	bool bFoundAny = false;

	for (const FString& FilePath : FoundFiles)
	{
		FString FileName = FPaths::GetBaseFilename(FilePath);

		if (FileName.StartsWith(Prefix))
		{
			FDateTime FileTime = PlatformFile.GetTimeStamp(*FilePath);

			if (FileTime > LatestTime)
			{
				LatestTime = FileTime;

				FString IndexStr = FileName.RightChop(Prefix.Len());
				if (IndexStr.Len() == 2 && IndexStr.IsNumeric())
				{
					LatestSlotIndex = FCString::Atoi(*IndexStr);
					bFoundAny = true;
				}
			}
		}
	}

	if (bFoundAny)
	{
		FB_FUNC_LOG(LogTemp, Log, TEXT("Most recently played slot: %d (Last modified: %s)"), LatestSlotIndex, *LatestTime.ToString());
	}
	else
	{
		FB_FUNC_LOG(LogTemp, Warning, TEXT("No save files found for PlayMode: %s"), *UEnum::GetValueAsString(PlayMode));
	}
	return LatestSlotIndex;
}

FString UFBSaveGameSubsystem::GetPlayModePrefix(EFBPlayMode PlayMode) const
{
	FString PlayModeStr = UEnum::GetValueAsString(PlayMode);
	PlayModeStr = PlayModeStr.Right(PlayModeStr.Len() - PlayModeStr.Find("::") - 2);
	return FString::Printf(TEXT("%s_Slot_"), *PlayModeStr);
}

bool UFBSaveGameSubsystem::CreateSaveSlotAndStart(const EFBPlayMode PlayMode)
{
	UFBSaveGame* SaveGame = CreateSaveSlot(PlayMode);
	if (SaveGame == nullptr)
	{
		FB_FUNC_LOG(LogTemp, Error, TEXT("Failed to create save slot"));
		return false;
	}
	FB_FUNC_LOG(LogTemp, Log, TEXT("Success Create SaveGame %s"), *SaveGame->GetSlotName());

	return LoadAndStartGameFromSlot(SaveGame->GetSlotName());
}

UFBSaveGame* UFBSaveGameSubsystem::CreateSaveSlot(const EFBPlayMode PlayMode)
{
	uint8 NewSlotIndex = FindNextAvailableSlotIndex(PlayMode);
	if (NewSlotIndex == 255)
	{
		FB_FUNC_LOG(LogTemp, Error, TEXT("Cannot create new slot: all slots in use"));
		return nullptr;
	}

	FString SlotName = GenerateSlotName(PlayMode, NewSlotIndex);
	if (UGameplayStatics::DoesSaveGameExist(SlotName, UserIndex))
	{
		FB_FUNC_LOG(LogTemp, Warning, TEXT("Slot collision detected at index %d, rescanning..."), NewSlotIndex);
		CachedSlotIndicesMap[PlayMode] = ScanExistingSlotIndices(PlayMode);
		return nullptr;
	}

	UFBSaveGame* SaveGame = CreateSaveGameInstance(PlayMode);
	if (!SaveGame)
	{
		FB_FUNC_LOG(LogTemp, Error, TEXT("Failed to create SaveGame instance"));
		return nullptr;
	}
	SaveGame->SetSlotName(SlotName);

	if (!UGameplayStatics::SaveGameToSlot(SaveGame, SlotName, UserIndex))
	{
		FB_FUNC_LOG(LogTemp, Error, TEXT("Failed to save game to slot: %s"), *SlotName);
		return nullptr;
	}

	TArray<uint8>& CachedIndices = CachedSlotIndicesMap.FindOrAdd(PlayMode);
	CachedIndices.Add(NewSlotIndex);
	CachedIndices.Sort();

	FB_FUNC_LOG(LogTemp, Log, TEXT("Created new save slot: %s (Index: %d)"), *SlotName, NewSlotIndex);
	return SaveGame;
}

bool UFBSaveGameSubsystem::LoadAndContinueLastGame(const EFBPlayMode PlayMode)
{
	uint8 LatestSlotIndex = GetMostRecentlyPlayedSlotIndex(PlayMode);
	if (LatestSlotIndex == 255)
	{
		FB_FUNC_LOG(LogTemp, Warning, TEXT("No recent save file found for continue"));
		return false;
	}

	FString LatestSlotName = GenerateSlotName(PlayMode, LatestSlotIndex);
	return LoadAndStartGameFromSlot(LatestSlotName);
}

bool UFBSaveGameSubsystem::LoadAndStartGameFromSlot(const FString& SlotName)
{
	UFBSaveGame* LoadedSaveGame = Cast<UFBSaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName, UserIndex));
	if (LoadedSaveGame == nullptr)
	{
		return false;
	}

	RecentLoadSaveGame = LoadedSaveGame;

	FPrimaryAssetId ExperienceId = LoadedSaveGame->GetUserFacingExperienceId();
	if (!ExperienceId.IsValid() || !IsExperiencePrimaryAssetId(ExperienceId))
	{
		FB_FUNC_LOG(LogTemp, Error, TEXT("Invalid ExperienceId in SaveGame"));
		return false;
	}

	UAssetManager& AssetManager = UAssetManager::Get();
	UFBUserFacingExperience* UserFacingExperience = Cast<UFBUserFacingExperience>(AssetManager.GetPrimaryAssetObject(ExperienceId));
	if (UserFacingExperience == nullptr)
	{
		TSharedPtr<FStreamableHandle> Handle = AssetManager.LoadPrimaryAsset(ExperienceId);
		if (!Handle.IsValid())
		{
			FB_FUNC_LOG(LogTemp, Error, TEXT("Failed to load Experience asset"));
			return false;
		}
		Handle->WaitUntilComplete();
	}

	UserFacingExperience = Cast<UFBUserFacingExperience>(AssetManager.GetPrimaryAssetObject(ExperienceId));
	if (UserFacingExperience == nullptr)
	{
		FB_FUNC_LOG(LogTemp, Error, TEXT("Experience asset is null after loading"));
		return false;
	}

	UCommonSession_HostSessionRequest* HostingRequest = UserFacingExperience->CreateHostingRequest();
	if (HostingRequest == nullptr)
	{
		FB_FUNC_LOG(LogTemp, Error, TEXT("Failed to create hosting request"));
		return false;
	}

	UCommonSessionSubsystem* SessionSubsystem = GetGameInstance()->GetSubsystem<UCommonSessionSubsystem>();
	if (SessionSubsystem == nullptr)
	{
		FB_FUNC_LOG(LogTemp, Error, TEXT("SessionSubsystem is null"));
		return false;
	}

	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (PlayerController == nullptr)
	{
		FB_FUNC_LOG(LogTemp, Error, TEXT("PlayerController is null"));
		return false;
	}

	SessionSubsystem->HostSession(PlayerController, HostingRequest);
	return true;
}

bool UFBSaveGameSubsystem::HasRecentSaveGame(EFBPlayMode PlayMode)
{
	uint8 LatestSlotIndex = GetMostRecentlyPlayedSlotIndex(PlayMode);
	return LatestSlotIndex != 255;
}


UFBSaveGame* UFBSaveGameSubsystem::CreateSaveGameInstance(const EFBPlayMode PlayMode)
{
	switch (PlayMode)
	{
	case EFBPlayMode::CombatMode:
		return NewObject<UFBCombatModeSaveData>();
		break;
	default:
		break;
	}

	FB_FUNC_LOG(LogTemp, Error, TEXT("Unknown PlayMode: %s"), *UEnum::GetValueAsString(PlayMode));
	return nullptr;
}

FString UFBSaveGameSubsystem::GenerateSlotName(const EFBPlayMode PlayMode, uint8 SlotIndex)
{
	FString SlotName = UEnum::GetValueAsString(PlayMode);
	SlotName = SlotName.Right(SlotName.Len() - SlotName.Find("::") - 2);
	return FString::Printf(TEXT("%s_Slot_%02u"), *SlotName, SlotIndex);
}

bool UFBSaveGameSubsystem::IsExperiencePrimaryAssetId(const FPrimaryAssetId& AssetId)
{
	return AssetId.PrimaryAssetType == FPrimaryAssetType("FBUserFacingExperience");
}

void UFBSaveGameSubsystem::StartAutoSave()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(AutoSaveTimer, this, &UFBSaveGameSubsystem::PerformBatchAutoSave, AutoSaveInterval, true);
	}
}

void UFBSaveGameSubsystem::StopAutoSave()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(AutoSaveTimer);
	}
}

void UFBSaveGameSubsystem::PerformBatchAutoSave()
{
	if (PendingSaveGames.Num() > 0)
	{
		FB_FUNC_LOG(LogTemp, Log, TEXT("Performing batch auto-save for %d pending saves"), PendingSaveGames.Num());

		for (UFBSaveGame* SaveGame : PendingSaveGames)
		{
			AsyncSaveGameSlot(SaveGame);
		}
		PendingSaveGames.Empty();
	}
}

void UFBSaveGameSubsystem::RequestSave(UFBSaveGame* SaveGame)
{
	if (SaveGame)
	{
		PendingSaveGames.Add(SaveGame);
	}
}

bool UFBSaveGameSubsystem::SaveGameSlot(UFBSaveGame* SaveGame)
{
	if (SaveGame == nullptr)
	{
		FB_FUNC_LOG(LogTemp, Warning, TEXT("SaveGame is null"));
		return false;
	}

	if (!SaveGame->ValidSlotName())
	{
		FB_FUNC_LOG(LogTemp, Warning, TEXT("SaveGame has invalid slot name"));
		return false;
	}

	const FString& SlotName = SaveGame->GetSlotName();

	if (!UGameplayStatics::SaveGameToSlot(SaveGame, SlotName, UserIndex))
	{
		FB_FUNC_LOG(LogTemp, Warning, TEXT("SaveGame failed: %s"), *SlotName);
		return false;
	}

	FB_FUNC_LOG(LogTemp, Log, TEXT("SaveGame successful: %s"), *SlotName);
	return true;
}

void UFBSaveGameSubsystem::AsyncSaveGameSlot(UFBSaveGame* SaveGame)
{
	if (SaveGame && SaveGame->ValidSlotName())
	{
		UGameplayStatics::AsyncSaveGameToSlot(SaveGame, SaveGame->GetSlotName(), UserIndex, FAsyncSaveGameToSlotDelegate::CreateUObject(this, &UFBSaveGameSubsystem::OnSaveCompleted));
	}
	else
	{
		FB_FUNC_LOG(LogTemp, Warning, TEXT("AsyncSave failed: SaveGame is null or has invalid slot name"));
	}
}

void UFBSaveGameSubsystem::OnSaveCompleted(const FString& SlotName, const int32 InUserIndex, bool bSuccess)
{
	if (bSuccess)
	{
		FB_FUNC_LOG(LogTemp, Log, TEXT("Async save completed successfully: %s"), *SlotName);
	}
	else
	{
		FB_FUNC_LOG(LogTemp, Error, TEXT("Async save failed: %s"), *SlotName);
	}

	if (OnAsyncSaveGameCompleted.IsBound())
	{
		OnAsyncSaveGameCompleted.Broadcast(bSuccess);
		OnAsyncSaveGameCompleted.Clear();
	}
}

void UFBSaveGameSubsystem::Deinitialize()
{
	StopAutoSave();

	if (PendingSaveGames.Num() > 0)
	{
		PerformBatchAutoSave();
	}
	Super::Deinitialize();
}
