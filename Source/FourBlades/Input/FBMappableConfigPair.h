// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputMappingContext.h" 
#include "FBMappableConfigPair.generated.h"

UENUM(BlueprintType)
enum class EIMCActivationType : uint8
{
	AlwaysActive,      // 항상 활성
	Default,           // 기본적으로 활성
	Manual             // 수동 전환 필요
};

USTRUCT()
struct FFBMappableConfigPair
{
	GENERATED_BODY()
public:
	FFBMappableConfigPair() = default;

    UPROPERTY(EditAnywhere, Category = "Input")
    TSoftObjectPtr<class UInputMappingContext> InputMappingContext;

    UPROPERTY(EditAnywhere, Category = "Input")
    int32 Priority = 0;

    UPROPERTY(EditAnywhere, Category = "Input")
    EIMCActivationType ActivationType = EIMCActivationType::AlwaysActive;

    UPROPERTY(EditAnywhere, Category = "Input")
    FName ConfigName;
};
