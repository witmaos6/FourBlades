// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/FBItemComponent.h"
#include "GameplayTagContainer.h"
#include "ChangeAttachmentSocketComponent.generated.h"

/*
*  확장 필요 시 구현
*/
//USTRUCT(BlueprintType)
//struct FTagSocketSwitchAttachment
//{
//	GENERATED_BODY()
//
//	UPROPERTY(EditDefaultsOnly, Category = "ChangeAttack")
//	int8 bWhenAddedTag : 1;
//
//	UPROPERTY(EditDefaultsOnly, Category = "ChangeAttack")
//	FGameplayTag Tag;
//
//	UPROPERTY(EditDefaultsOnly, Category = "ChangeAttack")
//	FName ChangeSocketName;
//};

/**
 * 
 */
UCLASS(Blueprintable, Meta = (BlueprintSpawnableComponent))
class FOURBLADES_API UChangeAttachmentSocketComponent : public UFBItemComponent
{
	GENERATED_BODY()
public:
	UChangeAttachmentSocketComponent();

protected:
	virtual void OnRegister();

	UPROPERTY()
	FName OriginSocketName;

	UPROPERTY(EditDefaultsOnly, Category = "Bind")
	uint8 bWhenAddedTag : 1;

	UPROPERTY(EditDefaultsOnly, Category = "SocketName")
	FName ChangeSocketName;

	UPROPERTY(EditDefaultsOnly, Category = Tag)
	FGameplayTag TriggerSocketChangeTag;

public:
	void SetOriginSocketName(FName InSocketName) { OriginSocketName = InSocketName; }

	void BindChangeSocket();

protected:
	void TagAddedChangeAttachmentSocket(const FGameplayTag Tag, int32 NewCount);

	void TagRemovedChangeAttachmentSocket(const FGameplayTag Tag, int32 NewCount);

};
