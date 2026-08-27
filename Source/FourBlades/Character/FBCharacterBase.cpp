// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/FBCharacterBase.h"
#include "PawnComponent/FBPawnExtensionComponent.h"
#include "Camera/FBCameraComponent.h"

AFBCharacterBase::AFBCharacterBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UFBCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;
}
