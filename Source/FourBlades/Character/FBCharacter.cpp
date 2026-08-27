// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/FBCharacter.h"
#include "PawnComponent/FBPawnExtensionComponent.h"
#include "Camera/FBCameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AbilitySystem/FBAbilitySystemComponent.h"
#include "PawnComponent/FBHealthComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameModes/FBGameState.h"
#include "AbilitySystem/Attributes/FBHealthSet.h"
#include "Kismet/GameplayStatics.h"
#include "GameModes/FBGameModeBase.h"
#include "Player/FBPlayerState.h"
#include "FourBlades.h"

AFBCharacter::AFBCharacter()
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	PawnExtComponent = CreateDefaultSubobject<UFBPawnExtensionComponent>(TEXT("PawnExtensionComponent"));
	PawnExtComponent->OnAbilitySystemInitialized_RegisterAndCall(FSimpleMulticastDelegate::FDelegate::CreateUObject(this, &ThisClass::OnAbilitySystemInitialized));
	PawnExtComponent->OnAbilitySystemUninitialized_Register(FSimpleMulticastDelegate::FDelegate::CreateUObject(this, &ThisClass::OnAbilitySystemUninitialized));

	CameraComponent = CreateDefaultSubobject<UFBCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetRelativeLocation(FVector(-300.f, 0.0f, 75.f));

	GetCharacterMovement()->bOrientRotationToMovement = true;

	bUseControllerRotationYaw = false;

	HealthComponent = CreateDefaultSubobject<UFBHealthComponent>(TEXT("HealthComponent"));

	GetCapsuleComponent()->SetCollisionProfileName(TEXT("FBPlayerCapsule"));
}

void AFBCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (AFBGameModeBase* GameMode = Cast<AFBGameModeBase>(UGameplayStatics::GetGameMode(this)))
	{
		GameMode->OnPlayerResurrectionDelegate.AddUObject(this, &AFBCharacter::Resurrection);
	}
}

void AFBCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AFBCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PawnExtComponent->SetupPlayerInputComponent();
}

void AFBCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	AFBGameState* GameState = Cast<AFBGameState>(GetWorld()->GetGameState());
	if (GameState)
	{
		GameState->PlayerPossessedFinish(this);
	}

	if (AFBPlayerState* PS = Cast<AFBPlayerState>(GetPlayerState()))
	{
		PS->InitSavePoint(this);
	}
}

UAbilitySystemComponent* AFBCharacter::GetAbilitySystemComponent() const
{
	return PawnExtComponent->GetFBAbilitySystemComponent();
}

void AFBCharacter::OnAbilitySystemInitialized()
{
	UFBAbilitySystemComponent* ASC = Cast<UFBAbilitySystemComponent>(GetAbilitySystemComponent());
	check(ASC);

	HealthComponent->InitializeWithAbilitySystem(ASC);

	if (const UFBHealthSet* HealthSet = ASC->GetSetChecked<UFBHealthSet>())
	{
		HealthSet->OnOutOfHealth.AddUObject(this, &AFBCharacter::SetDead);
	}
}

void AFBCharacter::OnAbilitySystemUninitialized()
{
	HealthComponent->UninitializeWithAbilitySystem();
}

void AFBCharacter::SetDead()
{
	GetCharacterMovement()->SetMovementMode(MOVE_None);

	PlayAnimMontage(DeadMontage);
}

void AFBCharacter::Resurrection()
{
	GetCharacterMovement()->SetMovementMode(MOVE_Walking);

	if (GetMesh()->GetAnimInstance()->Montage_IsPlaying(DeadMontage))
	{
		StopAnimMontage(DeadMontage);
	}

	UFBAbilitySystemComponent* ASC = Cast<UFBAbilitySystemComponent>(GetAbilitySystemComponent());
	if (const UFBHealthSet* HealthSet = ASC->GetSetChecked<UFBHealthSet>())
	{
		HealthSet->OnOutOfHealth.AddUObject(this, &AFBCharacter::SetDead);
	}
}