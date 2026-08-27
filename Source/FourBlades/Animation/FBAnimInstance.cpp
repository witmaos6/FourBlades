// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/FBAnimInstance.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AbilitySystemGlobals.h"
#include "AbilitySystemComponent.h"

UFBAnimInstance::UFBAnimInstance()
{
	MovingThreshould = 3.0f;

	JumpingThreshould = 100.f;
}

void UFBAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	Owner = Cast<ACharacter>(GetOwningActor());
	if (Owner)
	{
		Movement = Owner->GetCharacterMovement();

		if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Owner))
		{
			InitializeWithAbilitySystem(ASC);
		}
	}
}

void UFBAnimInstance::InitializeWithAbilitySystem(UAbilitySystemComponent* ASC)
{
	GameplayTagPropertyMap.Initialize(this, ASC);
}

void UFBAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (Movement)
	{
		Velocity = Movement->Velocity;
		GroundSpeed = Velocity.Size2D();
		bIsIdle = GroundSpeed < MovingThreshould;
		bIsFalling = Movement->IsFalling();
		bIsJumping = (bIsFalling & (Velocity.Z > JumpingThreshould));
		
		bIsContinuousMode = CheckContinuousMode();
	}
}

bool UFBAnimInstance::CheckContinuousMode()
{
	if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Owner))
	{
		const FGameplayTagContainer& TagContainer = ASC->GetOwnedGameplayTags();
		return TagContainer.HasTagExact(FGameplayTag::RequestGameplayTag(FName("Character.Ability.ContinuousAttack")));
	}
	return false;
}
