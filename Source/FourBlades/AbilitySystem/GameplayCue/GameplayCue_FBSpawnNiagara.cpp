// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/GameplayCue/GameplayCue_FBSpawnNiagara.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraDataInterfaceArrayFunctionLibrary.h"

UGameplayCue_FBSpawnNiagara::UGameplayCue_FBSpawnNiagara()
{
}

bool UGameplayCue_FBSpawnNiagara::OnExecute_Implementation(AActor* Target, const FGameplayCueParameters& Parameters) const
{
	if (NiagaraSystem)
	{
		for (const auto& TargetActor : Parameters.EffectContext.Get()->GetActors())
		{
			if (TargetActor.Get())
			{
				FVector SpawnLocation = TargetActor.Get()->GetActorLocation();
				UNiagaraFunctionLibrary::SpawnSystemAtLocation(Target, NiagaraSystem, SpawnLocation);
				return true;
			}
		}
	}

	return false;
}
