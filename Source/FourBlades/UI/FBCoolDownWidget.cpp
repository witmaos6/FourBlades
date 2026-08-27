// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/FBCoolDownWidget.h"
#include "AbilitySystemComponent.h"
#include "Components/ProgressBar.h"

UFBCoolDownWidget::UFBCoolDownWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	CachedTotalTime = 0.0f;
}

void UFBCoolDownWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (APlayerController* PC = GetOwningPlayer())
	{
		if (APawn* PlayerPawn = PC->GetPawn())
		{
			SetAbilitySystemComponent(PlayerPawn);
		}
		else
		{
			if (GetWorld())
			{
				GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateLambda([this]()
					{
						if (APlayerController* PC = GetOwningPlayer())
						{
							if (APawn* PlayerPawn = PC->GetPawn())
							{
								SetAbilitySystemComponent(PlayerPawn);
							}
						}
					}));
			}
		}
	}
	if (CoolDownProgressBar)
	{
		CoolDownProgressBar->SetPercent(0.0f);
	}
}

void UFBCoolDownWidget::SetAbilitySystemComponent(AActor* InOwner)
{
	Super::SetAbilitySystemComponent(InOwner);

	if (ASC && CoolDownTag.IsValid())
	{
		CoolDownDelegate = ASC->RegisterGameplayTagEvent(CoolDownTag, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &UFBCoolDownWidget::OnCoolDownTagChanged);
	}
}

void UFBCoolDownWidget::OnCoolDownTagChanged(const FGameplayTag Tag, int32 NewCount)
{
	if (NewCount > 0)
	{
		StartCooldownDisplay();
	}
	else
	{
		StopCooldownDisplay();
	}
}

void UFBCoolDownWidget::StartCooldownDisplay()
{
	if (GetOwningPlayer())
	{
		CachedTotalTime = GetCoolDownTotalTime();

		GetOwningPlayer()->GetWorldTimerManager().SetTimer(CooldownUpdateTimer, this, &UFBCoolDownWidget::UpdateCoolDown, 0.1f, true);
	}
}

void UFBCoolDownWidget::UpdateCoolDown()
{
	if (!CoolDownProgressBar || !ASC || CachedTotalTime <= 0.0f)
		return;

	float RemainingTime = GetCooldownRemainingTime();

	if (RemainingTime <= 0.0f)
	{
		StopCooldownDisplay();
		return;
	}

	float Progress = (RemainingTime / CachedTotalTime);
	Progress = FMath::Clamp(Progress, 0.0f, 1.0f);

	CoolDownProgressBar->SetPercent(Progress);
}

void UFBCoolDownWidget::StopCooldownDisplay()
{
	if (GetOwningPlayer())
	{
		GetOwningPlayer()->GetWorldTimerManager().ClearTimer(CooldownUpdateTimer);
	}

	if (CoolDownProgressBar)
	{
		CoolDownProgressBar->SetPercent(0.0f);
	}
}

float UFBCoolDownWidget::GetCoolDownTotalTime() const
{
	if (!ASC || !CoolDownTag.IsValid())
		return -1.0f;

	FGameplayTagContainer CoolDownTags;
	CoolDownTags.AddTag(CoolDownTag);
	FGameplayEffectQuery Query = FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(FGameplayTagContainer(CoolDownTags));

	TArray<FActiveGameplayEffectHandle> ActiveEffects = ASC->GetActiveEffects(Query);
	if (ActiveEffects.Num() > 0)
	{
		const FActiveGameplayEffect* Effect = ASC->GetActiveGameplayEffect(ActiveEffects[0]);
		if (Effect)
		{
			return Effect->GetDuration();
		}
	}
	return -1.0f;
}

float UFBCoolDownWidget::GetCooldownRemainingTime() const
{
	if (!ASC || !CoolDownTag.IsValid())
		return -1.0f;

	FGameplayTagContainer CoolDownTags;
	CoolDownTags.AddTag(CoolDownTag);
	FGameplayEffectQuery Query = FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(FGameplayTagContainer(CoolDownTags));

	TArray<float> Durations = ASC->GetActiveEffectsTimeRemaining(Query);

	if (Durations.Num() > 0)
	{
		return Durations[0];
	}
	return -1.0f;
}

void UFBCoolDownWidget::NativeDestruct()
{
	if (ASC && CoolDownDelegate.IsValid())
	{
		ASC->UnregisterGameplayTagEvent(CoolDownDelegate, CoolDownTag, EGameplayTagEventType::NewOrRemoved);
		CoolDownDelegate.Reset();
	}

	if (GetOwningPlayer() && CooldownUpdateTimer.IsValid())
	{
		GetOwningPlayer()->GetWorldTimerManager().ClearTimer(CooldownUpdateTimer);
	}

	Super::NativeDestruct();
}
