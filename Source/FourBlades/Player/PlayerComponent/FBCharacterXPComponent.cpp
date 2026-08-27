// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/PlayerComponent/FBCharacterXPComponent.h"
#include "Player/FBPlayerState.h"
#include "SaveGame/Interface/FBCharacterProfileInterface.h"
#include "SaveGame/FBSaveGame.h"

UFBCharacterXPComponent::UFBCharacterXPComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UFBCharacterXPComponent::OnRegister()
{
	Super::OnRegister();

	AFBPlayerState* Owner = Cast<AFBPlayerState>(GetOwner());
	if (Owner)
	{
		if (UFBSaveGame* OwnerSaveData = Owner->GetSaveData())
		{
			if (IFBCharacterProfileInterface* CharacterProfile = Cast<IFBCharacterProfileInterface>(OwnerSaveData))
			{
				ProfileInterface = CharacterProfile;

				CharacterProfile->GetXPChangeDelegate().AddUObject(this, &UFBCharacterXPComponent::CharacterXPUpdate);
			}
		}
	}
}

const uint32 UFBCharacterXPComponent::GetCharacterXP() const
{
	if (ProfileInterface.IsValid())
	{
		return ProfileInterface.Get()->GetCurrentXP();
	}
	return -1;
}

uint32 UFBCharacterXPComponent::GetCharacterXPRequiredForNextLevel() const
{
	if (ProfileInterface.IsValid())
	{
		return ProfileInterface.Get()->GetXPRequiredForNextLevel();
	}
	return -1;
}

float UFBCharacterXPComponent::GetNormalizedXP() const
{
	if (ProfileInterface.IsValid())
	{
		float CurrentXP = static_cast<float>(ProfileInterface.Get()->GetCurrentXP());
		float RequiredForNextLevel = static_cast<float>(ProfileInterface.Get()->GetXPRequiredForNextLevel());

		return CurrentXP / RequiredForNextLevel;
	}
	return -1.0f;
}

void UFBCharacterXPComponent::CharacterXPUpdate(const uint32 InOldXP, const uint32 InNewXP)
{
	if (OnCharacterXPUpdate.IsBound())
	{
		OnCharacterXPUpdate.Broadcast(InOldXP, InNewXP, GetNormalizedXP());
	}
}
