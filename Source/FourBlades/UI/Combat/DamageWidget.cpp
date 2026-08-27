// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Combat/DamageWidget.h"
#include "Components/TextBlock.h"
#include "Kismet/KismetMathLibrary.h"

UDamageWidget::UDamageWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	AutoRemoveDelay = 1.0f;
}

void UDamageWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(AutoRemoveTimerHandle, this, &UDamageWidget::AutoRemoveFromParent, AutoRemoveDelay, false);
	}
}

void UDamageWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	SetPositionInViewport(Position2D);
}

void UDamageWidget::AutoRemoveFromParent()
{
	RemoveFromParent();
}

void UDamageWidget::SetDamageWidget(float ApplyDamage)
{
	if (DamageText)
	{
		DamageText->SetText(FText::AsNumber(ApplyDamage));
	}
}
