// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SaveProgressWidget.generated.h"

/**
 * 
 */
UCLASS()
class FOURBLADES_API USaveProgressWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	USaveProgressWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Remove")
	float AutoRemoveDelay;

	FTimerHandle AutoRemoveTimerHandle;

	UFUNCTION()
	void AutoRemoveFromParent();
};
