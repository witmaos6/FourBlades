// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/CharacterStatus/FBStatPointAllocatorWidget.h"
#include "Player/FBPlayerState.h"
#include "SaveGame/Interface/FBCharacterProfileInterface.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "System/FBSaveGameSubsystem.h"
#include "SaveGame/FBSaveGame.h"
#include "FourBlades.h"

UFBStatPointAllocatorWidget::UFBStatPointAllocatorWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	TempRemainingPoints = 0;
}

void UFBStatPointAllocatorWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	BindButtonEvents();
}

void UFBStatPointAllocatorWidget::BindButtonEvents()
{
	if (AttackPowerPointUpButton)
		AttackPowerPointUpButton->OnClicked.AddDynamic(this, &UFBStatPointAllocatorWidget::OnAttackUpClicked);

	if (AttackPowerPointDownButton)
		AttackPowerPointDownButton->OnClicked.AddDynamic(this, &UFBStatPointAllocatorWidget::OnAttackDownClicked);

	if (DefensePointUpButton)
		DefensePointUpButton->OnClicked.AddDynamic(this, &UFBStatPointAllocatorWidget::OnDefenseUpClicked);

	if (DefensePointDownButton)
		DefensePointDownButton->OnClicked.AddDynamic(this, &UFBStatPointAllocatorWidget::OnDefenseDownClicked);

	if (HealthPointUpButton)
		HealthPointUpButton->OnClicked.AddDynamic(this, &UFBStatPointAllocatorWidget::OnHealthUpClicked);

	if (HealthPointDownButton)
		HealthPointDownButton->OnClicked.AddDynamic(this, &UFBStatPointAllocatorWidget::OnHealthDownClicked);

	if (ApplyButton)
		ApplyButton->OnClicked.AddDynamic(this, &UFBStatPointAllocatorWidget::OnApplyClicked);

	if (CancelButton)
		CancelButton->OnClicked.AddDynamic(this, &UFBStatPointAllocatorWidget::OnCancelClicked);
}

void UFBStatPointAllocatorWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (AFBPlayerState* PS = Cast<AFBPlayerState>(GetOwningPlayerState()))
	{
		if (UFBSaveGame* SaveGame = PS->GetSaveData())
		{
			if (IFBCharacterProfileInterface* CharacterProfile = Cast<IFBCharacterProfileInterface>(SaveGame))
			{
				SaveData = SaveGame;

				CharacterProfileInterface = CharacterProfile;
				CharacterProfile->GetLevelUpDelegate().AddUObject(this, &UFBStatPointAllocatorWidget::LevelUp);

				InitializeTempData();
				
				UpdateAllUI();
			}
		}
	}
}

void UFBStatPointAllocatorWidget::LevelUp(uint8 OldLevel, uint8 CurrentLevel)
{
	if (CharacterProfileInterface.IsValid())
	{
		IFBCharacterProfileInterface* Interface = CharacterProfileInterface.Get();
		TempRemainingPoints = Interface->GetRemainStatPoints();

		UpdateAllUI();
	}
}

void UFBStatPointAllocatorWidget::InitializeTempData()
{
	if (!CharacterProfileInterface.IsValid())
	{
		return;
	}

	IFBCharacterProfileInterface* Interface = CharacterProfileInterface.Get();
	if (Interface == nullptr)
		return;

	const FFBStatSaveData& StatData = Interface->GetStatSaveData();

	BaseAllocatedPoints.Empty();
	BaseAllocatedPoints.Add(FBTAG_CHARACTERSTAT_ATTACKPOWER_ALLOCATEDPOINT, StatData.AllocatedLevelUpStats.FindRef(FBTAG_CHARACTERSTAT_ATTACKPOWER_ALLOCATEDPOINT));
	BaseAllocatedPoints.Add(FBTAG_CHARACTERSTAT_DEFENSE_ALLOCATEDPOINT, StatData.AllocatedLevelUpStats.FindRef(FBTAG_CHARACTERSTAT_DEFENSE_ALLOCATEDPOINT));
	BaseAllocatedPoints.Add(FBTAG_CHARACTERSTAT_HEALTH_ALLOCATEDPOINT, StatData.AllocatedLevelUpStats.FindRef(FBTAG_CHARACTERSTAT_HEALTH_ALLOCATEDPOINT));

	ChangeDelta.Empty();
	ChangeDelta.Add(FBTAG_CHARACTERSTAT_ATTACKPOWER_ALLOCATEDPOINT, 0);
	ChangeDelta.Add(FBTAG_CHARACTERSTAT_DEFENSE_ALLOCATEDPOINT, 0);
	ChangeDelta.Add(FBTAG_CHARACTERSTAT_HEALTH_ALLOCATEDPOINT, 0);

	TempRemainingPoints = Interface->GetRemainStatPoints();
}

