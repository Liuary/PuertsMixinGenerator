// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Framework/Commands/Commands.h"
#include "PuertsMixinGeneratorStyle.h"

class FPuertsMixinGeneratorCommands : public TCommands<FPuertsMixinGeneratorCommands>
{
public:

	FPuertsMixinGeneratorCommands()
		: TCommands<FPuertsMixinGeneratorCommands>(TEXT("PuertsMixinGenerator"), NSLOCTEXT("Contexts", "PuertsMixinGenerator", "PuertsMixinGenerator Plugin"), NAME_None, FPuertsMixinGeneratorStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > GeneratePuertsMixinFileAction;
};
