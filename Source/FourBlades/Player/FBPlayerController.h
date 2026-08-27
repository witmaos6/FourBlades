// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonPlayerController.h"
#include "FBPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class FOURBLADES_API AFBPlayerController : public ACommonPlayerController
{
	GENERATED_BODY()
public:
	AFBPlayerController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void BeginPlay() override;

public:
	virtual void PostProcessInput(const float DeltaTime, const bool bGamePaused) override;

	class AFBPlayerState* GetFBPlayerState() const;

	class UFBAbilitySystemComponent* GetFBAbilitySystemComponent() const;
};
