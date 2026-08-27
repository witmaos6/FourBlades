// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "UObject/ObjectPtr.h"
#include "UObject/UObjectGlobals.h"
#include "FBHealthUserInterface.generated.h"

/**
 * 
 */
UCLASS()
class FOURBLADES_API UFBHealthUserInterface : public UCommonUserWidget
{
	GENERATED_BODY()
public:
	UFBHealthUserInterface(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	/*UFUNCTION()
	void OnHealthChagned();*/
};
