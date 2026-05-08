// Copyright Epic Games, Inc. All Rights Reserved.

#include "GAM415_KGrayGameMode.h"
#include "GAM415_KGrayCharacter.h"
#include "UObject/ConstructorHelpers.h"

AGAM415_KGrayGameMode::AGAM415_KGrayGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

}
