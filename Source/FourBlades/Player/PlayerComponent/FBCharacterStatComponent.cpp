// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/PlayerComponent/FBCharacterStatComponent.h"
#include "System/FBGameSingleton.h"
#include "Player/FBPlayerState.h"
#include "AbilitySystem/Attributes/FBHealthSet.h"
#include "AbilitySystem/Attributes/FBCombatSet.h"
#include "GameDataFromExcel/FBCharacterStatFromLevelTable.h"
#include "GameDataFromExcel/FBCharacterHealthCoefficient.h"
#include "AbilitySystem/FBAbilitySystemComponent.h"
#include "GameplayEffect.h"
#include "GameplayEffectTypes.h"
#include "SaveGame/Interface/FBCharacterProfileInterface.h"
#include "Kismet/GameplayStatics.h"
#include "System/FBSaveGameSubsystem.h"
#include "GameplayTagsManager.h"
#include "Library/GameplayTagLevelUtilities.h"
#include "GameModes/FBGameModeBase.h"
#include "SaveGame/FBSaveGame.h"
#include "FourBlades.h"

UFBCharacterStatComponent::UFBCharacterStatComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UFBCharacterStatComponent::OnRegister()
{
	Super::OnRegister();

	InitializeAttributeMapping();

	AFBPlayerState* Owner = Cast<AFBPlayerState>(GetOwner());
	if (Owner)
	{
		if (UFBSaveGame* OwnerSaveData = Owner->GetSaveData())
		{
			SaveData = OwnerSaveData;

			if (IFBCharacterProfileInterface* CharacterProfile = Cast<IFBCharacterProfileInterface>(SaveData))
			{
				CharacterProfile->GetCharacterStatChanged().AddUObject(this, &UFBCharacterStatComponent::UpdateCharacterStat);
			}
		}

		if (Owner->bOnPostInitializeComponent)
		{
			InitializeStats();
		}
		else
		{
			Owner->OnPostInitializeComponent.AddUObject(this, &UFBCharacterStatComponent::InitializeStats);
		}
	}
}

void UFBCharacterStatComponent::InitializeAttributeMapping()
{
	StatTagToAttributeMap.Add(FBTAG_CHARACTERSTAT_ATTACKPOWER, UFBCombatSet::GetAttackPowerAttribute());
	StatTagToAttributeMap.Add(FBTAG_CHARACTERSTAT_HEALTH, UFBHealthSet::GetMaxHealthAttribute());
	StatTagToAttributeMap.Add(FBTAG_CHARACTERSTAT_DEFENSE, UFBHealthSet::GetDefenseAttribute());
}

void UFBCharacterStatComponent::BeginPlay()
{
	Super::BeginPlay();

	if (AFBGameModeBase* GameMode = Cast<AFBGameModeBase>(UGameplayStatics::GetGameMode(GetOwner())))
	{
		GameMode->OnPlayerResurrectionDelegate.AddUObject(this, &UFBCharacterStatComponent::Resurrection);
	}
}

void UFBCharacterStatComponent::InitializeStats()
{
	FGameplayTagContainer RequireTags = GetRequireTags();

	UFBGameSingleton::Get().CallOrRegister_OnDataTableInitComplete(FOnDataTableInitComplete::FDelegate::CreateUObject(this, &UFBCharacterStatComponent::InitializeCharacterStat), RequireTags);
}

FGameplayTagContainer UFBCharacterStatComponent::GetRequireTags()
{
	FGameplayTagContainer RequireTags;
	RequireTags.AddTag(CharacterStatByLevelTag);

	// 직업 계수를 사용할 때 복원, 그런데 각 Entry구조체를 AttackPower, Defense 등 별개의 멤버 변수로 만들어서 필요했던 함수라 변형시켜야 할 것 같다.
	/*for (TFieldIterator<FStructProperty>It(GetClass()); It; ++It) 
	{
		FStructProperty* Property = *It;

		if (Property->Struct == FFBCharacterStatEntry::StaticStruct())
		{
			FFBCharacterStatEntry* StructData = Property->ContainerPtrToValuePtr<FFBCharacterStatEntry>(this);
			if (StructData)
			{
				RequireTags.AddTag(StructData->JobCoefficientTag);
			}
		}
	}*/

	return RequireTags;
}

