// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Health/FBGASHealthBarUserWidget.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/Attributes/FBHealthSet.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UFBGASHealthBarUserWidget::SetAbilitySystemComponent(AActor* InOwner)
{
	Super::SetAbilitySystemComponent(InOwner);

	if (ASC)
	{
		ASC->GetGameplayAttributeValueChangeDelegate(UFBHealthSet::GetHealthAttribute()).AddUObject(this, &UFBGASHealthBarUserWidget::OnHealthChanged);
		ASC->GetGameplayAttributeValueChangeDelegate(UFBHealthSet::GetMaxHealthAttribute()).AddUObject(this, &UFBGASHealthBarUserWidget::OnMaxHealthChanged);

		const UFBHealthSet* CurrentHealthSet = ASC->GetSet<UFBHealthSet>();
		ensure(CurrentHealthSet);

		if (CurrentHealthSet)
		{
			CurrentHealth = CurrentHealthSet->GetHealth();
			CurrentMaxHealth = CurrentHealthSet->GetMaxHealth();
			
			if (CurrentMaxHealth > 0.0f)
			{
				UpdateHealthBar();
			}
		}
	}
}

void UFBGASHealthBarUserWidget::OnHealthChanged(const FOnAttributeChangeData& ChangeData)
{
	CurrentHealth = ChangeData.NewValue;
	UpdateHealthBar();
}

void UFBGASHealthBarUserWidget::OnMaxHealthChanged(const FOnAttributeChangeData& ChangeData)
{
	CurrentMaxHealth = ChangeData.NewValue;
	UpdateHealthBar();
}

void UFBGASHealthBarUserWidget::UpdateHealthBar()
{
	if (HealthBar)
	{
		HealthBar->SetPercent(CurrentHealth / CurrentMaxHealth);
	}

	if (TextHealth)
	{
		TextHealth->SetText(FText::FromString(FString::Printf(TEXT("%.0f/%0.f"), CurrentHealth, CurrentMaxHealth)));
	}
}