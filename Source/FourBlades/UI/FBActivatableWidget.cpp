// Fill out your copyright notice in the Description page of Project Settings.


#include "FBActivatableWidget.h"

UFBActivatableWidget::UFBActivatableWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

TOptional<FUIInputConfig> UFBActivatableWidget::GetDesiredInputConfig() const
{
	switch (InputConfig)
	{
	case EFBWidgetInputMode::GameAndMenu:
		return FUIInputConfig(ECommonInputMode::All, GameMouseCaptureMode);
		break;
	case EFBWidgetInputMode::Game:
		return FUIInputConfig(ECommonInputMode::Game, GameMouseCaptureMode);
		break;
	case EFBWidgetInputMode::Menu:
		return FUIInputConfig(ECommonInputMode::Menu, GameMouseCaptureMode);
		break;
	case EFBWidgetInputMode::Default:
	default:
		return TOptional<FUIInputConfig>();
		break;
	}
}
