// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/CharacterStatus/FBCharacterStatMenu.h"
#include "Player/FBPlayerState.h"
#include "Player/PlayerComponent/FBCharacterStatComponent.h"
#include "Components/TextBlock.h"

UFBCharacterStatMenu::UFBCharacterStatMenu(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	InputConfig = EFBWidgetInputMode::Menu;

	GameMouseCaptureMode = EMouseCaptureMode::CapturePermanently;
}

void UFBCharacterStatMenu::NativeOnInitialized()
{
	Super::NativeOnInitialized();
}

void UFBCharacterStatMenu::NativeConstruct()
{
	Super::NativeConstruct();

	if (AFBPlayerState* PS = Cast<AFBPlayerState>(GetOwningPlayerState()))
	{
		if (UFBCharacterStatComponent* StatComponent = PS->FindComponentByClass<UFBCharacterStatComponent>())
		{
			CharacterStatComponent = StatComponent;

			StatComponent->OnUpdateCharacterStat.AddUObject(this, &UFBCharacterStatMenu::UpdateCharacterStatUI);

			UpdateCharacterStatUI();
		}
	}
}

void UFBCharacterStatMenu::UpdateCharacterStatUI()
{
	if (CharacterStatComponent.IsValid())
	{
		StatTextUpdate(FinalAttackPowerStat, FBTAG_CHARACTERSTAT_ATTACKPOWER);

		StatTextUpdate(FinalDefenseStat, FBTAG_CHARACTERSTAT_DEFENSE);

		StatTextUpdate(FinalMaxHealthStat, FBTAG_CHARACTERSTAT_HEALTH);
	}
}

void UFBCharacterStatMenu::StatTextUpdate(UTextBlock* StatText, const FGameplayTag& StatTag)
{
	if (UFBCharacterStatComponent* StatComponent = CharacterStatComponent.Get())
	{
		const float FinalStat = StatComponent->GetLastTotalStat(StatTag);
		int32 FinalStatToInt = FMath::RoundToInt(FinalStat);
		FString Formatted = FString::Printf(TEXT("%i"), FinalStatToInt);
		StatText->SetText(FText::FromString(Formatted));
	}
}

void UFBCharacterStatMenu::NativeDestruct()
{
	if (CharacterStatComponent.IsValid())
	{
		CharacterStatComponent.Get()->OnUpdateCharacterStat.RemoveAll(this);
	}

	Super::NativeDestruct();
}

