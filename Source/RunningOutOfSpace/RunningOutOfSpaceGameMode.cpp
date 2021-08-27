// Copyright Epic Games, Inc. All Rights Reserved.

#include "RunningOutOfSpaceGameMode.h"
#include "RunningOutOfSpaceCharacter.h"
#include "UObject/ConstructorHelpers.h"

ARunningOutOfSpaceGameMode::ARunningOutOfSpaceGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
