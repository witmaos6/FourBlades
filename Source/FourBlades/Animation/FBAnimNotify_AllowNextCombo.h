// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "FBAnimNotify_AllowNextCombo.generated.h"

DECLARE_DELEGATE(FOnAnimNotifyAllowNextCombo);

/**
 * 
 */
UCLASS()
class FOURBLADES_API UFBAnimNotify_AllowNextCombo : public UAnimNotify
{
	GENERATED_BODY()
public:
	UFBAnimNotify_AllowNextCombo();

protected:
	virtual FString GetNotifyName_Implementation() const override;

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

public:
	FOnAnimNotifyAllowNextCombo OnAnimNotifyAllowNextCombo;
};
