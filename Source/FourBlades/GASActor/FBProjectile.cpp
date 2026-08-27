// Fill out your copyright notice in the Description page of Project Settings.


#include "GASActor/FBProjectile.h"
#include "Components/BoxComponent.h"
#include "Character/FBCharacter.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "AbilitySystem/Attributes/FBHealthSet.h"
#include "AbilitySystem/FBGameplayEffectContext.h"

AFBProjectile::AFBProjectile()
{
	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
	CollisionBox->SetCollisionProfileName(TEXT("FBProjectile"));
	SetRootComponent(CollisionBox);

	FBHealthSet = CreateDefaultSubobject<UFBHealthSet>(TEXT("HealthSet"));

	ProjectileMovementComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComp"));
	ProjectileMovementComp->InitialSpeed = 1000.f;
	ProjectileMovementComp->MaxSpeed = 3000.f;
	ProjectileMovementComp->bRotationFollowsVelocity = true;

	MaxHealth = 1.0f;

	bIsDelay = false;
	DelayTime = 0.0f;
}

void AFBProjectile::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (CollisionBox)
	{
		CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &AFBProjectile::OnBoxBeginOverlap);
		CollisionBox->OnComponentHit.AddDynamic(this, &AFBProjectile::OnBoxBlock);
	}

	if (Owner)
	{
		ProjectileMovementComp->Velocity = GetActorForwardVector() * ProjectileMovementComp->InitialSpeed;
	}

	TArray<UAttributeSet*> AddAttributeSet;
	AddAttributeSet.Add(FBHealthSet);
	ASC->SetSpawnedAttributes(AddAttributeSet);

	FBHealthSet->OnCalculateDamageReduction.BindUObject(this, &AFBProjectile::ApplyCountDamage);
	FBHealthSet->OnOutOfHealth.AddUObject(this, &AFBProjectile::OnOutOfHealth);

	InitializeStat();

	if (bIsDelay)
	{
		ProjectileMovementComp->SetActive(false);
	}
}

void AFBProjectile::InitializeStat()
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

void AFBProjectile::BeginPlay()
{
	Super::BeginPlay();

	if (bIsDelay)
	{
		FTimerHandle ProjectileTimer;
		GetWorld()->GetTimerManager().SetTimer(ProjectileTimer, this, &AFBProjectile::BeginShot, DelayTime, false);
	}
}

void AFBProjectile::BeginShot()
{
	ProjectileMovementComp->SetActive(true);
	ProjectileMovementComp->Velocity = GetActorForwardVector() * ProjectileMovementComp->InitialSpeed;
}

void AFBProjectile::OnOutOfHealth()
{
	// To do: Execute Gameplay Cue, Sound and particle
	if (ASC && DestructionParticleTag.IsValid())
	{
		FGameplayCueParameters CueParams;
		CueParams.Location = GetActorLocation();
		CueParams.Normal = GetActorForwardVector();
		CueParams.Instigator = this;
		ASC->ExecuteGameplayCue(DestructionParticleTag, CueParams);
	}
	Destroy();
}

void AFBProjectile::OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor->IsA(AFBCharacter::StaticClass()))
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
}

void AFBProjectile::OnBoxBlock(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	Destroy();
}
