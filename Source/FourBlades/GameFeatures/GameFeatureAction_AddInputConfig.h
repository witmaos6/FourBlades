#pragma once

#include "CoreMinimal.h"
#include "GameFeatureAction_WorldActionBase.h"
#include "Input/FBMappableConfigPair.h"
#include "GameFeatureAction_AddInputConfig.generated.h"

UCLASS()
class UGameFeatureAction_AddInputConfig : public UGameFeatureAction_WorldActionBase
{
	GENERATED_BODY()
public:
	virtual void OnGameFeatureActivating(FGameFeatureActivatingContext& Context) override;

	virtual void OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context) override;

	virtual void AddToWorld(const FWorldContext& WorldContext, const FGameFeatureStateChangeContext& ChangeContext) override;

private:
	struct FPerContextData
	{
		TArray<TSharedPtr<struct FComponentRequestHandle>> ExtensionRequestHandles;
		TArray<TWeakObjectPtr<APawn>> PawnsAddedTo;
	};

	void Reset(FPerContextData& ActiveData);

	// GameFeatureState 변경 시 콜백되는 함수
	void HandlePawnExtension(AActor* Actor, FName EventName, FGameFeatureStateChangeContext ChangeContext);

	// InputConfig를 EnhancedInputLocalPlayerSubsystem에 추가 및 제거
	void AddInputConfig(APawn* Pawn, FPerContextData& ActiveData); // PlayerMappableInputConfig 사용이 중단되었기 때문에 다른 방식으로 변경 필요, 사용 중단
	void RemoveInputConfig(APawn* Pawn, FPerContextData& ActiveData);// PlayerMappableInputConfig 사용이 중단되었기 때문에 다른 방식으로 변경 필요, 사용 중단

	TMap<FGameFeatureStateChangeContext, FPerContextData> ContextData;

	UPROPERTY(EditAnywhere)
	TArray<FFBMappableConfigPair> InputConfigs;
};