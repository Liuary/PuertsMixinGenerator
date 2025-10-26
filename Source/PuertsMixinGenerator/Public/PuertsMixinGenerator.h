// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FToolBarBuilder;
class FMenuBuilder;

class FPuertsMixinGeneratorModule : public IModuleInterface
{
public:
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

public:
	
	// 获取当前打开的资产
	UObject* GetCurrentAsset();
	// 生成 PuertsMixin 文件
	void GeneratePuertsMixinFile();

private:
	
	void RegisterMenus();


private:
	TSharedPtr<class FUICommandList> PluginCommands;
};
