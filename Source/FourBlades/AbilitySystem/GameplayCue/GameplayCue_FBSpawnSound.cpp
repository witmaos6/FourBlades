// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/GameplayCue/GameplayCue_FBSpawnSound.h"
#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"

UGameplayCue_FBSpawnSound::UGameplayCue_FBSpawnSound()
{
}

bool UGameplayCue_FBSpawnSound::OnExecute_Implementation(AActor* Target, const FGameplayCueParameters& Parameters) const
{
	if (Sound)
	{
		if (SoundAttenuation)
		{
			UGameplayStatics::PlaySoundAtLocation(Target, Sound, Target->GetActorLocation(), 1.0f, 1.0f, 0.0f, SoundAttenuation);
		}
		else
		{
			UGameplayStatics::PlaySoundAtLocation(Target, Sound, Target->GetActorLocation());
		}
		return true;
	}
	return false;
}
