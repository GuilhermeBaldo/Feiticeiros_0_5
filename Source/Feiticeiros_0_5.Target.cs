// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;
using System.Collections.Generic;

public class Feiticeiros_0_5Target : TargetRules
{
	public Feiticeiros_0_5Target(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;

		ExtraModuleNames.AddRange( new string[] { "Feiticeiros_0_5" } );
	}
}