void UFBCharacterStatComponent::InitializeCharacterStat()
{	
	AFBPlayerState* PlayerState = Cast<AFBPlayerState>(GetOwner());
	if (!IsValid(PlayerState))
	{
		return;
	}
	if (SaveData == nullptr)
	{
		if (UFBSaveGame* OwnerSaveData = PlayerState->GetSaveData())
		{
			SaveData = OwnerSaveData;

			if (IFBCharacterProfileInterface* CharacterProfile = Cast<IFBCharacterProfileInterface>(SaveData))
			{
				CharacterProfile->GetCharacterStatChanged().AddUObject(this, &UFBCharacterStatComponent::UpdateCharacterStat);
			}
		}
		if (SaveData == nullptr)
		{
			FB_FUNC_LOG(LogTemp, Warning, TEXT("SaveData is null"));
			return;
		}
	}
	IFBCharacterProfileInterface* CharacterProfile = Cast<IFBCharacterProfileInterface>(SaveData);
	if (CharacterProfile == nullptr)
	{
		FB_FUNC_LOG(LogTemp, Warning, TEXT("SaveData not FBCharacterProfileInterface"));
		return;
	}
	UFBGameSingleton& GameData = UFBGameSingleton::Get();
	FFBCharacterStatFromLevelTable* CharacterStatTableRow = GameData.GetDataTableRow<FFBCharacterStatFromLevelTable>(CharacterStatByLevelTag, CharacterProfile->GetCharacterLevel());
	if (CharacterStatTableRow == nullptr)
	{
		FB_FUNC_LOG(LogTemp, Warning, TEXT("Table is null"));
		return;
	}

	FFBStatModifierEntry AttackPowerEntry(FBTAG_CHARACTERSTAT_ATTACKPOWER_LEVEL, CharacterStatTableRow->AttackPower);
	CharacterStatTable.SetStatEntry(AttackPowerEntry);

	FFBStatModifierEntry DefenseEntry(FBTAG_CHARACTERSTAT_DEFENSE_LEVEL, CharacterStatTableRow->Defense);
	CharacterStatTable.SetStatEntry(DefenseEntry);

	FFBStatModifierEntry HealthEntry(FBTAG_CHARACTERSTAT_HEALTH_LEVEL, CharacterStatTableRow->MaxAddHealth);
	CharacterStatTable.SetStatEntry(HealthEntry);
	
	UpdateAllocatedStatPoint(CharacterProfile->GetStatSaveData());

	InitializeAttributeSet();

	if (OnUpdateCharacterStat.IsBound())
	{
		OnUpdateCharacterStat.Broadcast();
	}
}

void UFBCharacterStatComponent::InitializeAttributeSet()
{
	AFBPlayerState* PlayerState = Cast<AFBPlayerState>(GetOwner());
	if (PlayerState == nullptr)
		return;

	if (UFBAbilitySystemComponent* ASC = PlayerState->GetFBAbilitySystemComponent())
	{
		UGameplayEffect* StatInitEffect = NewObject<UGameplayEffect>(GetTransientPackage(), FName("StatInitEffect"));
		StatInitEffect->DurationPolicy = EGameplayEffectDurationType::Instant;

		StatInitEffect->Modifiers.SetNum(4);
	
		// HealthSet
		FGameplayTag HealthTag = FBTAG_CHARACTERSTAT_HEALTH;
		StatInitEffect->Modifiers[0].Attribute = StatTagToAttributeMap.FindChecked(HealthTag);
		StatInitEffect->Modifiers[0].ModifierOp = EGameplayModOp::Override;
		StatInitEffect->Modifiers[0].ModifierMagnitude = FScalableFloat(CharacterStatTable.GetTotalStat(HealthTag));

		StatInitEffect->Modifiers[1].Attribute = UFBHealthSet::GetHealthAttribute();
		StatInitEffect->Modifiers[1].ModifierOp = EGameplayModOp::Override;
		StatInitEffect->Modifiers[1].ModifierMagnitude = FScalableFloat(CharacterStatTable.GetTotalStat(HealthTag));

		FGameplayTag DefenseTag = FBTAG_CHARACTERSTAT_DEFENSE;
		StatInitEffect->Modifiers[2].Attribute = StatTagToAttributeMap.FindChecked(DefenseTag);
		StatInitEffect->Modifiers[2].ModifierOp = EGameplayModOp::Override;
		StatInitEffect->Modifiers[2].ModifierMagnitude = FScalableFloat(CharacterStatTable.GetTotalStat(DefenseTag));

		// CombatSet
		FGameplayTag AttackPowerTag = FBTAG_CHARACTERSTAT_ATTACKPOWER;
		StatInitEffect->Modifiers[3].Attribute = StatTagToAttributeMap.FindChecked(AttackPowerTag);
		StatInitEffect->Modifiers[3].ModifierOp = EGameplayModOp::Override;
		StatInitEffect->Modifiers[3].ModifierMagnitude = FScalableFloat(CharacterStatTable.GetTotalStat(AttackPowerTag));

		FGameplayEffectContextHandle EffectContextHandle = ASC->MakeEffectContext();
		EffectContextHandle.AddSourceObject(PlayerState);
		ASC->ApplyGameplayEffectToSelf(StatInitEffect, 1.0f, EffectContextHandle);
	}
}

