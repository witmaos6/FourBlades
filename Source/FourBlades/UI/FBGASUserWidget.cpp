// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/FBGASUserWidget.h"
#include "AbilitySystemBlueprintLibrary.h"

void UFBGASUserWidget::SetAbilitySystemComponent(AActor* InOwner)
{
	if (IsValid(InOwner))
	{
		ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(InOwner);
	}
}

UAbilitySystemComponent* UFBGASUserWidget::GetAbilitySystemComponent() const
{
	return ASC;
}
