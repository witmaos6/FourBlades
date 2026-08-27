// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/System/MinimapWidget.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "Kismet/KismetMathLibrary.h"

void UMinimapWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UMinimapWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (APlayerController* Controller = GetOwningPlayer())
	{
		float ControllerYaw = Controller->K2_GetActorRotation().Yaw;
		bool bCondition = ControllerYaw < 0.0f;
		float ResultYaw = UKismetMathLibrary::SelectFloat(ControllerYaw + 360.f, ControllerYaw, bCondition) / 360.f;

		UKismetMaterialLibrary::SetScalarParameterValue(GetWorld(), PlayerTransform, FName(TEXT("PlayerYaw")), ResultYaw);

		if (APawn* OwnerPawn = Controller->GetPawn())
		{
			FLinearColor Color = UKismetMathLibrary::Conv_VectorToLinearColor(OwnerPawn->GetActorLocation());
			UKismetMaterialLibrary::SetVectorParameterValue(GetWorld(), PlayerTransform, FName(TEXT("PlayerLocation2D")), Color);
		}
	}
}
