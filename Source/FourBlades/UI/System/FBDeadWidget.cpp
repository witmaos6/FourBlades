// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/System/FBDeadWidget.h"
#include "Components/Button.h"
#include "GameModes/FBGameModeBase.h"
#include "Kismet/GameplayStatics.h"

void UFBDeadWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ResurrectionButton->OnClicked.AddDynamic(this, &UFBDeadWidget::OnReviveButtonClicked);
}

void UFBDeadWidget::OnReviveButtonClicked()
{
	if (APlayerController* PC = GetOwningPlayer())
	{
		AFBGameModeBase* GameMode = Cast<AFBGameModeBase>(UGameplayStatics::GetGameMode(PC));
		if (GameMode)
		{
			GameMode->BeginResurrection();
		}
	}
}

void UFBDeadWidget::NativeDestruct()
{
	ResurrectionButton->OnClicked.Clear();

	Super::NativeDestruct();
}
