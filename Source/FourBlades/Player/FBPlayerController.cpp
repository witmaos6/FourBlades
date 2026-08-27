// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/FBPlayerController.h"
#include "Camera/FBPlayerCameraManager.h"
#include "FBPlayerState.h"
#include "AbilitySystem/FBAbilitySystemComponent.h"
#include "AbilitySystem/Attributes/FBHealthSet.h"

AFBPlayerController::AFBPlayerController(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PlayerCameraManagerClass = AFBPlayerCameraManager::StaticClass();
}

void AFBPlayerController::BeginPlay()
{
	Super::BeginPlay();
}

void AFBPlayerController::PostProcessInput(const float DeltaTime, const bool bGamePaused)
{
	if (UFBAbilitySystemComponent* ASC = GetFBAbilitySystemComponent())
	{
		ASC->ProcessAbilityInput(DeltaTime, bGamePaused);
	}

	Super::PostProcessInput(DeltaTime, bGamePaused);
}

AFBPlayerState* AFBPlayerController::GetFBPlayerState() const
{
	return CastChecked<AFBPlayerState>(PlayerState, ECastCheckedType::NullAllowed);
}

UFBAbilitySystemComponent* AFBPlayerController::GetFBAbilitySystemComponent() const
{
	const AFBPlayerState* PS = GetFBPlayerState();
	return (PS ? PS->GetFBAbilitySystemComponent() : nullptr);
}