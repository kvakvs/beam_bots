// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

public class BEAMBots : ModuleRules {
    private string ModulePath {
        get { return Path.GetDirectoryName(this.ModuleDirectory); }
    }

    private string ThirdPartyPath {
        get { return Path.GetFullPath(Path.Combine(ModulePath, "../ThirdParty/")); }
    }

    public BEAMBots(TargetInfo tgt) {
        PublicDependencyModuleNames.AddRange(new string[] {
            "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay"
        });
        MyExtraLibs(tgt);
    }

    public bool MyExtraLibs(TargetInfo tgt) {
        bool isLibrarySupported = false;

        if ((tgt.Platform == UnrealTargetPlatform.Win64) /* || (tgt.Platform == UnrealTargetPlatform.Win32) */) {
            isLibrarySupported = true;

            //string PlatformString = (Target.Platform == UnrealTargetPlatform.Win64) ? "x64" : "x86";
            string LibrariesPath = Path.Combine(ThirdPartyPath, "ZeroMQ", "_build64", "lib", "Release");
            string LibStatic = "s"; // "s" or ""
            string LibDebug = ""; // "gd" or ""

            //PublicAdditionalLibraries.Add(
            //    Path.Combine(LibrariesPath, "libzmq-v140-mt-" + LibDebug + "-4_1_5.lib"));
            PublicAdditionalLibraries.Add(
                Path.Combine(LibrariesPath, "libzmq-v140-mt-" + LibStatic + LibDebug + "-4_1_5.lib"));
        }

        if (isLibrarySupported) {
            // Include path
            PublicIncludePaths.Add(Path.Combine(ThirdPartyPath, "ZeroMQ", "include"));
            PublicIncludePaths.Add(Path.Combine(ThirdPartyPath, "CPPZMQ"));
        }

        Definitions.Add(string.Format("WITH_ZEROMQ={0}", isLibrarySupported ? 1 : 0));
        Definitions.Add("ZMQ_STATIC");

        return isLibrarySupported;
    }
}
