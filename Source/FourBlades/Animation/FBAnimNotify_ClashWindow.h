// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "FBAnimNotify_ClashWindow.generated.h"

DECLARE_DELEGATE(FOnAnimNotifyClashWindow);

/**
 * 
 */
UCLASS()
class FOURBLADES_API UFBAnimNotify_ClashWindow : public UAnimNotify
{
	GENERATED_BODY()
public:
	UFBAnimNotify_ClashWindow();

protected:
	virtual FString GetNotifyName_Implementation() const override;

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

public:
	FOnAnimNotifyClashWindow OnAnimNotifyClashWindow;
};
