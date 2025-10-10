

using UnrealBuildTool;
using System.Collections.Generic;

public class Group2_ExamTarget : TargetRules
{
	public Group2_ExamTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V5;

		ExtraModuleNames.AddRange( new string[] { "Group2_Exam" } );
	}
}