void UFBCharacterStatComponent::UpdateCharacterStat(const FFBStatSaveData& InStatSaveData)
{
	UpdateAllocatedStatPoint(InStatSaveData);

	UpdateAttributeSet();

	if (OnUpdateCharacterStat.IsBound())
	{
		OnUpdateCharacterStat.Broadcast();
	}
}

void UFBCharacterStatComponent::UpdateAllocatedStatPoint(const FFBStatSaveData& InStatSaveData)
{
	for (const auto& [StatIdTag, Value] : InStatSaveData.AllocatedLevelUpStats)
	{
		FFBStatModifierEntry Entry(StatIdTag, static_cast<float>(Value));
		CharacterStatTable.SetStatEntry(Entry);
	}
}

void UFBCharacterStatComponent::UpdateAttributeSet()
{
	AFBPlayerState* PlayerState = Cast<AFBPlayerState>(GetOwner());
	if (PlayerState == nullptr)
		return;

	if (UFBAbilitySystemComponent* ASC = PlayerState->GetFBAbilitySystemComponent())
	{
		UGameplayEffect* StatInitEffect = NewObject<UGameplayEffect>(GetTransientPackage(), FName("StatInitEffect"));
		StatInitEffect->DurationPolicy = EGameplayEffectDurationType::Instant;

		StatInitEffect->Modifiers.SetNum(3);

		// HealthSet
		FGameplayTag HealthTag = FBTAG_CHARACTERSTAT_HEALTH;
		StatInitEffect->Modifiers[0].Attribute = StatTagToAttributeMap.FindChecked(HealthTag);
		StatInitEffect->Modifiers[0].ModifierOp = EGameplayModOp::Override;
		StatInitEffect->Modifiers[0].ModifierMagnitude = FScalableFloat(CharacterStatTable.GetTotalStat(HealthTag));

		FGameplayTag DefenseTag = FBTAG_CHARACTERSTAT_DEFENSE;
		StatInitEffect->Modifiers[1].Attribute = StatTagToAttributeMap.FindChecked(DefenseTag);
		StatInitEffect->Modifiers[1].ModifierOp = EGameplayModOp::Override;
		StatInitEffect->Modifiers[1].ModifierMagnitude = FScalableFloat(CharacterStatTable.GetTotalStat(DefenseTag));

		// CombatSet
		FGameplayTag AttackPowerTag = FBTAG_CHARACTERSTAT_ATTACKPOWER;
		StatInitEffect->Modifiers[2].Attribute = StatTagToAttributeMap.FindChecked(AttackPowerTag);
		StatInitEffect->Modifiers[2].ModifierOp = EGameplayModOp::Override;
		StatInitEffect->Modifiers[2].ModifierMagnitude = FScalableFloat(CharacterStatTable.GetTotalStat(AttackPowerTag));

		FGameplayEffectContextHandle EffectContextHandle = ASC->MakeEffectContext();
		EffectContextHandle.AddSourceObject(PlayerState);
		ASC->ApplyGameplayEffectToSelf(StatInitEffect, 1.0f, EffectContextHandle);
	}
}

