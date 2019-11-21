// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class TowerofAngra : ModuleRules
{
	public TowerofAngra(ReadOnlyTargetRules Target) : base(Target)
	{
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;


        bEnforceIWYU = false;

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore",

            "UMG", "AIModule", "GameplayTasks", "NavigationSystem" });


        PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
    }
}
