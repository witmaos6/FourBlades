// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "FBGASWidgetComponent.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, Meta = (BlueprintSpawnableComponent))
class FOURBLADES_API UFBGASWidgetComponent : public UWidgetComponent
{
	GENERATED_BODY()
public:
	UFBGASWidgetComponent();
protected:
	virtual void InitWidget() override;

	UPROPERTY(EditDefaultsOnly, Category = "Render")
	float CullDistanceSize;
};
