// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/CharacterStatus/FBCharacterJobWidget.h"
#include "Player/FBPlayerState.h"
#include "SaveGame/Interface/FBCharacterProfileInterface.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "SaveGame/FBSaveGame.h"

UFBCharacterJobWidget::UFBCharacterJobWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{

}

void UFBCharacterJobWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (AFBPlayerState* PS = Cast<AFBPlayerState>(GetOwningPlayerState()))
	{
		if (UFBSaveGame* SaveData = PS->GetSaveData())
		{
			if (IFBCharacterProfileInterface* CharacterProfile = Cast<IFBCharacterProfileInterface>(SaveData))
			{
				const EFBCharacterJob Job = CharacterProfile->GetCharacterJob();
				UpdateJobImage(Job);

				UpdateCharacterLevel(0, CharacterProfile->GetCharacterLevel());

				FOnCharacterLevelUp& LevelUpDelegate = CharacterProfile->GetLevelUpDelegate();
				LevelUpDelegate.AddUObject(this, &UFBCharacterJobWidget::UpdateCharacterLevel);
			}
		}
	}
}

void UFBCharacterJobWidget::UpdateJobImage(const EFBCharacterJob CharacterJob)
{
	TSoftObjectPtr<UTexture2D>* FoundImage = JobImageMap.Find(CharacterJob);
	if (FoundImage && !FoundImage->IsNull())
	{
		LoadJobImageSync(*FoundImage);
	}
}

void UFBCharacterJobWidget::LoadJobImageSync(TSoftObjectPtr<UTexture2D> ImageAsset)
{
	if (ImageAsset.IsNull() && JobImage)
	{
		return;
	}

	UTexture2D* LoadedTexture = ImageAsset.LoadSynchronous();
	if (LoadedTexture)
	{
		JobImage->SetBrushFromTexture(LoadedTexture);
	}
}

void UFBCharacterJobWidget::UpdateCharacterLevel(const uint8 InOldLevel, const uint8 InNewLevel)
{
	if (CharacterLevel && InOldLevel != InNewLevel)
	{
		FString FormattedLevel = FString::Printf(TEXT("%02u"), InNewLevel);
		CharacterLevel->SetText(FText::FromString(FormattedLevel));
	}
}

void UFBCharacterJobWidget::NativeDestruct()
{
	// 현재는 로직이 없지만 이후에 캐릭터 사망시 위젯을 삭제하는 등 작업이 필요할 수 있음
	Super::NativeDestruct();
}