void UFBStatPointAllocatorWidget::UpdateAllUI()
{
	UpdateStatDisplay();

	UpdateButtonStates();
}

void UFBStatPointAllocatorWidget::UpdateStatDisplay()
{
	if (TempAttackAllocatedPoint)
	{
		int32 AttackDelta = ChangeDelta.FindRef(FBTAG_CHARACTERSTAT_ATTACKPOWER_ALLOCATEDPOINT);
		if (AttackDelta > 0)
			TempAttackAllocatedPoint->SetText(FText::FromString(FString::Printf(TEXT("+%d"), AttackDelta)));
		else if (AttackDelta < 0)
			TempAttackAllocatedPoint->SetText(FText::FromString(FString::Printf(TEXT("%d"), AttackDelta)));
		else
			TempAttackAllocatedPoint->SetText(FText::FromString(TEXT("")));
	}

	if (TempDefenseAllocatedPoint)
	{
		int32 DefenseDelta = ChangeDelta.FindRef(FBTAG_CHARACTERSTAT_DEFENSE_ALLOCATEDPOINT);
		if (DefenseDelta > 0)
			TempDefenseAllocatedPoint->SetText(FText::FromString(FString::Printf(TEXT("+%d"), DefenseDelta)));
		else if (DefenseDelta < 0)
			TempDefenseAllocatedPoint->SetText(FText::FromString(FString::Printf(TEXT("%d"), DefenseDelta)));
		else
			TempDefenseAllocatedPoint->SetText(FText::FromString(TEXT("")));
	}

	if (TempHealthAllocatedPoint)
	{
		int32 HealthDelta = ChangeDelta.FindRef(FBTAG_CHARACTERSTAT_HEALTH_ALLOCATEDPOINT);
		if (HealthDelta > 0)
			TempHealthAllocatedPoint->SetText(FText::FromString(FString::Printf(TEXT("+%d"), HealthDelta)));
		else if (HealthDelta < 0)
			TempHealthAllocatedPoint->SetText(FText::FromString(FString::Printf(TEXT("%d"), HealthDelta)));
		else
			TempHealthAllocatedPoint->SetText(FText::FromString(TEXT("")));
	}
	if (RemainStatPoint)
	{
		RemainStatPoint->SetText(FText::AsNumber(TempRemainingPoints));
	}
}

void UFBStatPointAllocatorWidget::UpdateButtonStates()
{
	bool bCanAllocate = (TempRemainingPoints > 0);
	if (AttackPowerPointUpButton)
	{
		AttackPowerPointUpButton->SetIsEnabled(bCanAllocate);
	}
	if (DefensePointUpButton)
	{
		DefensePointUpButton->SetIsEnabled(bCanAllocate);
	}
	if (HealthPointUpButton)
	{
		HealthPointUpButton->SetIsEnabled(bCanAllocate);
	}
	
	if (AttackPowerPointDownButton)
	{
		bool bCanDeallocateAttack = CanDeallocatePoint(FBTAG_CHARACTERSTAT_ATTACKPOWER_ALLOCATEDPOINT);
		AttackPowerPointDownButton->SetIsEnabled(bCanDeallocateAttack);
	}
	if (DefensePointDownButton)
	{
		bool bCanDeallocateDefense = CanDeallocatePoint(FBTAG_CHARACTERSTAT_DEFENSE_ALLOCATEDPOINT);
		DefensePointDownButton->SetIsEnabled(bCanDeallocateDefense);
	}
	if (HealthPointDownButton)
	{
		bool bCanDeallocateHealth = CanDeallocatePoint(FBTAG_CHARACTERSTAT_HEALTH_ALLOCATEDPOINT);
		HealthPointDownButton->SetIsEnabled(bCanDeallocateHealth);
	}

	bool bHasChanges = HasPendingChanges();
	if (ApplyButton)
	{
		ApplyButton->SetIsEnabled(bHasChanges);
	}
	if (CancelButton)
	{
		CancelButton->SetIsEnabled(bHasChanges);
	}
}

