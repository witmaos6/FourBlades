// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/PlayerComponent/EnemyHealthbarManager.h"
#include "Physics/FBCollisionChannels.h"
#include "Enemy/FBEnemyCharacterBase.h"
#include "Components/CapsuleComponent.h"
#include "Engine/OverlapResult.h" 
#include "Blueprint/UserWidget.h"
#include "UI/FBGASUserWidget.h"
#include "Library/FBDetectionUtilityLibrary.h"
#include "FourBlades.h"

UEnemyHealthbarManager::UEnemyHealthbarManager(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;

	VisibleDistance = 2000.f;
}

void UEnemyHealthbarManager::BeginPlay()
{
	Super::BeginPlay();

	GetWorldTimerManager().SetTimer(EnemyDetectTimer, this, &UEnemyHealthbarManager::EnemyDetect, 1.0f, true);
}

void UEnemyHealthbarManager::EnemyDetect()
{
	if (GetOwner() == nullptr)
		return;

	APlayerController* PC = Cast<APlayerController>(GetOwner());
	if (PC == nullptr)
		return;

	FVector ViewLocation;
	FRotator ViewRotation;
	PC->GetPlayerViewPoint(ViewLocation, ViewRotation);

	DetectedEnemies.Empty();

	FVector TraceCenter = ViewLocation;
	TArray<FOverlapResult> Overlaps;
	bool bHitResult = GetWorld()->OverlapMultiByChannel(Overlaps, TraceCenter, FQuat::Identity, FB_TraceChannel_PlayerAttack, FCollisionShape::MakeSphere(VisibleDistance));

	for (const FOverlapResult& OverlapResult : Overlaps)
	{
		if (AFBEnemyCharacterBase* Enemy = Cast<AFBEnemyCharacterBase>(OverlapResult.GetActor()))
		{
			bool bTargetInSector = UFBDetectionUtilityLibrary::IsTargetInSectorFromActor(ViewLocation, ViewRotation, Enemy, PC->PlayerCameraManager->GetFOVAngle() / 2.0f);
			bool bEnemyGradeIsNormal = (Enemy->GetGrade() == EEnemyGrade::Normal);
			if (bTargetInSector && bEnemyGradeIsNormal)
			{
				DetectedEnemies.Add(Enemy);
			}
		}
	}

#if ENABLE_DRAW_DEBUG
	FColor LineColor = DetectedEnemies.IsEmpty() ? FColor::Red : FColor::Green;
	DrawDebugSphere(GetWorld(), TraceCenter, VisibleDistance, 16, LineColor, false, 0.2f);
#endif
}

void UEnemyHealthbarManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UpdateWidget();
}

void UEnemyHealthbarManager::UpdateWidget()
{
	if (GetOwner() == nullptr)
		return;

	APlayerController* PC = Cast<APlayerController>(GetOwner());
	if (PC == nullptr)
		return;

	TArray<AFBEnemyCharacterBase*> TempDetectedEnemies = DetectedEnemies;

	for (AFBEnemyCharacterBase* DetectedEnemy : TempDetectedEnemies)
	{
		if (!EnemiesWidgets.Contains(DetectedEnemy))
		{
			if (HealthbarWidgetClass)
			{
				UUserWidget* Widget = CreateWidget<UUserWidget>(PC, HealthbarWidgetClass);
				if (Widget)
				{
					Widget->AddToViewport();
					EnemiesWidgets.Add(DetectedEnemy, Widget);
				}
				if (UFBGASUserWidget* GASUserWidget = Cast<UFBGASUserWidget>(Widget)) // Interface로 교체할 수도 있음
				{
					GASUserWidget->SetAbilitySystemComponent(DetectedEnemy);
				}
			}
		}

		UUserWidget* EnemyWidget = EnemiesWidgets.FindRef(DetectedEnemy);
		if (EnemyWidget)
		{
			float ZDistance = DetectedEnemy->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
			FVector WorldLocation = DetectedEnemy->GetActorLocation() + FVector(0, 0, ZDistance);
			FVector2D ScreenPosition;
			bool bProjected = PC->ProjectWorldLocationToScreen(WorldLocation, ScreenPosition);
			if (bProjected)
			{
				EnemyWidget->SetPositionInViewport(ScreenPosition, true);
			}
		}
	}

	TArray<AFBEnemyCharacterBase*> RemoveKey;
	for (auto& EnemyWidget : EnemiesWidgets)
	{
		if (!TempDetectedEnemies.Contains(EnemyWidget.Key))
		{
			if (EnemyWidget.Value)
			{
				EnemyWidget.Value->RemoveFromParent();
				RemoveKey.Add(EnemyWidget.Key);
			}
		}
	}
	for (AFBEnemyCharacterBase* Key : RemoveKey)
	{
		EnemiesWidgets.Remove(Key);
	}
}

void UEnemyHealthbarManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (!DetectedEnemies.IsEmpty())
	{
		DetectedEnemies.Empty();
	}
	if (!EnemiesWidgets.IsEmpty())
	{
		EnemiesWidgets.Empty();
	}
	if (EnemyDetectTimer.IsValid())
	{
		GetWorldTimerManager().ClearTimer(EnemyDetectTimer);
	}
	Super::EndPlay(EndPlayReason);
}