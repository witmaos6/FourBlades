// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FBCharacterPartsTypes.h"
#include "Components/ControllerComponent.h"
#include "FBControllerComponent_CharacterParts.generated.h"

/**
 * 
 */

USTRUCT()
struct FFBControllerCharacterPartEntry
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	FFBCharacterPart Part;

	FFBCharacterPartHandle Handle;
};

UCLASS(meta = (BlueprintSpawnableComponent))
class FOURBLADES_API UFBControllerComponent_CharacterParts : public UControllerComponent
{
	GENERATED_BODY()
public:
	UFBControllerComponent_CharacterParts(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void BeginPlay() override;

	UFUNCTION()
	void OnPossessedPawnChanged(APawn* OldPawn, APawn* NewPawn);

	class UFBPawnComponent_CharacterParts* GetPawnCustomizer() const;

	UFUNCTION(BlueprintCallable, Category = Cosmetics)
	void AddCharacterPart(const FFBCharacterPart& NewPart); // Dedicated를 위한 함수

	void AddCharacterPartInternal(const FFBCharacterPart& NewPart);

	UPROPERTY(EditAnywhere, Category = Cosmetics)
	TArray<FFBControllerCharacterPartEntry> CharacterParts;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	void RemoveAllCharacterParts();
};
