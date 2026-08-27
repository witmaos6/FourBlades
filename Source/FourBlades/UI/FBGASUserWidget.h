// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AbilitySystemInterface.h"
#include "FBGASUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class FOURBLADES_API UFBGASUserWidget : public UUserWidget, public IAbilitySystemInterface
{
	GENERATED_BODY()
protected:
	//virtual void NativeConstruct() override;
public:
	UFUNCTION(BlueprintCallable, Category = "Ability")
	virtual void SetAbilitySystemComponent(AActor* InOwner);

	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override;

protected:
	UPROPERTY(VisibleDefaultsOnly, Category = GAS)
	TObjectPtr<class UAbilitySystemComponent> ASC;
};
