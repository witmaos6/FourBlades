// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/PawnComponent/FBSpawnWidgetComponent.h"
#include "Enemy/FBEnemyCharacterBase.h"
#include "AbilitySystem/FBEnemyAbilitySystemComponent.h"
#include "AbilitySystem/Attributes/FBHealthSet.h"
#include "UI/Combat/DamageWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Components/CapsuleComponent.h"
#include "AbilitySystem/FBGameplayEffectContext.h"
#include "GameFramework/PlayerState.h"
#include "FourBlades.h"

UFBSpawnWidgetComponent::UFBSpawnWidgetComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UFBSpawnWidgetComponent::OnRegister()
{
	Super::OnRegister();

	if (AFBEnemyCharacterBase* Owner = Cast<AFBEnemyCharacterBase>(GetOwner()))
	{
		Owner->CallOrRegister_OnInitAbilityActorInfo(FOnInitAbilityActorInfo::FDelegate::CreateUObject(this, &UFBSpawnWidgetComponent::OnInitAbilityActorInfo));
	}
}

void UFBSpawnWidgetComponent::OnInitAbilityActorInfo(UAbilitySystemComponent* ASC)
{
	if (ASC)
	{
		if (const UFBHealthSet* HealthSet = ASC->GetSetChecked<UFBHealthSet>())
		{
			HealthSet->OnApplyDamageToHealth.AddUObject(this, &UFBSpawnWidgetComponent::SpawnDamageWidget);
		}
	}
}

void UFBSpawnWidgetComponent::SpawnDamageWidget(AActor* Instigator, AActor* EffectCauser, const FFBTargetPayload& TargetPayload)
{
	if (Instigator == nullptr)
	{
		return;
	}

	APlayerState* PS = Cast<APlayerState>(Instigator);
	if (PS == nullptr)
	{
		return;
	}

	APlayerController* PC = PS->GetPlayerController();
	if (PC == nullptr)
	{
		return;
	}

	AFBEnemyCharacterBase* Owner = Cast<AFBEnemyCharacterBase>(GetOwner()); // ACharacter 또는 AActor로 변경할 수 있음
	if (Owner == nullptr)
	{
		return;
	}

	FVector WorldLocation = Owner->GetActorLocation();
	float AddHeight = Owner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	WorldLocation.Z += AddHeight;

	FVector2D ScreenLocation;
	if (UGameplayStatics::ProjectWorldToScreen(PC, WorldLocation, ScreenLocation))
	{
		UDamageWidget* DamageWidget = CreateWidget<UDamageWidget>(GetWorld(), DamageWidgetClass);
		if (DamageWidget)
		{
			DamageWidget->SetPosition2D(ScreenLocation);
			DamageWidget->SetDamageWidget(TargetPayload.GetFinalDamage());
			DamageWidget->AddToViewport();
			DamageWidget->SetVisibility(ESlateVisibility::Visible);
		}
	}
}