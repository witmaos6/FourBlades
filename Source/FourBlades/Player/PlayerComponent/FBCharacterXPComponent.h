// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/PlayerStateComponent.h"
#include "FBCharacterXPComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnCharacterXPUpdate, const uint32 InOldXP, const uint32 InNewXP, const float NormalizedXP);

/**
 * 
 */
UCLASS()
class FOURBLADES_API UFBCharacterXPComponent : public UPlayerStateComponent
{
	GENERATED_BODY()
public:
	UFBCharacterXPComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void OnRegister() override;

private:
	TWeakInterfacePtr<class IFBCharacterProfileInterface> ProfileInterface;

public:
	FOnCharacterXPUpdate OnCharacterXPUpdate;

	const uint32 GetCharacterXP() const;

	uint32 GetCharacterXPRequiredForNextLevel() const;

	float GetNormalizedXP() const;

	void CharacterXPUpdate(const uint32 InOldXP, const uint32 InNewXP);
};
