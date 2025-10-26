// Copyright Epic Games, Inc. All Rights Reserved.

#include "PuertsMixinGenerator.h"

#include "ISettingsModule.h"
#include "PuertsMixinGeneratorStyle.h"
#include "PuertsMixinGeneratorCommands.h"
#include "PuertsMixinGeneratorSetting.h"
#include "Selection.h"
#include "Misc/MessageDialog.h"
#include "ToolMenus.h"
#include "WidgetBlueprint.h"
#include "Tools/UAssetEditor.h"

static const FName PuertsMixinGeneratorTabName("PuertsMixinGenerator");
static const FString PuertsMixinGeneratorGenerateFlag = TEXT("PuertsMixinGenerator_Auto_Generator_Flag");

#define LOCTEXT_NAMESPACE "FPuertsMixinGeneratorModule"

void FPuertsMixinGeneratorModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	FPuertsMixinGeneratorStyle::Initialize();
	FPuertsMixinGeneratorStyle::ReloadTextures();

	FPuertsMixinGeneratorCommands::Register();

	PluginCommands = MakeShareable(new FUICommandList);

	// 注册菜单 - 生成PuertsMixin文件
	PluginCommands->MapAction(
		FPuertsMixinGeneratorCommands::Get().GeneratePuertsMixinFileAction,
		FExecuteAction::CreateRaw(this, &FPuertsMixinGeneratorModule::GeneratePuertsMixinFile),
		FCanExecuteAction());

	UToolMenus::RegisterStartupCallback(
		FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FPuertsMixinGeneratorModule::RegisterMenus));

	// 注册设置
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->RegisterSettings("Project", "Plugins", "PuertsMixinGenerator",
		                                 FText::FromString("Puerts Mixin Generator Settings"),
		                                 FText::FromString("Open Puerts Mixin Generator Settings"),
		                                 GetMutableDefault<UPuertsMixinGeneratorSetting>());
	}
}

void FPuertsMixinGeneratorModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	// 取消注册设置
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->UnregisterSettings("Project", "Plugins", "PuertsMixinGenerator");
	}

	UToolMenus::UnRegisterStartupCallback(this);

	UToolMenus::UnregisterOwner(this);

	FPuertsMixinGeneratorStyle::Shutdown();

	FPuertsMixinGeneratorCommands::Unregister();
}

UObject* FPuertsMixinGeneratorModule::GetCurrentAsset()
{
	// 通过AssetEditorSubsystem获取当前编辑的资产
	UAssetEditorSubsystem* AssetEditorSubsystem = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>();
	
	if (!AssetEditorSubsystem)
	{
		return nullptr;
	}
	
	// 获取资产列表
	TArray<UObject*> AssetEditors = AssetEditorSubsystem->GetAllEditedAssets();
	double LastActiveTime = 0;
	UObject* CurrentAsset = nullptr;
	for (size_t i = 0; i < AssetEditors.Num(); ++i)
	{
		// 获取资产编辑器实例
		UObject* Asset = AssetEditors[i];
		IAssetEditorInstance* AssetEditor = AssetEditorSubsystem->FindEditorForAsset(Asset, false);
		if (!AssetEditor)
		{
			continue;
		}
		if (AssetEditor->GetLastActivationTime() > LastActiveTime)
		{
			LastActiveTime = AssetEditor->GetLastActivationTime();
			CurrentAsset = Asset;
		}
	}
	
	return CurrentAsset;
}
void FPuertsMixinGeneratorModule::RegisterMenus()
{
	// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
	FToolMenuOwnerScoped OwnerScoped(this);
	{
		// 添加到蓝图编辑器的工具栏中
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("AssetEditor.BlueprintEditor.ToolBar");
		FToolMenuSection& Section = Menu->AddSection("PuertsMixin", LOCTEXT("PuertsMixin", "Puerts Mixin"));
		Section.AddMenuEntryWithCommandList(FPuertsMixinGeneratorCommands::Get().GeneratePuertsMixinFileAction,
		                                    PluginCommands);
	}
	{
		// 添加到控件蓝图编辑器的工具栏中
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("AssetEditor.WidgetBlueprintEditor.ToolBar");
		FToolMenuSection& Section = Menu->AddSection("PuertsMixin", LOCTEXT("PuertsMixin", "Puerts Mixin"));
		Section.AddMenuEntryWithCommandList(FPuertsMixinGeneratorCommands::Get().GeneratePuertsMixinFileAction,
		                                    PluginCommands);
	}
}

