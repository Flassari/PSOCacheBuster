#include "CoreGlobals.h"
#include "Misc/App.h"
#include "Misc/CommandLine.h"
#include "Misc/ConfigCacheIni.h"
#include "Modules/ModuleManager.h"
#include "RHI.h"
#include "Windows/WindowsPlatformMisc.h"
#include "HAL/FileManager.h"

DEFINE_LOG_CATEGORY_STATIC(LogPSOCacheBuster, Display, All);

class FPSOCacheBusterModule : public IModuleInterface
{
protected:

	/* 
	* This function should be a copy of the most up-to-date implementation of ClearPSODriverCache()
	* from Engine/Source/Runtime/D3D12RHI/Private/Windows/WindowsD3D12Device.cpp:
	* https://github.com/EpicGames/UnrealEngine/blob/ue5-main/Engine/Source/Runtime/D3D12RHI/Private/Windows/WindowsD3D12Device.cpp
	* 
	* If a GPU vendor changes their cache locations or file names, it can be easier for many projects
	* to update this plugin instead of modifying/updating the engine.
	* 
	* If the original function changes, update this function to match.
	*/
	static void ClearPSODriverCache()
	{
		FString LocalAppDataFolder = FPlatformMisc::GetEnvironmentVariable(TEXT("LOCALAPPDATA"));
		if (!LocalAppDataFolder.IsEmpty())
		{
			auto ClearFolder = [](const FString& PSOPath, const TCHAR* Extension)
				{
					TArray<FString> Files;
					IFileManager& FileManager = IFileManager::Get();
					FileManager.FindFiles(Files, *PSOPath, Extension);
					for (FString& File : Files)
					{
						FString FilePath = FString::Printf(L"%s\\%s", *PSOPath, *File);
						FileManager.Delete(*FilePath, /*RequireExists*/ false, /*EvenReadOnly*/ true, /*Quiet*/ true);
					}
				};

			if (IsRHIDeviceNVIDIA())
			{
				// NVIDIA used to have a global cache, but now also has a per-driver cache in a different folder in LocalLow.
				FString GlobalPSOPath = FPaths::Combine(*LocalAppDataFolder, TEXT("NVIDIA"), TEXT("DXCache"));
				ClearFolder(GlobalPSOPath, nullptr);

				FString PerDriverPSOPath = FPaths::Combine(*LocalAppDataFolder, TEXT(".."), TEXT("LocalLow"), TEXT("NVIDIA"), TEXT("PerDriverVersion"), TEXT("DXCache"));
				ClearFolder(PerDriverPSOPath, nullptr);
			}
			else if (IsRHIDeviceAMD())
			{
				FString PSOPath = FPaths::Combine(*LocalAppDataFolder, TEXT("AMD"), TEXT("DxCache"));
				ClearFolder(PSOPath, nullptr);

				PSOPath = FPaths::Combine(*LocalAppDataFolder, TEXT("AMD"), TEXT("DxcCache"));
				ClearFolder(PSOPath, nullptr);

				PSOPath = FPaths::Combine(*LocalAppDataFolder, TEXT(".."), TEXT("LocalLow"), TEXT("AMD"), TEXT("DxCache"));
				ClearFolder(PSOPath, nullptr);

				PSOPath = FPaths::Combine(*LocalAppDataFolder, TEXT(".."), TEXT("LocalLow"), TEXT("AMD"), TEXT("DxcCache"));
				ClearFolder(PSOPath, nullptr);
			}
			else if (IsRHIDeviceIntel())
			{
				// Intel stores the cache in LocalLow.
				FString PSOPath = FPaths::Combine(*LocalAppDataFolder, TEXT(".."), TEXT("LocalLow"), TEXT("Intel"), TEXT("ShaderCache"));
				ClearFolder(PSOPath, nullptr);
			}
		}
		else
		{
			UE_LOG(LogPSOCacheBuster, Error, TEXT("clearPSODriverCache failed: please ensure that LOCALAPPDATA points to C:\\Users\\<username>\\AppData\\Local"));
		}
	}

public:
	virtual void StartupModule() override
	{
#if IS_CLIENT_TARGET || UE_GAME // Ignore Editor, Server, and Program build targets

		if (FApp::GetBuildConfiguration() == EBuildConfiguration::Shipping)
		{
			bool bRunInShippingBuilds = false;
			GConfig->GetBool(TEXT("PSOCacheBuster"), TEXT("bRunInShippingBuilds"), bRunInShippingBuilds, GGameIni);
			if (!bRunInShippingBuilds)
			{
				// Do not run in Shipping configuration (unless configured to)
				return;
			}
		}

		UE_LOG(LogPSOCacheBuster, Log, TEXT("Busting the PSO cache."));
		ClearPSODriverCache();
		// The engine will re-clear the cache if -clearPSODriverCache is specified, but that's fine.
#endif
	}
};

IMPLEMENT_MODULE(FPSOCacheBusterModule, PSOCacheBuster)
