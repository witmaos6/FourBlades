// Fill out your copyright notice in the Description page of Project Settings.


#include "GASActor/FBReflectableProjectile.h"
#include "Components/SphereComponent.h"
#include "Character/FBCharacter.h"
#include "Enemy/FBEnemyCharacterBase.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "AbilitySystem/Attributes/FBHealthSet.h"
#include "ActorComponents/LifetimeMonitorComponent.h"
#include "AbilitySystem/FBGameplayEffectContext.h"
#include "Kismet/GameplayStatics.h"
#include "FourBlades.h"

AFBReflectableProjectile::AFBReflectableProjectile()
{
	CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionSphere"));
	CollisionSphere->SetCollisionProfileName(TEXT("FBProjectile"));
	SetRootComponent(CollisionSphere);

	FBHealthSet = CreateDefaultSubobject<UFBHealthSet>(TEXT("HealthSet"));

	ProjectileMovementComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComp"));
	ProjectileMovementComp->InitialSpeed = 1000.f;
	ProjectileMovementComp->MaxSpeed = 3000.f;
	ProjectileMovementComp->bRotationFollowsVelocity = true;

	LifetimeMonitorComponent = CreateDefaultSubobject<ULifetimeMonitorComponent>(TEXT("LifetimeMonitorComponent"));
	LifetimeMonitorComponent->ConditionType = ELifetimeConditionType::TimeBase;
	LifetimeMonitorComponent->Lifetime = 3.0f;

	MaxHealth = 1.0f;

	bIsDelay = false;
	DelayTime = 0.0f;
}

void AFBReflectableProjectile::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (CollisionSphere)
	{
		CollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &AFBReflectableProjectile::OnSphereBeginOverlap);
	}

	if (Owner)
	{
		ProjectileMovementComp->Velocity = GetActorForwardVector() * ProjectileMovementComp->InitialSpeed;
	}

	LifetimeMonitorComponent->OnLifetimeConditionMet.AddUObject(this, &AFBReflectableProjectile::LifetimeDestroy);

	TArray<UAttributeSet*> AddAttributeSet;
	AddAttributeSet.Add(FBHealthSet);
	ASC->SetSpawnedAttributes(AddAttributeSet);

	FBHealthSet->OnCalculateDamageReduction.BindUObject(this, &AFBReflectableProjectile::ApplyCountDamage);
	FBHealthSet->OnApplyDamageToHealth.AddUObject(this, &AFBReflectableProjectile::OnAttacked);

	InitializeStat();

	if (bIsDelay)
	{
		ProjectileMovementComp->SetActive(false);
	}
}

void AFBReflectableProjectile::InitializeStat()
{
	if (ASC)
	{
		UGameplayEffect* StatInitEffect = NewObject<UGameplayEffect>(GetTransientPackage(), FName("StatInitEffect"));
		StatInitEffect->DurationPolicy = EGameplayEffectDurationType::Instant;

		StatInitEffect->Modifiers.SetNum(2);

		StatInitEffect->Modifiers[0].Attribute = UFBHealthSet::GetMaxHealthAttribute();
		StatInitEffect->Modifiers[0].ModifierOp = EGameplayModOp::Override;
		StatInitEffect->Modifiers[0].ModifierMagnitude = FScalableFloat(MaxHealth);

		StatInitEffect->Modifiers[1].Attribute = UFBHealthSet::GetHealthAttribute();
		StatInitEffect->Modifiers[1].ModifierOp = EGameplayModOp::Override;
		StatInitEffect->Modifiers[1].ModifierMagnitude = FScalableFloat(MaxHealth);

		FGameplayEffectContextHandle EffectContextHandle = ASC->MakeEffectContext();
		EffectContextHandle.AddSourceObject(this);
		ASC->ApplyGameplayEffectToSelf(StatInitEffect, 1.0f, EffectContextHandle);
	}
}

void AFBReflectableProjectile::BeginPlay()
{
	Super::BeginPlay();

	if (bIsDelay)
	{
		FTimerHandle ProjectileTimer;
		GetWorld()->GetTimerManager().SetTimer(ProjectileTimer, this, &AFBReflectableProjectile::BeginShot, DelayTime, false);
	}

	LifetimeMonitorComponent->SetCenterLocation(GetActorLocation());
}

void AFBReflectableProjectile::BeginShot()
{
	ProjectileMovementComp->SetActive(true);
	ProjectileMovementComp->Velocity = GetActorForwardVector() * ProjectileMovementComp->InitialSpeed;
}

