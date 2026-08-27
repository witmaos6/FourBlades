#pragma once

#include "GameplayTagContainer.h"

// CharacterStat.Health
#define FBTAG_CHARACTERSTAT_HEALTH FGameplayTag::RequestGameplayTag(FName("CharacterStat.Health"))
#define FBTAG_CHARACTERSTAT_HEALTH_LEVEL FGameplayTag::RequestGameplayTag(FName("CharacterStat.Health.Level"))
#define FBTAG_CHARACTERSTAT_HEALTH_ALLOCATEDPOINT FGameplayTag::RequestGameplayTag(FName("CharacterStat.Health.AllocatedPoint"))

// CharacterStat.Defense
#define FBTAG_CHARACTERSTAT_DEFENSE FGameplayTag::RequestGameplayTag(FName("CharacterStat.Defense"))
#define FBTAG_CHARACTERSTAT_DEFENSE_LEVEL FGameplayTag::RequestGameplayTag(FName("CharacterStat.Defense.Level"))
#define FBTAG_CHARACTERSTAT_DEFENSE_ALLOCATEDPOINT FGameplayTag::RequestGameplayTag(FName("CharacterStat.Defense.AllocatedPoint"))

// CharacterStat.AttackPower
#define FBTAG_CHARACTERSTAT_ATTACKPOWER FGameplayTag::RequestGameplayTag(FName("CharacterStat.AttackPower"))
#define FBTAG_CHARACTERSTAT_ATTACKPOWER_LEVEL FGameplayTag::RequestGameplayTag(FName("CharacterStat.AttackPower.Level"))
#define FBTAG_CHARACTERSTAT_ATTACKPOWER_ALLOCATEDPOINT FGameplayTag::RequestGameplayTag(FName("CharacterStat.AttackPower.AllocatedPoint"))

// Character.State
#define FBTAG_CHARACTER_STATE_NORMAL FGameplayTag::RequestGameplayTag(FName("Character.State.Normal"))
#define FBTAG_CHARACTER_STATE_ATTACK FGameplayTag::RequestGameplayTag(FName("Character.State.Attack"))
#define FBTAG_CHARACTER_STATE_GUARD FGameplayTag::RequestGameplayTag(FName("Character.State.Guard"))
#define FBTAG_CHARACTER_STATE_AVOID FGameplayTag::RequestGameplayTag(FName("Character.State.Avoid"))
#define FBTAG_CHARACTER_STATE_SPRINT FGameplayTag::RequestGameplayTag(FName("Character.State.Sprint"))
#define FBTAG_CHARACTER_STATE_FLINCH FGameplayTag::RequestGameplayTag(FName("Character.State.Flinch"))
#define FBTAG_CHARACTER_STATE_KNOCKBACK FGameplayTag::RequestGameplayTag(FName("Character.State.Knockback"))
#define FBTAG_CHARACTER_STATE_DEAD FGameplayTag::RequestGameplayTag(FName("Character.State.Dead"))

// Character.Ability
#define FBTAG_CHARACTER_ABILITY_SLASH FGameplayTag::RequestGameplayTag(FName("Character.Ability.Slash"))
#define FBTAG_CHARACTER_ABILITY_SMASH FGameplayTag::RequestGameplayTag(FName("Character.Ability.Smash"))
#define FBTAG_CHARACTER_ABILITY_RUSH FGameplayTag::RequestGameplayTag(FName("Character.Ability.Rush"))
#define FBTAG_CHARACTER_ABILITY_GUARD FGameplayTag::RequestGameplayTag(FName("Character.Ability.Guard"))
#define FBTAG_CHARACTER_ABILITY_AVOID FGameplayTag::RequestGameplayTag(FName("Character.Ability.Avoid"))

#define FBTAG_CHARACTER_ABILITY_CONTINUOUSATTACK_CLASH FGameplayTag::RequestGameplayTag(FName("Character.Ability.ContinuousAttack.Clash"))

