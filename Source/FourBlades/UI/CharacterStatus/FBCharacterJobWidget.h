// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "SaveGame/FBSaveGameTypes.h"
#include "FBCharacterJobWidget.generated.h"

/**
 * 
 */
UCLASS()
class FOURBLADES_API UFBCharacterJobWidget : public UCommonUserWidget
{
	GENERATED_BODY()
public:
	UFBCharacterJobWidget(const FObjectInitializer& ObjectInitializer);

protected:
	//virtual void NativeOnInitialized() override; 현재는 사용 안함

	virtual void NativeConstruct() override;

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Image", meta = (BindWidget))
	TObjectPtr<class UImage> JobImage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Job Images")
	TMap<EFBCharacterJob, TSoftObjectPtr<UTexture2D>> JobImageMap;

	void UpdateJobImage(const EFBCharacterJob CharacterJob);

	void LoadJobImageSync(TSoftObjectPtr<UTexture2D> ImageAsset);

protected:
	UPROPERTY(BlueprintReadOnly, Category = "TextBlock", meta = (BindWidget))
	TObjectPtr<class UTextBlock> CharacterLevel;

	void UpdateCharacterLevel(const uint8 InOldLevel, const uint8 InNewLevel);

protected:
	virtual void NativeDestruct() override;
};
