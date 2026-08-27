// Fill out your copyright notice in the Description page of Project Settings.


#include "Equipment/FBEquipmentDefinition.h"
#include "FBEquipmentInstance.h"

UFBEquipmentDefinition::UFBEquipmentDefinition(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
, InstanceType(UFBEquipmentInstance::StaticClass())
{
	
}
