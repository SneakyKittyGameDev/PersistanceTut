// Copyright Epic Games, Inc. All Rights Reserved.

#include "PersistanceTutGameMode.h"
#include "PersistancePlayerController.h"

#include "UObject/ConstructorHelpers.h"

APersistanceTutGameMode::APersistanceTutGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}

void APersistanceTutGameMode::PostLogin(APlayerController* NewPlayer)
{
	//Super::PostLogin(NewPlayer);

	if (APersistancePlayerController* PC = Cast<APersistancePlayerController>(NewPlayer))
	{
		PC->HandleServerEntry();
	}
}