const float UFBCharacterStatComponent::GetLastTotalStat(const FGameplayTag& StatTag)
{
	return CharacterStatTable.GetLastTotalStat(StatTag);
}

void UFBCharacterStatComponent::AddXP(const uint32 Amount)
{
	if (SaveData)
	{
		IFBCharacterProfileInterface* CharacterProfileInterface = Cast<IFBCharacterProfileInterface>(SaveData);
		if (CharacterProfileInterface)
		{
			CharacterProfileInterface->AddXP(Amount);
			if (UFBSaveGameSubsystem* FBSaveGameSubsystem = UGameplayStatics::GetGameInstance(GetWorld())->GetSubsystem<UFBSaveGameSubsystem>())
			{
				if (CharacterProfileInterface->PossibleLevelUp())
				{
					CharacterProfileInterface->LevelUp();

					LevelUp();

					FBSaveGameSubsystem->AsyncSaveGameSlot(SaveData);
				}
				else
				{
					FBSaveGameSubsystem->RequestSave(SaveData);
				}
			}
		}
	}
}

void UFBCharacterStatComponent::LevelUp()
{
	if (SaveData == nullptr)
		return;

	// To do: 캐릭터가 레벨업을 할 때 Particle생성 및 사운드 재생
	InitializeCharacterStat();
}

void UFBCharacterStatComponent::Resurrection()
{
	AFBPlayerState* PlayerState = Cast<AFBPlayerState>(GetOwner());
	if (PlayerState == nullptr)
		return;

	if (UFBAbilitySystemComponent* ASC = PlayerState->GetFBAbilitySystemComponent())
	{
		UGameplayEffect* StatInitEffect = NewObject<UGameplayEffect>(GetTransientPackage(), FName("ResurrectionEffect"));
		StatInitEffect->DurationPolicy = EGameplayEffectDurationType::Instant;
		StatInitEffect->Modifiers.SetNum(1);

		// HealthSet
		FGameplayTag HealthTag = FBTAG_CHARACTERSTAT_HEALTH;
		StatInitEffect->Modifiers[0].Attribute = UFBHealthSet::GetHealthAttribute();
		StatInitEffect->Modifiers[0].ModifierOp = EGameplayModOp::Override;
		StatInitEffect->Modifiers[0].ModifierMagnitude = FScalableFloat(CharacterStatTable.GetTotalStat(HealthTag));

		FGameplayEffectContextHandle EffectContextHandle = ASC->MakeEffectContext();
		EffectContextHandle.AddSourceObject(PlayerState);
		ASC->ApplyGameplayEffectToSelf(StatInitEffect, 1.0f, EffectContextHandle);
	}
}

void UFBCharacterStatComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (OnUpdateCharacterStat.IsBound())
	{
		OnUpdateCharacterStat.Clear();
	}
	Super::EndPlay(EndPlayReason);
}

/*
* CharacterStatTable
*/
void FFBCharacterStatTable::SetStatEntry(const FFBStatModifierEntry& InModifierEntry)
{
	if (!InModifierEntry.StatIdTag.IsValid())
	{
		FB_FUNC_LOG(LogTemp, Warning, TEXT("Entry has no StatIdTag"));
		return;
	}

	FGameplayTag Level2Tag = UGameplayTagLevelUtilities::GetTagAtLevel(InModifierEntry.StatIdTag, 2);
	if (Level2Tag.IsValid() && IsValidEntryTag(Level2Tag))
	{
		if (CharacterStatEntries.Contains(Level2Tag))
		{
			if (!CharacterStatEntries[Level2Tag].SetStatValue(InModifierEntry))
			{
				FB_FUNC_LOG(LogTemp, Warning, TEXT("Add StatValue Failed"));
			}
		}
		else
		{
			FFBCharacterStatEntry StatEntry = FFBCharacterStatEntry(Level2Tag);
			if (StatEntry.SetStatValue(InModifierEntry))
			{
				CharacterStatEntries.Add(Level2Tag, StatEntry);
			}
			else
			{
				FB_FUNC_LOG(LogTemp, Warning, TEXT("SetStatValue Failed"));
			}
		}
	}
}

