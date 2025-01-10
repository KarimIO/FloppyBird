// Copyright (c) Ubisoft. All Rights Reserved.
// Licensed under the Apache 2.0 License. See LICENSE.md in the project root for license information.

using Sharpmake;

namespace FloppyBird
{
	[Sharpmake.Generate]
	public class FloppyBirdProject : Project
	{
        public string BasePath = @"[project.SharpmakeCsPath]\source";

		public FloppyBirdProject()
		{
			Name = "FloppyBird";
			AddTargets(new Target(Platform.win64, DevEnv.vs2019, Optimization.Debug | Optimization.Release));
			SourceRootPath = BasePath;
		}

		[Configure]
		public void ConfigureAll(Configuration conf, Target target)
		{
			conf.Options.Add(Sharpmake.Options.Vc.Compiler.CppLanguageStandard.CPP20);
			conf.Options.Add(Sharpmake.Options.Vc.Linker.SubSystem.Windows);
			conf.ProjectFileName = "[project.Name]_[target.DevEnv]_[target.Platform]";
			conf.ProjectPath = @"[project.SharpmakeCsPath]\projects";

			conf.AssemblyIncludePaths.Add(@"[project.SharpmakeCsPath]\dependencies\DirectXHeaders\src");
			conf.IncludePaths.Add(@"[project.SharpmakeCsPath]\dependencies\DirectXHeaders\include");
            conf.IncludePaths.Add(@"[project.BasePath]");
            conf.Defines.Add("WIN32_LEAN_AND_MEAN");
            conf.Defines.Add("NOMINMAX");
			conf.LibraryFiles.Add("d3d12.lib");
			conf.LibraryFiles.Add("dxgi.lib");
			conf.LibraryFiles.Add("D3Dcompiler.lib");
		}
	}

	[Sharpmake.Generate]
	public class FloppyBirdSolution : Sharpmake.Solution
	{
		public FloppyBirdSolution()
		{
			Name = "FloppyBird";
			AddTargets(new Target(Platform.win64, DevEnv.vs2019, Optimization.Debug | Optimization.Release));
		}

		[Configure()]
		public void ConfigureAll(Configuration conf, Target target)
		{
			conf.SolutionFileName = "[solution.Name]_[target.DevEnv]_[target.Platform]";
			conf.SolutionPath = @"[solution.SharpmakeCsPath]\projects";
			conf.AddProject<FloppyBirdProject>(target);
		}
	}

	public static class Main
	{
		[Sharpmake.Main]
		public static void SharpmakeMain(Sharpmake.Arguments arguments)
		{
			KitsRootPaths.SetUseKitsRootForDevEnv(DevEnv.vs2019, KitsRootEnum.KitsRoot10, Options.Vc.General.WindowsTargetPlatformVersion.v10_0_19041_0);
			arguments.Generate<FloppyBirdSolution>();
		}
	}
}
