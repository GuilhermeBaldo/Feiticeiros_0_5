// Fill out your copyright notice in the Description page of Project Settings.


#include "Feiticeiros_0_5GameModeBase.h"
#include "MyPlayerController.h"
#include "MyCharacter.h"
#include "UObject/ConstructorHelpers.h"

AFeiticeiros_0_5GameModeBase::AFeiticeiros_0_5GameModeBase()
{
	// use our custom PlayerController class
	PlayerControllerClass = AMyPlayerController::StaticClass();

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Blueprints/BP_MyCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}