// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class BatchProcessAssets : ModuleRules
{
	public BatchProcessAssets(ReadOnlyTargetRules Target) : base(Target)
	{
		
		PublicIncludePaths.AddRange(
			new string[] {
				//"BatchProcessAssets/Public",
                "Runtime/AssetRegistry/Public",
                "Runtime/Engine/Classes/Materials",
				// ... add public include paths required here ...
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
                //"AssetTools",
				"BatchProcessAssets/Private",
                //"Editor/UnrealEd/Private",
                //"Editor/UnrealEd/Private/Settings",
                //"Editor/PackagesDialog/Public",
                //"Developer/DerivedDataCache/Public",
                //"Developer/TargetPlatform/Public",
                //"Developer/AssetTools/Private",
				// ... add other private include paths required here ...
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
                "ContentBrowser",
                "Core",
                "CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				"InputCore",
				"UnrealEd",
				"LevelEditor",
                "RHI",
                "AssetRegistry",
                "Analytics",
				// ... add private dependencies that you statically link with here ...	
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
               "AssetTools",
				// ... add any modules that your module loads dynamically here ...
			}
			);
	}
}
