// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "GameplayTagContainer.h"
#include "UIExtensionSystem.h"
#include "GameFeatures/GameFeatureAction_WorldActionBase.h"
#include "GameFeatureAction_AddWidgets.generated.h"

/*
* HUDLayoutRequest
*/
USTRUCT()
struct FFBHUDLayoutRequest
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Category = UI, meta = (AssetBundles = "Client"))
	TSoftClassPtr<UCommonActivatableWidget> LayoutClass; // UI의 레이아웃으로 UCommonActivatableWidget를 사용
	
	UPROPERTY(EditAnywhere, Category = UI)
	FGameplayTag LayerID; // PrimaryGameLayout의 LayerID
};

/*
* HUDElementEntry
*/
USTRUCT()
struct FFBHUDElementEntry
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Category = UI, meta = (AssetBundles = "Client"))
	TSoftClassPtr<UUserWidget> WidgetClass; // HUDLayout 위에 올릴 대상이 되는 Widget

	UPROPERTY(EditAnywhere, Category = UI)
	FGameplayTag SlotID; // HUDLayoutRequest에 올린 LayoutClass에 정의된 Slot(GameplayTag)를 의미
};

/**
 * GameFeatureAction_AddWidgets
 */
UCLASS()
class FOURBLADES_API UGameFeatureAction_AddWidgets : public UGameFeatureAction_WorldActionBase
{
	GENERATED_BODY()
public:
	struct FPerContextData
	{
		TArray<TSharedPtr<struct FComponentRequestHandle>> ComponentRequests;
		TArray<TWeakObjectPtr<UCommonActivatableWidget>> LayoutsAdded;

		TArray<FUIExtensionHandle> ExtensionHandles;
	};
public:
	virtual void AddToWorld(const FWorldContext& WorldContext, const FGameFeatureStateChangeContext& ChangeContext) override;

	void HandleActorExtension(AActor* Actor, FName EventName, FGameFeatureStateChangeContext ChangeContext);

	void AddWidgets(AActor* Actor, FPerContextData& ActiveData);

	void RemoveWidgets(AActor* Actor, FPerContextData& ActiveData);

	virtual void OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context) override;

	void Reset(FPerContextData& ActiveData);

public:
	TMap<FGameFeatureStateChangeContext, FPerContextData> ContextData;

	UPROPERTY(EditAnywhere, Category = UI)
	TArray<FFBHUDLayoutRequest> Layout; // 형태를 정의하는 Layout

	UPROPERTY(EditAnywhere, Category = UI)
	TArray<FFBHUDElementEntry> Widgets; // Layout위에 올라갈 Widget
};
