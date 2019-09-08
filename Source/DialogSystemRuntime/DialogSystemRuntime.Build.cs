using UnrealBuildTool;

public class DialogSystemRuntime : ModuleRules
{
    public DialogSystemRuntime(ReadOnlyTargetRules Target) : base(Target)
    {

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
                "Core",
            }
        );


        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore",
            }
        );


        DynamicallyLoadedModuleNames.AddRange(
            new string[]
            {
            }
        );
    }
}