// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "CommonGameInstance.h"
#include "FBGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class FOURBLADES_API UFBGameInstance : public UCommonGameInstance
{
	GENERATED_BODY()
	
	virtual void Init() override;

	virtual void Shutdown() override;
};
