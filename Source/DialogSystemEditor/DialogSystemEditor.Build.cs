using UnrealBuildTool;

public class DialogSystemEditor : ModuleRules
{
    public DialogSystemEditor(ReadOnlyTargetRules Target) : base(Target)
    {
	    PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        PublicIncludePaths.AddRange(
	        new string[] {
		        // ... add public include paths required here ...
	        }
        );


        PrivateIncludePaths.AddRange(
	        new string[] {
		        // ... add public include paths required here ...
	        }
        );


        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core"
                , "TargetPlatform"
				, "GameplayTags"
            }
        );


        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore",
                "InputCore",
                "UnrealEd",
                "PropertyEditor",
                "LevelEditor",
                "EditorStyle",
                "GraphEditor",
                "BlueprintGraph",
                "Projects",
                "ApplicationCore",
                "DesktopPlatform",
                "XmlParser",
                "DialogSystemRuntime",
            }
        );
        if (Target.Version.MinorVersion >= 24)
        {
	        PrivateDependencyModuleNames.AddRange(
		        new string[]
		        {
			        "ToolMenus"
		        }
	        );
        }
    }
}