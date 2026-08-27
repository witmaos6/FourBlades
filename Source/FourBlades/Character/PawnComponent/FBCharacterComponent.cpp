// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/PawnComponent/FBCharacterComponent.h"
#include "FBPawnExtensionComponent.h"
#include "GameplayTags/FBGameplayTags.h"
#include "Player/FBPlayerState.h"
#include "Player/FBPlayerController.h"
#include "Components/GameFrameworkComponentManager.h"
#include "Character/FBPawnData.h"
#include "Camera/FBCameraComponent.h"
#include "PlayerMappableInputConfig.h"
#include "Input/FBInputComponent.h"
#include "Input/FBMappableConfigPair.h"
#include "EnhancedInputSubsystems.h"
#include "AbilitySystem/FBAbilitySystemComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FBCharacterComponent)

const FName UFBCharacterComponent::NAME_ActorFeatureName("FBCharacterComp");

const FName UFBCharacterComponent::NAME_BindInputsNow("BindInputsNow");

UFBCharacterComponent::UFBCharacterComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = false;
}

void UFBCharacterComponent::OnRegister()
{
	Super::OnRegister();

	if (GetPawn<APawn>() == nullptr) // Component가 Pawn에 잘 부착되어 있는지 확인
	{
		// UE_LOG()
		return;
	}

	RegisterInitStateFeature();
}

void UFBCharacterComponent::BeginPlay()
{
	Super::BeginPlay();

	BindOnActorInitStateChanged(UFBPawnExtensionComponent::NAME_ActorFeatureName, FGameplayTag(), false); // PawnExtension만 관찰하면 되기 때문에 첫번째 인자로 PawnExtension을 넣어준다.

	ensure(TryToChangeInitState(FFBGameplayTags::Get().InitState_Spawned));

	CheckDefaultInitialization();
}

void UFBCharacterComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UnregisterInitStateFeature();

	Super::EndPlay(EndPlayReason);
}

void UFBCharacterComponent::OnActorInitStateChanged(const FActorInitStateChangedParams& Params)
{
	const FFBGameplayTags& InitTags = FFBGameplayTags::Get();
	if (Params.FeatureName == UFBPawnExtensionComponent::NAME_ActorFeatureName)
	{
		if (Params.FeatureState == InitTags.InitState_DataInitialized)
		{
			CheckDefaultInitialization();
		}
	}
}

bool UFBCharacterComponent::CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) const
{
	check(Manager);

	APawn* Pawn = GetPawn<APawn>();
	const FFBGameplayTags& InitTags = FFBGameplayTags::Get();
	AFBPlayerState* PS = GetPlayerState<AFBPlayerState>();

	if (!CurrentState.IsValid() && DesiredState == InitTags.InitState_Spawned)
	{
		if (Pawn)
		{
			return true;
		}
	}

	if (CurrentState == InitTags.InitState_Spawned && DesiredState == InitTags.InitState_DataAvailable)
	{
		if (!PS)
		{
			return false;
		}
		return true;
	}

	if (CurrentState == InitTags.InitState_DataAvailable && DesiredState == InitTags.InitState_DataInitialized)
	{
		return PS && Manager->HasFeatureReachedInitState(Pawn, UFBPawnExtensionComponent::NAME_ActorFeatureName, InitTags.InitState_DataInitialized);
	}

	if (CurrentState == InitTags.InitState_DataInitialized && DesiredState == InitTags.InitState_GameplayReady)
	{
		return true;
	}

	return false;
}

