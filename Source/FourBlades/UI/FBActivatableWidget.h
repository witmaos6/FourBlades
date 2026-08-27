// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "FBActivatableWidget.generated.h"

UENUM(BlueprintType)
enum class EFBWidgetInputMode : uint8
{
	Default,
	GameAndMenu,
	Game,
	Menu,
};

/**
 * 
 */
UCLASS()
class FOURBLADES_API UFBActivatableWidget : public UCommonActivatableWidget
{
	GENERATED_BODY()
public:
	UFBActivatableWidget(const FObjectInitializer& ObjectInitializer);

	virtual TOptional<FUIInputConfig> GetDesiredInputConfig() const override;

	UPROPERTY(EditDefaultsOnly, Category = Input)
	EFBWidgetInputMode InputConfig = EFBWidgetInputMode::Default;

	UPROPERTY(EditDefaultsOnly, Category = Input)
	EMouseCaptureMode GameMouseCaptureMode = EMouseCaptureMode::CapturePermanently;
};
