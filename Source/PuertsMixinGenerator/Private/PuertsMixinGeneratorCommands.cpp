// Copyright Epic Games, Inc. All Rights Reserved.

#include "PuertsMixinGeneratorCommands.h"

#define LOCTEXT_NAMESPACE "FPuertsMixinGeneratorModule"

void FPuertsMixinGeneratorCommands::RegisterCommands()
{
	UI_COMMAND(GeneratePuertsMixinFileAction, "PuertsMixinGenerator", "Execute PuertsMixinGenerator action", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE
