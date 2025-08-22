// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class MovingOut : ModuleRules
{
	public MovingOut(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicDependencyModuleNames.AddRange(new [] { "Core", "CoreUObject", "Engine", "InputCore", "PhysicsCore", "NavigationSystem", "AIModule", "Niagara", "EnhancedInput", "GameplayTags", "GeometryCollectionEngine", "FieldSystemEngine", "CableComponent", "UMG"});

    }
	
}
			
