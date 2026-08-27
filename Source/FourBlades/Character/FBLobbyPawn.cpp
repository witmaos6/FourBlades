// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/FBLobbyPawn.h"
#include "PawnComponent/FBPawnExtensionComponent.h"
#include "Camera/FBCameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "AbilitySystem/FBAbilitySystemComponent.h"

AFBLobbyPawn::AFBLobbyPawn()
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	PawnExtComponent = CreateDefaultSubobject<UFBPawnExtensionComponent>(TEXT("PawnExtensionComponent"));

	CameraComponent = CreateDefaultSubobject<UFBCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetRelativeLocation(FVector(-300.f, 0.0f, 75.f));

	GetCharacterMovement()->bOrientRotationToMovement = true;

	bUseControllerRotationYaw = false;

	GetCapsuleComponent()->SetCollisionProfileName(TEXT("FBPlayerCapsule"));
}

void AFBLobbyPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PawnExtComponent->SetupPlayerInputComponent();
}

UAbilitySystemComponent* AFBLobbyPawn::GetAbilitySystemComponent() const
{
	return PawnExtComponent->GetFBAbilitySystemComponent();
}

void AFBLobbyPawn::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponent())
	{
		ASC->CancelAllAbilities();
	}

	Super::EndPlay(EndPlayReason);
}
