// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "PuertsMixinGeneratorSetting.generated.h"

/**
 * Plugin configuration settings
 */
UCLASS(Config=PuertsMixinGenerator, defaultconfig, meta=(DisplayName="PuertsMixinGenerator"))
class PUERTSMIXINGENERATOR_API UPuertsMixinGeneratorSetting : public UObject
{
	GENERATED_BODY()

public:
	// PuertsMixin 文件路径，相对于项目根目录
	UPROPERTY(Config, EditAnywhere, Category="PuertsMixinGenerator")
	FString PuertsMixinPath = "TypeScript";

	// PuertsMixin import 列表收集文件路径,相对于 PuertsMixin 文件路径，用来收集所有自动生成的mixin文件并生成一个总的import文件
	UPROPERTY(Config, EditAnywhere, Category="PuertsMixinGenerator")
	FString PuertsImportListPath = "PuertsImportList.ts";

	// PuertsMixin import 列表收集文件范围，只有在这个范围内的文件才会被收集
	UPROPERTY(Config, EditAnywhere, Category="PuertsMixinGenerator")
	FString PuertsImportListPathScope = "Game";

	// PuertsMixin 生成文件替换一些前缀，将Key替换为Value
	UPROPERTY(Config, EditAnywhere, Category="PuertsMixinGenerator")
	TMap<FString, FString> ReplaceNameStartString;
	
};
