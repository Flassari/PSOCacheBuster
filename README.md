# PSOCacheBuster
Clears the PSO driver cache for non-shipping and non-editor builds for easier testing and profiling of first-run-like experience.

Implements its own `ClearPSODriverCache()` function to be able to be updated independently from engine versions, in case GPU vendors change their PSO caching file/folder structure.

> [!NOTE]
> Nvidia recently changed the naming of their driver-cached PSOs, meaning that -clearPSODriverCache command line won't work in UE versions older than 5.6 without an engine fix. This plugin also implements that fix.

If you want it to run in shipping builds (advanced users) then you can add this to your Config/DefaultGame.ini file:
```
[PSOCacheBuster]
bRunInShippingBuilds=false
```
If you do that, remember to NOT ship this to users.

# Installation
Copy this plugin to either your ProjectName/Plugins or Engine/Plugins folder.

Enable the plugin in the Plugins window in Unreal Editor.

# License
PSOCacheBuster is licensed under [The Unlicense](https://unlicense.org/).

TLDR: Do whatever you want with it, no need to credit me.
