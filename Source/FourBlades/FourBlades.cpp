// Copyright Epic Games, Inc. All Rights Reserved.

#include "FourBlades.h"
#include "Modules/ModuleManager.h"

class FFBGameModule : public FDefaultGameModuleImpl
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};

void FFBGameModule::StartupModule()
{
	FDefaultGameModuleImpl::StartupModule();
}

void FFBGameModule::ShutdownModule()
{
	FDefaultGameModuleImpl::ShutdownModule();
}

IMPLEMENT_PRIMARY_GAME_MODULE(FFBGameModule, FourBlades, "FourBlades");
