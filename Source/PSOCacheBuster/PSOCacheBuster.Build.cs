using UnrealBuildTool;

public class PSOCacheBuster : ModuleRules
{
	public PSOCacheBuster(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		PrivateDependencyModuleNames.AddRange(new string[] { "Core", "RHI"});
	}
}