bool UFBStatPointAllocatorWidget::CanDeallocatePoint(const FGameplayTag& StatTag)
{
	int32 TempPoints = GetFinalAllocatedPoints(StatTag);
	return TempPoints > 0;
}

int32 UFBStatPointAllocatorWidget::GetFinalAllocatedPoints(const FGameplayTag& StatTag)
{
	int32 BasePoints = BaseAllocatedPoints.FindRef(StatTag);
	int32 Delta = ChangeDelta.FindRef(StatTag);
	return BasePoints + Delta;
}

bool UFBStatPointAllocatorWidget::HasPendingChanges() const
{
	for (const auto& [StatTag, Delta] : ChangeDelta)
	{
		if (Delta != 0)
		{
			return true;
		}
	}
	return false;
}

void UFBStatPointAllocatorWidget::OnAttackUpClicked()
{
	AllocatePoint(FBTAG_CHARACTERSTAT_ATTACKPOWER_ALLOCATEDPOINT);
}

void UFBStatPointAllocatorWidget::OnAttackDownClicked()
{
	DeallocatePoint(FBTAG_CHARACTERSTAT_ATTACKPOWER_ALLOCATEDPOINT);
}

void UFBStatPointAllocatorWidget::OnDefenseUpClicked()
{
	AllocatePoint(FBTAG_CHARACTERSTAT_DEFENSE_ALLOCATEDPOINT);
}

void UFBStatPointAllocatorWidget::OnDefenseDownClicked()
{
	DeallocatePoint(FBTAG_CHARACTERSTAT_DEFENSE_ALLOCATEDPOINT);
}

void UFBStatPointAllocatorWidget::OnHealthUpClicked()
{
	AllocatePoint(FBTAG_CHARACTERSTAT_HEALTH_ALLOCATEDPOINT);
}

void UFBStatPointAllocatorWidget::OnHealthDownClicked()
{
	DeallocatePoint(FBTAG_CHARACTERSTAT_HEALTH_ALLOCATEDPOINT);
}

void UFBStatPointAllocatorWidget::AllocatePoint(const FGameplayTag& StatTag)
{
	if (TempRemainingPoints > 0)
	{
		ChangeDelta.FindOrAdd(StatTag)++;
		TempRemainingPoints--;

		UpdateAllUI();
	}
}

void UFBStatPointAllocatorWidget::DeallocatePoint(const FGameplayTag& StatTag)
{
	if (CanDeallocatePoint(StatTag))
	{
		ChangeDelta.FindOrAdd(StatTag)--;
		TempRemainingPoints++;

		UpdateAllUI();
	}
}

void UFBStatPointAllocatorWidget::OnApplyClicked()
{
	ApplyTempDataToSaveGame();
}

void UFBStatPointAllocatorWidget::OnCancelClicked()
{
	InitializeTempData();

	UpdateAllUI();
}

void UFBStatPointAllocatorWidget::ApplyTempDataToSaveGame()
{
	if (!CharacterProfileInterface.IsValid())
	{
		FB_FUNC_LOG(LogTemp, Warning, TEXT("CharacterProfileInterface is invalid"));
		return;
	}

	FFBStatSaveData NewStatSaveData;
	for (const auto& [StatTag, BasePoints] : BaseAllocatedPoints)
	{
		int32 FinalPoints = GetFinalAllocatedPoints(StatTag);
		if (FinalPoints > 0)
		{
			NewStatSaveData.AllocatedLevelUpStats.Add(StatTag, static_cast<uint32>(FinalPoints));
		}
	}
	NewStatSaveData.RemainingLevelUpStatPoints = TempRemainingPoints;
	CharacterProfileInterface.Get()->UpdateStatSaveData(NewStatSaveData);

	for (auto& [StatTag, Delta] : ChangeDelta)
	{
		Delta = 0;
	}

	for (const auto& [StatTag, Points] : NewStatSaveData.AllocatedLevelUpStats)
	{
		BaseAllocatedPoints.FindOrAdd(StatTag) = static_cast<int32>(Points);
	}

	if (UFBSaveGameSubsystem* FBSaveGameSubsystem = GetGameInstance()->GetSubsystem<UFBSaveGameSubsystem>())
	{
		FBSaveGameSubsystem->AsyncSaveGameSlot(SaveData);
	}
	
	UpdateAllUI();
}

void UFBStatPointAllocatorWidget::NativeDestruct()
{

	Super::NativeDestruct();
}