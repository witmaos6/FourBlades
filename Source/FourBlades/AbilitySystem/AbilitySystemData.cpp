// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AbilitySystemData.h"
#include "FourBlades.h"

UAbilitySystemData::UAbilitySystemData(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	SlashComboIndex = 0;
}

void UAbilitySystemData::StartContinuousComboTimeout(float ComboTimeoutDuration)
{
	CancelContinuousComboTimeout();

	GetWorld()->GetTimerManager().SetTimer(ContinuousComboTimeoutHandle, this, &UAbilitySystemData::ResetContinuousCombo, ComboTimeoutDuration, false);
}

void UAbilitySystemData::CancelContinuousComboTimeout()
{
    if (!ContinuousComboTimeoutHandle.IsValid())
    {
        return;
    }

    UWorld* World = GetWorld();
    if (World && IsValid(World))
    {
        if (World->GetTimerManager().IsTimerActive(ContinuousComboTimeoutHandle))
        {
            World->GetTimerManager().ClearTimer(ContinuousComboTimeoutHandle);
        }
    }
    else
    {
        FB_FUNC_LOG(LogTemp, Warning, TEXT("CancelTimer: World is nullptr or invalid"));
    }

    ContinuousComboTimeoutHandle.Invalidate();
}

void UAbilitySystemData::ResetContinuousCombo()
{
    UWorld* World = GetWorld();
    if (IsValid(World) && ContinuousComboTimeoutHandle.IsValid() && World->GetTimerManager().IsTimerActive(ContinuousComboTimeoutHandle))
    {
        World->GetTimerManager().ClearTimer(ContinuousComboTimeoutHandle);
    }
    
    ContinuousCombo.PrevDirection = EContinuousDirection::NONE;
    ContinuousCombo.LeftComboIndex = 0;
    ContinuousCombo.RightComboIndex = 0;
}

void UAbilitySystemData::BeginDestroy()
{
	CancelContinuousComboTimeout();

	Super::BeginDestroy();
}
