// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/CharacterStatus/SaveProgressWidget.h"

USaveProgressWidget::USaveProgressWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	AutoRemoveDelay = 1.0f;
}

void USaveProgressWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(AutoRemoveTimerHandle, this, &USaveProgressWidget::AutoRemoveFromParent, AutoRemoveDelay, false);
	}
}

void USaveProgressWidget::AutoRemoveFromParent()
{
	RemoveFromParent();
}
