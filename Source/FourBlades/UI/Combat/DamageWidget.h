// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DamageWidget.generated.h"

/**
 * 
 */
UCLASS()
class FOURBLADES_API UDamageWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UDamageWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Remove")
	float AutoRemoveDelay;

	FTimerHandle AutoRemoveTimerHandle;

public:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UFUNCTION()
	void AutoRemoveFromParent();

protected:
	FVector2D Position2D;

public:
	void SetPosition2D(FVector2D InPosition) { Position2D = InPosition; }

	void SetDamageWidget(float ApplyDamage);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> DamageText;
};
