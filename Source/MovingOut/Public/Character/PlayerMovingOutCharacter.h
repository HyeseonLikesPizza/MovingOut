// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/MovingOutCharacter.h"
#include "PlayerMovingOutCharacter.generated.h"

class UPhysicsHandleComponent;
struct FInputActionValue;
class UInteractiveComponent;

UCLASS()
class MOVINGOUT_API APlayerMovingOutCharacter : public AMovingOutCharacter
{
	GENERATED_BODY()

public:

	APlayerMovingOutCharacter();

	virtual void Tick(float DeltaSeconds) override;
	
	
	void HandleMove(const FInputActionValue& Value);
	void TryGrab();
	void TryRelease();
	void ThrowAim();
	void ThrowRelease();

	UPROPERTY()
	UInteractiveComponent* InteractiveComponent;

private:
	UPhysicsHandleComponent* PhysicsHandle;
	FHitResult Hit;
};
