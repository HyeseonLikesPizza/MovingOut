// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/MovingOutCharacter.h"
#include "PlayerMovingOutCharacter.generated.h"

class UPhysicsHandleComponent;
struct FInputActionValue;

UCLASS()
class MOVINGOUT_API APlayerMovingOutCharacter : public AMovingOutCharacter
{
	GENERATED_BODY()

public:

	APlayerMovingOutCharacter();
	
	void HandleMove(const FInputActionValue& Value);
	void TryGrab();
	void TryRelease();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Grab)
	float GrabTraceDistance;

	UPROPERTY(EditDefaultsOnly, Category=Grab)
	FName GrabBoneName;

private:
	
	bool bIsGrabbing;
	
	UPhysicsHandleComponent* PhysicsHandle;
};
