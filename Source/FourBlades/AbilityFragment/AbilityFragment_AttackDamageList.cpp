// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilityFragment/AbilityFragment_AttackDamageList.h"

const FSkillDamageList& UAbilityFragment_AttackDamageList::GetSkillDamageValueList(uint8 Index)
{
    if (SkillDamageValueList.IsValidIndex(Index))
    {
        return SkillDamageValueList[Index];
    }
    return SkillDamageValueList.Last();
}