// Character.Ability SubTags
#define FBTAG_CHARACTER_ABILITY_GUARD_PARRY FGameplayTag::RequestGameplayTag(FName("Character.Ability.Guard.Parry"))
#define FBTAG_CHARACTER_ABILITY_GUARD_COUNTER FGameplayTag::RequestGameplayTag(FName("Character.Ability.Guard.Counter"))

// Character.Ability.Block
#define FBTAG_CHARACTER_ABILITY_BLOCK_SMASH FGameplayTag::RequestGameplayTag(FName("Character.Ability.Block.Smash"))
#define FBTAG_CHARACTER_ABILITY_BLOCK_CONTINUOUSATTACK FGameplayTag::RequestGameplayTag(FName("Character.Ability.Block.ContinuousAttack"))

// AttackType
#define FBTAG_ATTACKTYPE_ATTACK_NORMAL FGameplayTag::RequestGameplayTag(FName("AttackType.Attack.Normal"))
#define FBTAG_ATTACKTYPE_ATTACK_FLINCH FGameplayTag::RequestGameplayTag(FName("AttackType.Attack.Flinch"))
#define FBTAG_ATTACKTYPE_ATTACK_KNOCKBACK FGameplayTag::RequestGameplayTag(FName("AttackType.Attack.Knockback"))

#define FBTAG_ATTACKTYPE_RESIST_FLINCH FGameplayTag::RequestGameplayTag(FName("AttackType.Resist.Flinch"))
#define FBTAG_ATTACKTYPE_RESIST_KNOCKBACK FGameplayTag::RequestGameplayTag(FName("AttackType.Resist.Knockback"))
#define FBTAG_ATTACKTYPE_RESIST_LAUNCH FGameplayTag::RequestGameplayTag(FName("AttackType.Resist.Launch"))

// Enemy.State
#define FBTAG_ENEMY_STATE_IDLE FGameplayTag::RequestGameplayTag(FName("Enemy.State.Idle"))
#define FBTAG_ENEMY_STATE_ATTACK FGameplayTag::RequestGameplayTag(FName("Enemy.State.Attack"))
#define FBTAG_ENEMY_STATE_FLINCH FGameplayTag::RequestGameplayTag(FName("Enemy.State.Flinch"))
#define FBTAG_ENEMY_STATE_KNOCKBACK FGameplayTag::RequestGameplayTag(FName("Enemy.State.Knockback"))
#define FBTAG_ENEMY_STATE_DEAD FGameplayTag::RequestGameplayTag(FName("Enemy.State.Dead"))

// Enemy.Ability
#define FBTAG_ENEMY_ABILITY_ATTACK FGameplayTag::RequestGameplayTag(FName("Enemy.Ability.Attack"))
#define FBTAG_ENEMY_ABILITY_RUSH FGameplayTag::RequestGameplayTag(FName("Enemy.Ability.Rush"))
#define FBTAG_ENEMY_ABILITY_FLINCH FGameplayTag::RequestGameplayTag(FName("Enemy.Ability.Flinch"))
#define FBTAG_ENEMY_ABILITY_KNOCKBACK FGameplayTag::RequestGameplayTag(FName("Enemy.Ability.Knockback"))
#define FBTAG_ENEMY_ABILITY_DEATH FGameplayTag::RequestGameplayTag(FName("Enemy.Ability.Death"))

// Enemy.Ability.Block
#define FBTAG_ENEMY_ABILITY_BLOCK_CLASH FGameplayTag::RequestGameplayTag(FName("Enemy.Ability.Block.Clash"))

// Enemy.Key
#define FBTAG_ENEMYKEY FGameplayTag::RequestGameplayTag(FName("EnemyKey"))

// SharedAbility
#define FBTAG_SHAREDABILITY_FLINCH FGameplayTag::RequestGameplayTag(FName("SharedAbility.Flinch"))
#define FBTAG_SHAREDABILITY_KNOCKBACK FGameplayTag::RequestGameplayTag(FName("SharedAbility.Knockback"))
#define FBTAG_SHAREDABILITY_LAUNCH FGameplayTag::RequestGameplayTag(FName("SharedAbility.Launch"))
