// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/HUD.h"
#include "FBHUD.generated.h"

/**
 * 
 */
UCLASS()
class FOURBLADES_API AFBHUD : public AHUD
{
	GENERATED_BODY()
public:
	AFBHUD(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void PreInitializeComponents() override;

	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
};
