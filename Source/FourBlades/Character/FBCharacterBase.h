// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ModularCharacter.h"
#include "CharacterComponents/FBCharacterMovementComponent.h"
#include "FBCharacterBase.generated.h"

UCLASS()
class FOURBLADES_API AFBCharacterBase : public AModularCharacter
{
	GENERATED_BODY()

public:
	AFBCharacterBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	FORCEINLINE UFBCharacterMovementComponent* GetFBCharacterMovement() const
	{
		return Cast<UFBCharacterMovementComponent>(GetCharacterMovement());
	}
};
