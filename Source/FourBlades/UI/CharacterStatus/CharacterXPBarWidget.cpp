// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/CharacterStatus/CharacterXPBarWidget.h"
#include "Player/PlayerComponent/FBCharacterXPComponent.h"
#include "Components/ProgressBar.h"
#include "GameModes/FBExperienceManagerComponent.h"
#include "FourBlades.h"

void UCharacterXPBarWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (!TryInitializeXPComponent())
	{
		RetryCount = 0;
		MaxRetries = 10;
		GetWorld()->GetTimerManager().SetTimer(InitializationTimer, this, &UCharacterXPBarWidget::RetryInitialization, 0.1, true);
	}
}

bool UCharacterXPBarWidget::TryInitializeXPComponent()
{
	if (APlayerState* PS = GetOwningPlayerState())
	{
		if (UFBCharacterXPComponent* CharacterXPComponent = PS->FindComponentByClass<UFBCharacterXPComponent>())
		{
			CharacterXPComponent->OnCharacterXPUpdate.AddUObject(this, &UCharacterXPBarWidget::CharacterXPUpdate);

			uint32 InitXP = CharacterXPComponent->GetCharacterXP();
			CharacterXPUpdate(InitXP, InitXP, CharacterXPComponent->GetNormalizedXP());

			return true;
		}
	}
	return false;
}

void UCharacterXPBarWidget::RetryInitialization()
{
	RetryCount++;

	if (TryInitializeXPComponent() || RetryCount >= MaxRetries)
	{
		GetWorld()->GetTimerManager().ClearTimer(InitializationTimer);

		if (RetryCount >= MaxRetries)
		{
			FB_FUNC_LOG(LogTemp, Warning, TEXT("Failed to Initialize XPComponent"));
		}
	}
}

void UCharacterXPBarWidget::CharacterXPUpdate(const uint32 InOldXP, const uint32 InNewXP, const float NormalizedXP)
{
	if (ProgressBar)
	{
		ProgressBar->SetPercent(NormalizedXP);
	}
}

void UCharacterXPBarWidget::NativeDestruct()
{
	if (APlayerState* PS = GetOwningPlayerState())
	{
		if (UFBCharacterXPComponent* CharacterXPComponent = PS->FindComponentByClass<UFBCharacterXPComponent>())
		{
			CharacterXPComponent->OnCharacterXPUpdate.RemoveAll(this);
		}
	}
	Super::NativeDestruct();
}