void UFBCharacterComponent::HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState)
{
	const FFBGameplayTags& InitTags = FFBGameplayTags::Get();

	if (CurrentState == InitTags.InitState_DataAvailable && DesiredState == InitTags.InitState_DataInitialized)
	{
		APawn* Pawn = GetPawn<APawn>();
		AFBPlayerState* PS = GetPlayerState<AFBPlayerState>();
		if (!ensure(Pawn && PS))
		{
			return;
		}

		const bool bIsLocallyControlled = Pawn->IsLocallyControlled();
		const UFBPawnData* PawnData = nullptr;

		if (UFBPawnExtensionComponent* PawnExtComp = UFBPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
		{
			PawnData = PawnExtComp->GetPawnData<UFBPawnData>();

			PawnExtComp->InitializeAbilitySystem(PS->GetFBAbilitySystemComponent(), PS);
		}

		if (bIsLocallyControlled && PawnData)
		{
			if (UFBCameraComponent* CameraComponent = UFBCameraComponent::FindCameraComponent(Pawn))
			{
				CameraComponent->DetermineCameraModeDelegate.BindUObject(this, &ThisClass::DetermineCameraMode);
			}
		}

		if (AFBPlayerController* PC = GetController<AFBPlayerController>())
		{
			if (Pawn->InputComponent != nullptr)
			{
				InitializePlayerInput(Pawn->InputComponent);
			}
		}
	}
}

void UFBCharacterComponent::CheckDefaultInitialization()
{
	const FFBGameplayTags& InitTags = FFBGameplayTags::Get();
	static const TArray<FGameplayTag> StateChain = { InitTags.InitState_Spawned, InitTags.InitState_DataAvailable, InitTags.InitState_DataInitialized, InitTags.InitState_GameplayReady };
	ContinueInitStateChain(StateChain);
}

TSubclassOf<class UFBCameraMode> UFBCharacterComponent::DetermineCameraMode() const
{
	const APawn* Pawn = GetPawn<APawn>();
	if (!Pawn)
		return nullptr;

	if (UFBPawnExtensionComponent* PawnExtComp = UFBPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
	{
		if (const UFBPawnData* PawnData = PawnExtComp->GetPawnData<UFBPawnData>())
			return PawnData->DefaultCameraMode;
	}

	return nullptr;
}

void UFBCharacterComponent::InitializePlayerInput(UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);

	const APawn* Pawn = GetPawn<APawn>();
	if (Pawn == nullptr)
	{
		return;
	}
		
	const APlayerController* PC = GetController<APlayerController>();
	check(PC);

	const ULocalPlayer* LP = PC->GetLocalPlayer();
	check(LP);

	UEnhancedInputLocalPlayerSubsystem* Subsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	check(Subsystem);

	Subsystem->ClearAllMappings();
	
	AllConfigs.Empty();
	for (const FFBMappableConfigPair& Pair : InputConfigs)
	{
		if (!Pair.ConfigName.IsNone())
		{
			AllConfigs.Add(Pair.ConfigName, Pair);
		}
	}
	ApplyInputConfigs();

	if (const UFBPawnExtensionComponent* PawnExtComp = UFBPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
	{
		if (const UFBPawnData* PawnData = PawnExtComp->GetPawnData<UFBPawnData>())
		{
			if (const UFBInputConfig* InputConfig = PawnData->InputConfig)
			{
				const FFBGameplayTags& GameplayTags = FFBGameplayTags::Get();

				UFBInputComponent* IC = CastChecked<UFBInputComponent>(PlayerInputComponent);
				{
					{
						TArray<uint32> BindHandles;
						IC->BindAbilityActions(InputConfig, this, &ThisClass::Input_AbilityInputTagPressed, &ThisClass::Input_AbilityInputTagReleased, BindHandles);
					}

					IC->BindNativeAction(InputConfig, GameplayTags.InputTag_Move, ETriggerEvent::Triggered, this, &ThisClass::Input_Move, false);
					IC->BindNativeAction(InputConfig, GameplayTags.InputTag_Look_Mouse, ETriggerEvent::Triggered, this, &ThisClass::Input_Look_Mouse, false);
					IC->BindNativeAction(InputConfig, GameplayTags.InputTag_Ability_Confirm, ETriggerEvent::Triggered, this, &ThisClass::Input_AbilityInputConfirm, false);
				}
			}
		}
	}

	// GameFeatureAction_AddInputConfig의 HandlePawnExtension 콜백 함수 전달 
	UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(const_cast<APawn*>(Pawn), NAME_BindInputsNow);
}

void UFBCharacterComponent::Input_Move(const FInputActionValue& InputActionValue)
{
	APawn* Pawn = GetPawn<APawn>();
	AController* Controller = Pawn ? Pawn->GetController() : nullptr;

	if (Controller)
	{
		const FVector2D Value = InputActionValue.Get<FVector2D>();
		const FRotator MovementRotation(0.0f, Controller->GetControlRotation().Yaw, 0.0f);

		if (Value.X != 0.0f)
		{
			const FVector ForWardDirection = FRotationMatrix(MovementRotation).GetUnitAxis(EAxis::X);
			Pawn->AddMovementInput(ForWardDirection, Value.X);
		}

		if (Value.Y != 0.0f)
		{
			const FVector RightDirection = FRotationMatrix(MovementRotation).GetUnitAxis(EAxis::Y);
			Pawn->AddMovementInput(RightDirection, Value.Y);
		}
	}
}

void UFBCharacterComponent::Input_Look_Mouse(const FInputActionValue& InputActionValue)
{
	APawn* Pawn = GetPawn<APawn>();
	if (Pawn == nullptr)
	{
		return;
	}

	const FVector2D Value = InputActionValue.Get<FVector2D>();
	if (Value.X != 0.0f)
	{
		Pawn->AddControllerYawInput(Value.X);
	}

	if (Value.Y != 0.0f)
	{
		double AimInversionValue = -Value.Y; // IA_Look_Mouse에서 Y값을 Negative로 설정하는 것도 가능
		Pawn->AddControllerPitchInput(AimInversionValue);
	}
}

void UFBCharacterComponent::Input_AbilityInputTagPressed(FGameplayTag InputTag)
{
	if (const APawn* Pawn = GetPawn<APawn>())
	{
		if (const UFBPawnExtensionComponent* PawnExtComp = UFBPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
		{
			if (UFBAbilitySystemComponent* ASC = PawnExtComp->GetFBAbilitySystemComponent())
			{
				ASC->AbilityInputTagPressed(InputTag);
			}
		}
	}
}

void UFBCharacterComponent::Input_AbilityInputTagReleased(FGameplayTag InputTag)
{
	if (const APawn* Pawn = GetPawn<APawn>())
	{
		if (const UFBPawnExtensionComponent* PawnExtComp = UFBPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
		{
			if (UFBAbilitySystemComponent* ASC = PawnExtComp->GetFBAbilitySystemComponent())
			{
				ASC->AbilityInputTagReleased(InputTag);
			}
		}
	}
}

void UFBCharacterComponent::Input_AbilityInputConfirm()
{
	if (const APawn* Pawn = GetPawn<APawn>())
	{
		if (const UFBPawnExtensionComponent* PawnExtComp = UFBPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
		{
			if (UFBAbilitySystemComponent* ASC = PawnExtComp->GetFBAbilitySystemComponent())
			{
				ASC->LocalInputConfirm();
			}
		}
	}
}

void UFBCharacterComponent::ActivateInputConfig(FName ConfigName)
{
	if (ConfigName.IsNone())
	{
		UE_LOG(LogTemp, Warning, TEXT("ActivateInputConfig: ConfigName is None"));
		return;
	}

	if (FFBMappableConfigPair* ConfigPair = AllConfigs.Find(ConfigName))
	{
		ActivateIMC(*ConfigPair);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("InputConfig '%s' not found"), *ConfigName.ToString());
	}
}

void UFBCharacterComponent::DeactivateInputConfig(FName ConfigName)
{
	if (ConfigName.IsNone())
	{
		UE_LOG(LogTemp, Warning, TEXT("DeactivateInputConfig: ConfigName is None"));
		return;
	}

	if (FFBMappableConfigPair* ConfigPair = ActiveConfigs.Find(ConfigName))
	{
		DeactivateIMC(*ConfigPair);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("InputConfig '%s' is not active"), *ConfigName.ToString());
	}
}

void UFBCharacterComponent::SwitchInputConfig(FName DeactivateConfig, FName ActivateConfig)
{
	if (!DeactivateConfig.IsNone())
	{
		DeactivateInputConfig(DeactivateConfig);
	}

	if (!ActivateConfig.IsNone())
	{
		ActivateInputConfig(ActivateConfig);
	}
}

bool UFBCharacterComponent::IsInputConfigActive(FName ConfigName)
{
	return ActiveConfigs.Contains(ConfigName);
}

void UFBCharacterComponent::ApplyInputConfigs()
{
	UEnhancedInputLocalPlayerSubsystem* Subsystem = GetEnhancedInputSubsystem();
	if (!Subsystem)
	{
		return;
	}

	for (const FFBMappableConfigPair& Pair : InputConfigs)
	{
		if (!Pair.InputMappingContext.IsNull())
		{
			switch (Pair.ActivationType)
			{
			case EIMCActivationType::AlwaysActive:
			case EIMCActivationType::Default:
				ActivateIMC(Pair);
				break;

			case EIMCActivationType::Manual:
				break;
			}
		}
	}
}

void UFBCharacterComponent::ActivateIMC(const FFBMappableConfigPair& ConfigPair)
{
	UEnhancedInputLocalPlayerSubsystem* Subsystem = GetEnhancedInputSubsystem();
	if (!Subsystem || ConfigPair.InputMappingContext.IsNull())
	{
		return;
	}

	if (ActiveConfigs.Contains(ConfigPair.ConfigName))
	{
		UE_LOG(LogTemp, Warning, TEXT("IMC '%s' is already active"), *ConfigPair.ConfigName.ToString());
		return;
	}

	UInputMappingContext* IMC = ConfigPair.InputMappingContext.LoadSynchronous();
	if (IMC)
	{
		FModifyContextOptions Options;
		Options.bIgnoreAllPressedKeysUntilRelease = false;

		Subsystem->AddMappingContext(IMC, ConfigPair.Priority, Options);

		ActiveConfigs.Add(ConfigPair.ConfigName, ConfigPair);
	}
}

void UFBCharacterComponent::DeactivateIMC(const FFBMappableConfigPair& ConfigPair)
{
	UEnhancedInputLocalPlayerSubsystem* Subsystem = GetEnhancedInputSubsystem();
	if (!Subsystem || ConfigPair.InputMappingContext.IsNull())
	{
		return;
	}

	if (!ActiveConfigs.Contains(ConfigPair.ConfigName))
	{
		UE_LOG(LogTemp, Warning, TEXT("IMC '%s' is not active"), *ConfigPair.ConfigName.ToString());
		return;
	}

	UInputMappingContext* IMC = ConfigPair.InputMappingContext.LoadSynchronous();
	if (IMC)
	{
		Subsystem->RemoveMappingContext(IMC);

		ActiveConfigs.Remove(ConfigPair.ConfigName);
	}
}

UEnhancedInputLocalPlayerSubsystem* UFBCharacterComponent::GetEnhancedInputSubsystem() const
{
	const APawn* Pawn = GetPawn<APawn>();
    if (!Pawn)
    {
        return nullptr;
    }

    const APlayerController* PC = Cast<APlayerController>(Pawn->GetController());
    if (!PC)
    {
        return nullptr;
    }

    const ULocalPlayer* LP = PC->GetLocalPlayer();
    if (!LP)
    {
        return nullptr;
    }

    return LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
}
