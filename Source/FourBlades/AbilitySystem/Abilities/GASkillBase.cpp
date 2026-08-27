// Fill out your copyright notice in the Description page of Project Settings.


#include "GASkillBase.h"
#include "Character/FBCharacterBase.h"
#include "AbilitySystem/GameplayEffect/FBDamageEffect.h"
#include "AbilitySystemBlueprintLibrary.h"

UGASkillBase::UGASkillBase()
{
	DamageEffect = UFBDamageEffect::StaticClass();
	// To do: 캐릭터가 웨폰을 착용했을 때만 실행 허용
}
