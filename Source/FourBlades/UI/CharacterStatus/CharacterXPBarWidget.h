// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CharacterXPBarWidget.generated.h"

/**
 * 
 */
UCLASS()
class FOURBLADES_API UCharacterXPBarWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;

	bool TryInitializeXPComponent();

	void RetryInitialization();

	void CharacterXPUpdate(const uint32 InOldXP, const uint32 InNewXP, const float NormalizedXP);

protected:
	FTimerHandle InitializationTimer;

private:
	uint8 RetryCount;
	
	uint8 MaxRetries;

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UProgressBar> ProgressBar;

protected:
	virtual void NativeDestruct() override;
};
