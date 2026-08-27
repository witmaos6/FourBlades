// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayCueNotify_Static.h"
#include "GameplayCue_FBSpawnSound.generated.h"

/**
 * 
 */
UCLASS()
class FOURBLADES_API UGameplayCue_FBSpawnSound : public UGameplayCueNotify_Static
{
	GENERATED_BODY()
public:
	UGameplayCue_FBSpawnSound();

	virtual bool OnExecute_Implementation(AActor* Target, const FGameplayCueParameters& Parameters) const override;

	// To do: 추가 적인 로직을 실행할 수 있게 K2_OnExecute같은 함수를 만들 수 있음

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SoundAttenuation")
	TObjectPtr<USoundAttenuation> SoundAttenuation;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SoundAttenuation")
	TObjectPtr<class USoundCue> Sound;
};
