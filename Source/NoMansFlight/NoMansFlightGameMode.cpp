// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "NoMansFlight.h"
#include "NoMansFlightGameMode.h"
#include "NoMansFlightPawn.h"

ANoMansFlightGameMode::ANoMansFlightGameMode()
{
	// set default pawn class to our flying pawn
	DefaultPawnClass = ANoMansFlightPawn::StaticClass();
}
