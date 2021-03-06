using UnrealBuildTool;

public class DialogSystemRuntime : ModuleRules
{
    public DialogSystemRuntime(ReadOnlyTargetRules Target) : base(Target)
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
				, "GameplayTags"
				, "GameplayAbilities"
            }
        );


        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject"
                , "Engine"
                , "Slate"
                , "SlateCore"
            }
        );


        DynamicallyLoadedModuleNames.AddRange(
            new string[]
            {
            }
        );
    }
}