const float FFBCharacterStatTable::GetTotalStat(const FGameplayTag& StatTag)
{
	if (!IsValidSourceTag(StatTag))
	{
		return -1.0f;
	}

	if (CharacterStatEntries.Contains(StatTag))
	{
		FFBCharacterStatEntry& StatEntry = CharacterStatEntries.FindChecked(StatTag);
		const float TotalStat = StatEntry.GetTotalStat();
		LastTotalStat.Add(StatTag, TotalStat);
		return TotalStat;
	}
	return -1.0f;
}

const float FFBCharacterStatTable::GetLastTotalStat(const FGameplayTag& StatTag)
{
	if (!IsValidSourceTag(StatTag))
	{
		return -1.0f;
	}
	if (LastTotalStat.Contains(StatTag))
	{
		return LastTotalStat[StatTag];
	}
	
	const float TotalStat = GetTotalStat(StatTag);
	return TotalStat;
}

bool FFBCharacterStatTable::IsValidEntryTag(const FGameplayTag& StatTag) const
{
	if (!StatTag.IsValid())
	{
		FB_FUNC_LOG(LogTemp, Warning, TEXT("Invalid Tag"));
		return false;
	}
	return AllowedSourceTags.HasTag(StatTag);
}

bool FFBCharacterStatTable::IsValidSourceTag(const FGameplayTag& StatTag) const
{
	if (!StatTag.IsValid())
	{
		FB_FUNC_LOG(LogTemp, Warning, TEXT("Invalid Tag"));
		return false;
	}
	FGameplayTag Level2Tag = UGameplayTagLevelUtilities::GetTagAtLevel(StatTag, 2);
	if (!Level2Tag.IsValid())
	{
		FB_FUNC_LOG(LogTemp, Warning, TEXT("Invalid Level2Tag"));
		return false;
	}

	const TArray<FGameplayTag>& TagArray = AllowedSourceTags.GetGameplayTagArray();
	for (const FGameplayTag& AllowedTag : TagArray)
	{
		if (AllowedTag.MatchesTagExact(Level2Tag))
		{
			return true;
		}
	}
	FB_FUNC_LOG(LogTemp, Warning, TEXT("Invalid StatIdTag"));
	return false;
}

/*
* CharacterStatEntry
*/
FFBCharacterStatEntry::FFBCharacterStatEntry(const FGameplayTag& InParentTag) : ParentStatTag(InParentTag)
{
	const UGameplayTagsManager& TagManager = UGameplayTagsManager::Get();
	const FGameplayTagContainer CharacterStatTags = TagManager.RequestGameplayTagChildren(InParentTag);

	for (const FGameplayTag& StatTag : CharacterStatTags)
	{
		AllowedSourceTags.AddTag(StatTag);
	}
}
bool FFBCharacterStatEntry::IsValidSourceTag(const FGameplayTag& SourceTag) const
{
	FGameplayTag Level2Tag = UGameplayTagLevelUtilities::GetTagAtLevel(SourceTag, 2);
	if (!ParentStatTag.MatchesTagExact(Level2Tag))
		return false;
	return AllowedSourceTags.IsEmpty() || AllowedSourceTags.HasTag(SourceTag);
}

bool FFBCharacterStatEntry::SetStatValue(const FFBStatModifierEntry& InModifierEntry)
{
	FGameplayTag Level3Tag = UGameplayTagLevelUtilities::GetTagAtLevel(InModifierEntry.StatIdTag, 3);
	if (!Level3Tag.IsValid())
	{
		FB_FUNC_LOG(LogTemp, Warning, TEXT("Invalid Level3Tag"));
		return false;
	}
	if (!IsValidSourceTag(InModifierEntry.StatIdTag))
	{
		FB_FUNC_LOG(LogTemp, Warning, TEXT("Invalid StatIdTag"));
		return false;
	}

	StatValues.Add(InModifierEntry.StatIdTag, InModifierEntry.Value);
	return true;
}

const float FFBCharacterStatEntry::GetTotalStat() const
{
	float Additive = 0.0f;
	for (const auto& [SourceTag, Value] : StatValues)
	{
		Additive += Value;
	}
	return Additive;
}

