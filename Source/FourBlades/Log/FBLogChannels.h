// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

//#include "Containers/UnrealString.h"
//#include "Logging/LogMacros.h"

//Default
DECLARE_LOG_CATEGORY_EXTERN(LogFB, Log, All);

// Network
#define LOG_NETMODEINFO ((GetNetMode() == ENetMode::NM_Client) ? *FString::Printf(TEXT("CLIENT%d"), GPlayInEditorID) : ((GetNetMode() == ENetMode::NM_Standalone) ? TEXT("STANDALONE") : TEXT("SERVER"))) 
#define LOG_CALLINFO ANSI_TO_TCHAR (__FUNCTION__)
#define GAS_LOG(LogCat, Verbosity, Format, ...) UE_LOG(LogCat, Verbosity, TEXT("[%s] %s %s"), LOG_NETMODEINFO, LOG_CALLINFO, *FString::Printf(Format, ##__VA_ARGS__))
DECLARE_LOG_CATEGORY_EXTERN(LogNetwork, Log, All);
