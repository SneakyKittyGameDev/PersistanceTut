// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "PersistanceTutGameMode.generated.h"

UCLASS(minimalapi)
class APersistanceTutGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	APersistanceTutGameMode();

	virtual void PostLogin(APlayerController* NewPlayer) override;
};



