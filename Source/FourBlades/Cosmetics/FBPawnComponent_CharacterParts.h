// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FBCharacterPartsTypes.h"
#include "FBCosmeticAnimationTypes.h"
#include "Components/PawnComponent.h"
#include "FBPawnComponent_CharacterParts.generated.h"

class UFBPawnComponent_CharacterParts;

/*
* AppliedCharacterPartEntry
*/
USTRUCT()
struct FFBAppliedCharacterPartEntry
{
	GENERATED_BODY()

	UPROPERTY()
	FFBCharacterPart Part;

	UPROPERTY()
	int32 PartHandle = INDEX_NONE;

	UPROPERTY()
	TObjectPtr<UChildActorComponent> SpawnedComponent = nullptr;
};

/*
* CharacterPartList
*/
USTRUCT()
struct FFBCharacterPartList
{
	GENERATED_BODY()
	
	FFBCharacterPartList() : OwnerComponent(nullptr) {}

	FFBCharacterPartList(UFBPawnComponent_CharacterParts* InOwnerComponent) : OwnerComponent(InOwnerComponent) {}

	FFBCharacterPartHandle AddEntry(FFBCharacterPart NewPart);
	bool SpawnActorForEntry(FFBAppliedCharacterPartEntry& Entry);

	void RemoveEntry(FFBCharacterPartHandle Handle);
	void DestroyActorForEntry(FFBAppliedCharacterPartEntry& Entry);

	FGameplayTagContainer CollectCombinedTags() const;

	UPROPERTY()
	TArray<FFBAppliedCharacterPartEntry> Entries;

	UPROPERTY()
	TObjectPtr<UFBPawnComponent_CharacterParts> OwnerComponent;

	int32 PartHandleCounter = 0;
};

/*
* PawnComponent_CharacterParts
*/
UCLASS()
class FOURBLADES_API UFBPawnComponent_CharacterParts : public UPawnComponent
{
	GENERATED_BODY()
public:
	UFBPawnComponent_CharacterParts(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	FFBCharacterPartHandle AddCharacterPart(const FFBCharacterPart& NewPart);

	void RemoveCharacterPart(FFBCharacterPartHandle Handle);

	void BroadcastChanged();

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = Cosmetics)
	FGameplayTagContainer GetCombinedTag(FGameplayTag RequiredPrefix) const;

	USkeletalMeshComponent* GetParentMeshComponent() const;

	USceneComponent* GetSceneComponentToAttachTo() const;
	
	UPROPERTY()
	FFBCharacterPartList CharacterPartList;

	UPROPERTY(EditAnywhere, Category = Cosmetic)
	FFBAnimBodyStyleSelectionSet BodyMeshes;
};
