// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/GAInteractionAbility/GA_SavingCurrentPoint.h"
#include "Player/FBPlayerState.h"
#include "System/FBSaveGameSubsystem.h"
#include "SaveGame/Interface/FBSavePointInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "SaveGame/FBSaveGame.h"

UGA_SavingCurrentPoint::UGA_SavingCurrentPoint(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UGA_SavingCurrentPoint::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	SetMovementMode(EMovementMode::MOVE_None);

	// 저장 중 애니메이션 실행할 수도 있음

	AFBPlayerState* PlayerState = Cast<AFBPlayerState>(GetOwnerPlayerState());
	AActor* Avatar = GetAvatarActorFromActorInfo();
	if (PlayerState && Avatar)
	{
		UFBSaveGame* SaveGame = PlayerState->GetSaveData();
		if (IFBSavePointInterface* Interface = Cast<IFBSavePointInterface>(SaveGame))
		{
			Interface->SetSavePoint(Avatar->GetActorLocation(), Avatar->GetActorRotation());

			if (UFBSaveGameSubsystem* FBSaveGameSubsystem = UGameplayStatics::GetGameInstance(PlayerState->GetWorld())->GetSubsystem<UFBSaveGameSubsystem>())
			{
				FBSaveGameSubsystem->OnAsyncSaveGameCompleted.AddUObject(this, &UGA_SavingCurrentPoint::AsyncSaveGameCompleted);
				FBSaveGameSubsystem->AsyncSaveGameSlot(SaveGame);
			}
		}
	}
}

void UGA_SavingCurrentPoint::AsyncSaveGameCompleted(bool bSuccess)
{
	ShowSaveResultWidget(bSuccess);

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
}

void UGA_SavingCurrentPoint::ShowSaveResultWidget(bool bSuccess)
{
	if (APlayerController* PC = GetActorInfo().PlayerController.Get())
	{
		UClass* ResultWidgetClass = bSuccess ? SaveSuccessWidget : SaveFailedWidget;
		if (ResultWidgetClass)
		{
			if (UUserWidget* ResultWidget = CreateWidget<UUserWidget>(PC, ResultWidgetClass))
			{
				ResultWidget->AddToViewport();
			}
		}
	}
}

void UGA_SavingCurrentPoint::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	SetMovementMode(EMovementMode::MOVE_Walking);

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
