/*// RTSPStreaming.Build.cs
using System.IO;
using UnrealBuildTool;

public class RTSPToVideo : ModuleRules
{
    public RTSPToVideo(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[]
        {
            "Core",
            "CoreUObject",
            "Engine",
            "UMG",
            "RenderCore",
            "RHI"
        });

        PrivateDependencyModuleNames.AddRange(new string[]
        {
            "Slate",
            "SlateCore",
	        "Projects"
        });

        // FFmpeg integration (adapted from your working code)
        string PluginDir = Path.GetFullPath(Path.Combine(ModuleDirectory, "../.."));
        string FFmpegRoot = Path.Combine(PluginDir, "ThirdParty", "FFmpeg");
        string FFmpegIncludePath = Path.Combine(FFmpegRoot, "include");
        string FFmpegLibPath = Path.Combine(FFmpegRoot, "lib", "Win64");
        string FFmpegBinPath = Path.Combine(FFmpegRoot, "bin", "Win64");

        // Debug output - now shows plugin-relative paths
        System.Console.WriteLine("\n=== RTSP Streaming Plugin FFmpeg Path Debug ===");
        System.Console.WriteLine($"Plugin Directory: {PluginDir}");
        System.Console.WriteLine($"FFmpeg Root: {FFmpegRoot}");
        System.Console.WriteLine($"Include Path: {FFmpegIncludePath}");
        System.Console.WriteLine($"Lib Path: {FFmpegLibPath}");
        System.Console.WriteLine($"Bin Path: {FFmpegBinPath}");

        // Check if paths exist
        if (Directory.Exists(FFmpegIncludePath))
        {
            System.Console.WriteLine("✓ Include path exists");
            PublicIncludePaths.Add(FFmpegIncludePath);
        }
        else
        {
            System.Console.WriteLine("✗ Include path missing: " + FFmpegIncludePath);
            throw new BuildException("FFmpeg include path not found!");
        }

        if (Directory.Exists(FFmpegLibPath))
        {
            System.Console.WriteLine("✓ Lib path exists");

            // Check each library file individually
            string[] LibFiles = { "avcodec.lib", "avformat.lib", "avutil.lib", "swscale.lib" };
            foreach (string LibFile in LibFiles)
            {
                string FullLibPath = Path.Combine(FFmpegLibPath, LibFile);
                if (File.Exists(FullLibPath))
                {
                    System.Console.WriteLine($"✓ Found: {LibFile}");
                    PublicAdditionalLibraries.Add(FullLibPath);
                }
                else
                {
                    System.Console.WriteLine($"✗ Missing: {FullLibPath}");
                    throw new BuildException($"FFmpeg library {LibFile} not found!");
                }
            }
        }
        else
        {
            System.Console.WriteLine("✗ Lib path missing: " + FFmpegLibPath);
            throw new BuildException("FFmpeg lib path not found!");
        }

        // Runtime dependencies (DLLs)
        if (Directory.Exists(FFmpegBinPath))
        {
            System.Console.WriteLine("✓ Bin path exists");

            string[] DllFiles = {
                "avcodec-61.dll",
                "avformat-61.dll",
                "avutil-59.dll",
                "swscale-8.dll"
            };

            foreach (string DllFile in DllFiles)
            {
                string FullDllPath = Path.Combine(FFmpegBinPath, DllFile);
                if (File.Exists(FullDllPath))
                {
                    System.Console.WriteLine($"✓ Found DLL: {DllFile}");
                    RuntimeDependencies.Add(FullDllPath);
                }
                else
                {
                    System.Console.WriteLine($"✗ Missing DLL: {FullDllPath}");
                    throw new BuildException($"FFmpeg DLL {DllFile} not found!");
                }
            }

            // Delay load DLLs
            PublicDelayLoadDLLs.AddRange(DllFiles);
        }
        else
        {
            System.Console.WriteLine("✗ Bin path missing: " + FFmpegBinPath);
            throw new BuildException("FFmpeg bin path not found!");
        }

        System.Console.WriteLine("=== End FFmpeg Debug ===\n");

        // Add the binary directory to the system path
        string FFmpegBinariesPath = Path.GetFullPath(Path.Combine(FFmpegRoot, "bin", "Win64"));
        if (Directory.Exists(FFmpegBinariesPath))
        {
            string PATH = System.Environment.GetEnvironmentVariable("PATH");
            PATH = FFmpegBinariesPath + Path.PathSeparator + PATH;
            System.Environment.SetEnvironmentVariable("PATH", PATH);
        }
    }
}*/

using System.IO;
using UnrealBuildTool;

public class RTSPToVideo : ModuleRules
{
    public RTSPToVideo(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[]
        {
            "Core",
            "CoreUObject",
            "Engine",
            "UMG",
            "RenderCore",
            "RHI"
        });

        PrivateDependencyModuleNames.AddRange(new string[]
        {
            "Slate",
            "SlateCore",
            "Projects"
        });

        // FFmpeg integration
        string PluginDir = Path.GetFullPath(Path.Combine(ModuleDirectory, "../.."));
        string FFmpegRoot = Path.Combine(PluginDir, "ThirdParty", "FFmpeg");
        string FFmpegIncludePath = Path.Combine(FFmpegRoot, "include");
        string FFmpegLibPath = Path.Combine(FFmpegRoot, "lib", "Win64");
        string FFmpegBinPath = Path.Combine(FFmpegRoot, "bin", "Win64");

        System.Console.WriteLine("\n=== RTSP Plugin FFmpeg Path Debug ===");
        System.Console.WriteLine($"Plugin Dir: {PluginDir}");
        System.Console.WriteLine($"Include: {FFmpegIncludePath}");
        System.Console.WriteLine($"Lib:     {FFmpegLibPath}");
        System.Console.WriteLine($"Bin:     {FFmpegBinPath}");

        if (!Directory.Exists(FFmpegIncludePath))
            throw new BuildException("FFmpeg include path not found!");
        PublicIncludePaths.Add(FFmpegIncludePath);

        if (!Directory.Exists(FFmpegLibPath))
            throw new BuildException("FFmpeg lib path not found!");

        string[] LibFiles = { "avcodec.lib", "avformat.lib", "avutil.lib", "swscale.lib" };
        foreach (var lib in LibFiles)
        {
            string fullPath = Path.Combine(FFmpegLibPath, lib);
            if (!File.Exists(fullPath))
                throw new BuildException($"Missing FFmpeg lib: {lib}");

            PublicAdditionalLibraries.Add(fullPath);
        }

        if (!Directory.Exists(FFmpegBinPath))
            throw new BuildException("FFmpeg bin path not found!");

        string[] DllFiles = {
            "avcodec-61.dll",
            "avformat-61.dll",
            "avutil-59.dll",
            "swscale-8.dll",
            "avdevice-61.dll",
            "avfilter-10.dll",
            "postproc-58.dll",
            "swresample-5.dll"
        };

        foreach (string dll in DllFiles)
        {
            string fullDllPath = Path.Combine(FFmpegBinPath, dll);
            if (!File.Exists(fullDllPath))
                throw new BuildException($"Missing FFmpeg DLL: {dll}");

            // This is the key part: ensures correct packaging
            RuntimeDependencies.Add("$(BinaryOutputDir)/" + dll, fullDllPath);
            PublicDelayLoadDLLs.Add(dll);
        }

        System.Console.WriteLine("=== End FFmpeg Debug ===\n");
    }
}
