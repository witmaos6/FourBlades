// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/FBActivatableWidget.h"
#include "FBDeadWidget.generated.h"

/**
 * 
 */
UCLASS()
class FOURBLADES_API UFBDeadWidget : public UFBActivatableWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;

public:
	UPROPERTY(BlueprintReadOnly, Category = "ResurrectionButton", meta = (BindWidget))
	TObjectPtr<class UButton> ResurrectionButton;

	UFUNCTION()
	void OnReviveButtonClicked();

protected:
	virtual void NativeDestruct() override;
};
