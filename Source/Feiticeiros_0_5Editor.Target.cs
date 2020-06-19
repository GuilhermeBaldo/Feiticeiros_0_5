// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;
using System.Collections.Generic;

public class Feiticeiros_0_5EditorTarget : TargetRules
{
	public Feiticeiros_0_5EditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;

		ExtraModuleNames.AddRange( new string[] { "Feiticeiros_0_5" } );
	}
}
