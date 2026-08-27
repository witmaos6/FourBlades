// Fill out your copyright notice in the Description page of Project Settings.


#include "Camera/FBCameraMode.h"
#include "FBPlayerCameraManager.h"
#include "FBCameraComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FBCameraMode)

/*
* CameraModeView
*/
FFBCameraModeView::FFBCameraModeView()
    : Location(ForceInit)
    , Rotation(ForceInit)
    , ControlRotation(ForceInit)
    , FieldOfView(FB_CAMERA_DEFAULT_FOV)
{

}

void FFBCameraModeView::Blend(const FFBCameraModeView& Other, float OtherWeight)
{
    if (OtherWeight <= 0.0f)
    {
        return;
    }
    else if (OtherWeight >= 1.0f)
    {
        *this = Other;
        return;
    }

    Location = FMath::Lerp(Location, Other.Location, OtherWeight);

    const FRotator DeltaRotation = (Other.Rotation - Rotation).GetNormalized();
    Rotation = Rotation + (OtherWeight * DeltaRotation);

    const FRotator DeltaControlRotation = (Other.ControlRotation - ControlRotation).GetNormalized();
    ControlRotation = ControlRotation + (OtherWeight * DeltaControlRotation);

    FieldOfView = FMath::Lerp(FieldOfView, Other.FieldOfView, OtherWeight);
}

/*
* CameraMode
*/
UFBCameraMode::UFBCameraMode(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    FieldOfView = FB_CAMERA_DEFAULT_FOV;
    ViewPitchMin = FB_CAMERA_DEFAULT_PITCH_MIN;
    ViewPitchMax = FB_CAMERA_DEFAULT_PITCH_MAX;

    BlendTime = 0.0f;
    BlendAlpha = 1.0f;
    BlendWeight = 1.0f;
    BlendExponent = 4.0f;
    BlendFunction = EFBCameraModeBlendFunction::EaseOut;
}

void UFBCameraMode::UpdateCameraMode(float DeltaTime)
{
    UpdateView(DeltaTime);

    UpdateBlending(DeltaTime);
}

void UFBCameraMode::UpdateView(float DeltaTime)
{
    FVector PivotLocation = GetPivotLocation();
    FRotator PivotRotation = GetPivotRotation();

    PivotRotation.Pitch = FMath::ClampAngle(PivotRotation.Pitch, ViewPitchMin, ViewPitchMax);

    View.Location = PivotLocation;
    View.Rotation = PivotRotation;

    View.ControlRotation = View.Rotation;
    View.FieldOfView = FieldOfView;
}

void UFBCameraMode::UpdateBlending(float DeltaTime)
{
    if (BlendTime > 0.0f)
    {
        BlendAlpha += (DeltaTime / BlendTime);
    }
    else
    {
        BlendAlpha = 1.0f;
    }

    const float Exponent = (BlendExponent > 0.0f) ? BlendExponent : 1.0f;
    switch (BlendFunction)
    {
    case EFBCameraModeBlendFunction::Linear:
        BlendWeight = BlendAlpha;
        break;
    case EFBCameraModeBlendFunction::EaseIn:
        BlendWeight = FMath::InterpEaseIn(0.0f, 1.0f, BlendAlpha, Exponent);
        break;
    case EFBCameraModeBlendFunction::EaseOut:
        BlendWeight = FMath::InterpEaseOut(0.0f, 1.0f, BlendAlpha, Exponent);
        break;
    case EFBCameraModeBlendFunction::EaseInOut:
        BlendWeight = FMath::InterpEaseInOut(0.0f, 1.0f, BlendAlpha, Exponent);
        break;
    default:
        checkf(false, TEXT("UpdateBlending: Invalid BlendFunction [%d]\n"), (uint8)BlendFunction);
        break;
    }
}

UFBCameraComponent* UFBCameraMode::GetFBCameraComponent() const
{
    // CameraMode를 생성할 때 CameraStack의 Outer(CameraComponent) 를 넣어주었기 때문에 CameraMode의 Out도 CameraComponent가 된다.
    return CastChecked<UFBCameraComponent>(GetOuter());
}

AActor* UFBCameraMode::GetTargetActor() const
{
    const UFBCameraComponent* CameraComponent = GetFBCameraComponent();
    return CameraComponent->GetTargetActor();
}

FVector UFBCameraMode::GetPivotLocation() const
{
    const AActor* TargetActor = GetTargetActor();
    check(TargetActor);

    if (const APawn* TargetPawn = Cast<APawn>(TargetActor))
    {
        return TargetPawn->GetPawnViewLocation();
    }
    return TargetActor->GetActorLocation();
}

