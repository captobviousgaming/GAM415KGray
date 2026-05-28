// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class GAM415_KGray : ModuleRules
{
    public GAM415_KGray(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        // [Week 3] Added Niagara. [Week 4] Added ProceduralMeshComponent for dynamic terrain generation.
        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "Niagara", "ProceduralMeshComponent" });
    }
}