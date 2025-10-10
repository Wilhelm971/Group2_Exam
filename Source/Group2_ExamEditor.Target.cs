

using UnrealBuildTool;
using System.Collections.Generic;

public class Group2_ExamEditorTarget : TargetRules
{
	public Group2_ExamEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V5;

		ExtraModuleNames.AddRange( new string[] { "Group2_Exam" } );
	}
}
