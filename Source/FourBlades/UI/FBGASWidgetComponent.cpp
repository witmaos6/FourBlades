// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/FBGASWidgetComponent.h"
#include "UI/FBGASUserWidget.h"

UFBGASWidgetComponent::UFBGASWidgetComponent()
{
	CullDistanceSize = 0.0f;
}

void UFBGASWidgetComponent::InitWidget()
{
	Super::InitWidget();

	UFBGASUserWidget* GASUserWidget = Cast<UFBGASUserWidget>(GetWidget());
	if (GASUserWidget)
	{
		GASUserWidget->SetAbilitySystemComponent(GetOwner());
	}
}
