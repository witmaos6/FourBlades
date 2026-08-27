// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/GAMenuAbility/GADeadMenuOpen.h"
#include "UI/System/FBDeadWidget.h"
#include "Components/Button.h"

UGADeadMenuOpen::UGADeadMenuOpen(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UGADeadMenuOpen::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (Widget)
	{
		if (UFBDeadWidget* DeadWidget = Cast<UFBDeadWidget>(Widget))
		{
			if (DeadWidget->ResurrectionButton)
			{
				DeadWidget->ResurrectionButton->OnClicked.AddDynamic(this, &UGADeadMenuOpen::CallEndAbility);
			}
		}	
	}
}

void UGADeadMenuOpen::CallEndAbility()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
}
