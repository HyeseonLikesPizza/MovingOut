// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/MovingOutCharacter.h"
#include "PlayerMovingOutCharacter.generated.h"

/**
 * 
 */
UCLASS()
class MOVINGOUT_API APlayerMovingOutCharacter : public AMovingOutCharacter
{
	GENERATED_BODY()

public:
	void Walk();

	
};