void AFBReflectableProjectile::OnAttacked(AActor* ASCInstigator, AActor* EffectCauser, const FFBTargetPayload& InTargetPayload)
{
	if (EffectCauser == nullptr)
		return;

	if (CachedInstigator.IsValid())
	{
		if (AActor* OriginInstigator = CachedInstigator.Get())
		{
			FVector OriginInstigatorLocation = OriginInstigator->GetActorLocation();
			FVector EffectCauserLocation = EffectCauser->GetActorLocation();

			ProjectileMovementComp->Velocity = (OriginInstigatorLocation - EffectCauserLocation).GetSafeNormal() * ProjectileMovementComp->InitialSpeed;

			bReflectable = true;

			LifetimeMonitorComponent->ResetLifeTimer();
			//LifetimeMonitorComponent->SetCenterLocation(EffectCauser->GetActorLocation());

			ApplyTemporaryTimeDilation(0.4f, 0.2f);

			FGameplayCueParameters CueParameters;
			CueParameters.Location = GetActorLocation();
			ASC->ExecuteGameplayCue(ReflectSuccessCue, CueParameters);
		}
	}
}

void AFBReflectableProjectile::ApplyTemporaryTimeDilation(float TimeScale, float DurationInRealSeconds)
{
	UWorld* World = GetWorld();
	if (World == nullptr)
	{
		return;
	}

	TimeScale = FMath::Clamp(TimeScale, 0.01f, 1.0f);
	DurationInRealSeconds = FMath::Max(DurationInRealSeconds, 0.0f);

	UGameplayStatics::SetGlobalTimeDilation(World, TimeScale);

	float RealResetTime = DurationInRealSeconds * TimeScale;
	FTimerHandle ResetTimer;
	World->GetTimerManager().SetTimer(ResetTimer, this, &AFBReflectableProjectile::ResetGlobalTimeDilation, RealResetTime, false);
}

void AFBReflectableProjectile::ResetGlobalTimeDilation()
{
	if (UWorld* World = GetWorld())
	{
		UGameplayStatics::SetGlobalTimeDilation(World, 1.0f);
	}
}

void AFBReflectableProjectile::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == nullptr)
		return;

	if (OtherActor->IsA(AFBCharacter::StaticClass()))
	{
		if (CachedInstigator.IsValid())
		{
			UAbilitySystemComponent* OwnerASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(CachedInstigator.Get());
			UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor);
			if (OwnerASC && TargetASC)
			{
				if (IsValid(GameplayEffectClass))
				{
					FGameplayEffectContextHandle EffectConetxtHandle = OwnerASC->MakeEffectContext();
					FFBGameplayEffectContext* DamageContext = FFBGameplayEffectContext::ExtractEffectContext(EffectConetxtHandle);
					DamageContext->SetDamageExecutionParams(MoveTemp(DamageExecutionParms));
					DamageContext->SetTargetPayload(MoveTemp(TargetPayload));

					FGameplayEffectSpecHandle EffectSpecHandle = OwnerASC->MakeOutgoingSpec(GameplayEffectClass, 1.0f, EffectConetxtHandle);

					OwnerASC->ApplyGameplayEffectSpecToTarget(*EffectSpecHandle.Data.Get(), TargetASC);

					Destroy();
				}
			}
		}
	}
	else if (OtherActor->IsA(AFBEnemyCharacterBase::StaticClass()) && bReflectable)
	{
		// To do: 플레이어가 튕겨내면 몬스터에게 적용할 데미지 로직 작성 *기획 필요 현재는 원래 데미지 적용
		if (CachedInstigator.IsValid())
		{
			UAbilitySystemComponent* OwnerASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(CachedInstigator.Get());
			if (OwnerASC)
			{
				if (IsValid(GameplayEffectClass))
				{
					FGameplayEffectContextHandle EffectConetxtHandle = OwnerASC->MakeEffectContext();
					FFBGameplayEffectContext* DamageContext = FFBGameplayEffectContext::ExtractEffectContext(EffectConetxtHandle);
					DamageContext->SetDamageExecutionParams(MoveTemp(DamageExecutionParms));
					DamageContext->SetTargetPayload(MoveTemp(TargetPayload));

					FGameplayEffectSpecHandle EffectSpecHandle = OwnerASC->MakeOutgoingSpec(GameplayEffectClass, 1.0f, EffectConetxtHandle);

					OwnerASC->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());

					Destroy();
				}
			}
		}
	}
}

void AFBReflectableProjectile::LifetimeDestroy()
{
	Destroy();
}

void AFBReflectableProjectile::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UWorld* World = GetWorld())
	{
		UGameplayStatics::SetGlobalTimeDilation(World, 1.0f);
	}

	Super::EndPlay(EndPlayReason);
}
