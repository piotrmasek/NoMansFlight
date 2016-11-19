// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class NoMansFlight : ModuleRules
{
	public NoMansFlight(TargetInfo Target)
	{
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore",
            "RHI", "RenderCore", "ShaderCore", "SimplexNoise", "RuntimeMeshComponent" });
	}
}