FRotator UFBCameraMode::GetPivotRotation() const
{
    const AActor* TargetActor = GetTargetActor();
    check(TargetActor);

    if (const APawn* TargetPawn = Cast<APawn>(TargetActor))
    {
        return TargetPawn->GetViewRotation();
    }
    return TargetActor->GetActorRotation();
}

/*
* CameraModeStack
*/
UFBCameraModeStack::UFBCameraModeStack(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

UFBCameraMode* UFBCameraModeStack::GetCameraModeInstance(TSubclassOf<UFBCameraMode>& CameraModeClass)
{
    check(CameraModeClass);

    for (UFBCameraMode* CameraMode : CameraModeInstances)
    {
        if ((CameraMode != nullptr) && (CameraMode->GetClass() == CameraModeClass))
        {
            return CameraMode;
        }
    }

    UFBCameraMode* NewCameraMode = NewObject<UFBCameraMode>(GetOuter(), CameraModeClass, NAME_None, RF_NoFlags);
    check(NewCameraMode);

    CameraModeInstances.Add(NewCameraMode);
    return NewCameraMode;
}

void UFBCameraModeStack::PushCameraMode(TSubclassOf<UFBCameraMode>& CameraModeClass)
{
    if (!CameraModeClass)
        return;

    UFBCameraMode* CameraMode = GetCameraModeInstance(CameraModeClass);
    check(CameraMode);

    int32 StackSize = CameraModeStack.Num();
    if ((StackSize > 0) && (CameraModeStack[0] == CameraMode)) // 0이 가장 나중에 넣은 데이터다.
        return;

    int32 ExistingStackIndex = INDEX_NONE;
    float ExistingStackContribution = 1.0f;

    for (int32 StackIndex = 0; StackIndex < StackSize; ++StackIndex)
    {
        if (CameraModeStack[StackIndex] == CameraMode)
        {
            ExistingStackIndex = StackIndex;
            ExistingStackContribution *= CameraMode->BlendWeight;
            break;
        }
        else
        {
            ExistingStackContribution *= (1.0f - CameraModeStack[StackIndex]->BlendWeight);
        }
    }

    if (ExistingStackIndex != INDEX_NONE)
    {
        CameraModeStack.RemoveAt(ExistingStackIndex);
        StackSize--;
    }
    else
    {
        ExistingStackContribution = 0.0f;
    }

    const bool bShouldBlend = ((CameraMode->BlendTime > 0.0f) && (StackSize > 0));
    const float BlendWeight = (bShouldBlend ? ExistingStackContribution : 1.0f);
    CameraMode->BlendWeight = BlendWeight;

    CameraModeStack.Insert(CameraMode, 0);
    CameraModeStack.Last()->BlendWeight = 1.0f;
}

void UFBCameraModeStack::EvaluateStack(float DeltaTime, FFBCameraModeView& OutCameraModeView)
{
    UpdateStack(DeltaTime);

    BlendStack(OutCameraModeView);
}

void UFBCameraModeStack::UpdateStack(float DeltaTime)
{
    const int32 StackSize = CameraModeStack.Num();
    if (StackSize <= 0)
        return;

    int32 RemoveCount = 0;
    int32 RemoveIndex = INDEX_NONE;
    for (int32 StackIndex = 0; StackIndex < StackSize; ++StackIndex)
    {
        UFBCameraMode* CameraMode = CameraModeStack[StackIndex];
        check(CameraMode);

        CameraMode->UpdateCameraMode(DeltaTime);

        if (CameraMode->BlendWeight >= 1.0f)
        {
            RemoveIndex = (StackIndex + 1);
            RemoveCount = (StackSize - RemoveIndex);
            break;
        }
    }

    if (RemoveCount > 0)
    {
        CameraModeStack.RemoveAt(RemoveIndex, RemoveCount);
    }
}

void UFBCameraModeStack::BlendStack(FFBCameraModeView& OutCameraModeView) const
{
    const int32 StackSize = CameraModeStack.Num();
    if (StackSize <= 0)
        return;

    const UFBCameraMode* CameraMode = CameraModeStack[StackSize - 1];
    check(CameraMode);

    OutCameraModeView = CameraMode->View;

    for (int32 StackIndex = (StackSize - 2); StackIndex >= 0; --StackIndex)
    {
        CameraMode = CameraModeStack[StackIndex];
        check(CameraMode);

        OutCameraModeView.Blend(CameraMode->View, CameraMode->BlendWeight);
    }
}