void FPuertsMixinGeneratorModule::GeneratePuertsMixinFile()
{
	// 获取当前所在资产
	UObject* Asset = GetCurrentAsset();
	if (Asset == nullptr)
	{
		FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("NoAssetSelected", "当前未打开蓝图资产"));
		return;
	}

	UBlueprint* Blueprint = Cast<UBlueprint>(Asset);
	// 检查是否是蓝图文件
	if (!Blueprint)
	{
		FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("NotBlueprintAsset", "当前资产不是蓝图"));
		return;
	}

	// 获取资产所在目录
	FString AssetDir = FPaths::GetPath(Asset->GetOutermost()->GetName());
	// 获取资产名字
	FString AssetName = FPaths::GetBaseFilename(Asset->GetName());

	// 如果文件名字前缀是一些特定文本，则替换
	FString GenerateName = AssetName;
	for (auto [key, value] : GetDefault<UPuertsMixinGeneratorSetting>()->ReplaceNameStartString)
	{
		if (GenerateName.Contains(key))
		{
			if (GenerateName.RemoveFromStart(key))
			{
				GenerateName = value + GenerateName;
			}
		}
	}

	// 读取设置的生成目录
	FString GeneratedDir = GetDefault<UPuertsMixinGeneratorSetting>()->PuertsMixinPath;

	// 获取生成文件路径：项目目录 + 生成目录 + 资产目录 + 资产名 + .ts
	FString GeneratedFilePath = FPaths::ProjectDir() / GeneratedDir / AssetDir / GenerateName + ".ts";

	// 检查文件是否已经生成，未生成才会生成新的文件
	if (!FPaths::FileExists(GeneratedFilePath))
	{
		// 获取资产路径
		FString AssetPath;

		// 获取生成的类
		if (UClass* GeneratedClass = Blueprint->GeneratedClass)
		{
			AssetPath = GeneratedClass->GetPathName();
		}
		else
		{
			// 备选方案：使用命名约定
			AssetPath = FString::Printf(TEXT("%s_C"), *Blueprint->GetName());
		}

		// 将路径的 / 替换为 .
		FString AssetPathPoint = AssetPath;
		AssetPathPoint.ReplaceInline(TEXT("/"), TEXT("."));

		// 生成文件内容
		FString FileContent = FString::Printf(TEXT("// %s\n"), *PuertsMixinGeneratorGenerateFlag);
		FileContent += FString::Printf(TEXT("import * as UE from 'ue';\n"));
		FileContent += FString::Printf(TEXT("import { blueprint } from 'puerts';\n"));

		FString SingleFind = "";
		// 从 AssetDir 中查找 '\\'或者'/'出现的次数，每出现一次，SingleFind增加一个'../'
		for (int32 i = 0; i < AssetDir.Len(); ++i)
		{
			if (AssetDir[i] == '/' || AssetDir[i] == '\\')
			{
				SingleFind += "../";
			}
		}
		FString SingleImport = FString::Printf(TEXT("import UISingleType from \"%sUtils/UISingle/UISingle\";"), *SingleFind);
		// FileContent += SingleImport + "\n\n";
		
		FileContent += FString::Printf(TEXT("const BP_%s_Class = UE.Class.Load('%s');\n"), *AssetName, *AssetPath);
		FileContent += FString::Printf(
			TEXT("const BP_%s = blueprint.tojs<typeof UE%s>(BP_%s_Class);\n\n"), *AssetName, *AssetPathPoint,
			*AssetName);
		FileContent += FString::Printf(TEXT("interface %s extends UE%s {}\n\n"), *GenerateName, *AssetPathPoint);
		FileContent += FString::Printf(TEXT("class %s {\n\tconstructor() {\n\n\t}\n\n}\n"), *GenerateName);
		// FileContent += FString::Printf(TEXT("class %s {\n\tconstructor() {\n\n\t}\n\tOnScriptInit(Single: UISingleType) {\n\n\t}\n\n}\n"), *GenerateName);
		FileContent += FString::Printf(TEXT("blueprint.mixin(BP_%s, %s);\n"), *AssetName, *GenerateName);

		// 将内容写入到文件中，以UTF8编码
		FFileHelper::SaveStringToFile(FileContent, *GeneratedFilePath, FFileHelper::EEncodingOptions::ForceUTF8);
	}

	// mixin生成文件目录完整路径：项目目录 + 生成目录 + 收集目录
	FString MixinDirFullPath = FPaths::ProjectDir() / GeneratedDir / GetDefault<UPuertsMixinGeneratorSetting>()->PuertsImportListPathScope;

	// 递归查找路径下的所有.ts文件
	TArray<FString> MixinFiles;
	IFileManager::Get().FindFilesRecursive(MixinFiles, *MixinDirFullPath, TEXT("*.ts"), true, false);
	// 将所有文件路径写入到import列表文件中
	FString ImportListContent;
	for (const FString& FilePath : MixinFiles)
	{

		// 检查文件开头是否包含mixin生成标志，如果不包含则不添加到import列表中
		FString FileContent;
		FFileHelper::LoadFileToString(FileContent, *FilePath);
		if (!FileContent.Contains(PuertsMixinGeneratorGenerateFlag))
		{
			continue;
		}
		
		// 去掉项目目录路径和生成目录路径，但是保留后面的路径，生成相对路径
		FString FilePathRelative = FilePath.Mid(FPaths::ProjectDir().Len() + GeneratedDir.Len());
		// 去掉文件扩展名
		FilePathRelative.RemoveFromEnd(TEXT(".ts"));
		// 生成import语句
		ImportListContent += FString::Printf(TEXT("import '.%s';\n"), *FilePathRelative);
	}

	// 获取设置的import列表文件路径
	FString ImportListFilePath = GetDefault<UPuertsMixinGeneratorSetting>()->PuertsImportListPath;
	// 生成列表文件完整路径：项目目录 + 生成目录 + import列表文件
	FString ImportListFullPath = FPaths::ProjectDir() / GeneratedDir / ImportListFilePath;

	// 将内容写入到import列表文件中，以UTF8编码
	FFileHelper::SaveStringToFile(ImportListContent, *ImportListFullPath, FFileHelper::EEncodingOptions::ForceUTF8);
}


#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FPuertsMixinGeneratorModule, PuertsMixinGenerator)
