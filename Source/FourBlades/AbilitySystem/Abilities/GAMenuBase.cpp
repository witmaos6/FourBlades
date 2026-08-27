// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/GAMenuBase.h"
#include "UI/FBActivatableWidget.h"
#include "CommonGame/Public/CommonUIExtensions.h"

UGAMenuBase::UGAMenuBase(const FObjectInitializer& ObjectInitializer)
{
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalOnly;
	// To do: 입력 정책 정하기 EX: 누르고 있는 동안 UI노출, On/Off 방식 등등
}

void UGAMenuBase::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	APlayerController* PC = Cast<APlayerController>(ActorInfo->PlayerController.Get());
	if (PC == nullptr)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, false, true);
		return;
	}

	if (!WidgetClass.IsNull())
	{
		UClass* LoadWidgetClass = WidgetClass.LoadSynchronous();
		if (LoadWidgetClass && LayerTag.IsValid())
		{
			Widget = UCommonUIExtensions::PushContentToLayer_ForPlayer(PC->GetLocalPlayer(), LayerTag, LoadWidgetClass);
		}

		if (Widget)
		{
			Widget->OnDeactivated().AddUObject(this, &UGAMenuBase::OnWidgetDeactivated);

			FInputModeGameAndUI InputMode;
			InputMode.SetWidgetToFocus(Widget->GetCachedWidget());
			InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

			PC->SetInputMode(InputMode);
			PC->SetShowMouseCursor(true);
		}
	}
}

void UGAMenuBase::InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	if (Widget && Widget->IsActivated())
	{
		Widget->DeactivateWidget();
		Widget = nullptr;
		EndAbility(Handle, ActorInfo, ActivationInfo, false, false);
	}
}

void UGAMenuBase::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (Widget && Widget->IsActivated())
	{
		Widget->DeactivateWidget();
	}
	Widget = nullptr;

	APlayerController* PC = Cast<APlayerController>(ActorInfo->PlayerController.Get());
	if (PC)
	{
		FInputModeGameOnly InputMode;
		InputMode.SetConsumeCaptureMouseDown(false);

		PC->SetInputMode(InputMode);
		PC->SetShowMouseCursor(false);
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGAMenuBase::OnWidgetDeactivated()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
}
