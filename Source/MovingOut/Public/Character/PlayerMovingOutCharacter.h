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

	UPROPERTY()
	UInteractiveComponent* InteractiveComponent;

	UPROPERTY()
	UPhysicsHandleComponent* PhysicsHandle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent* LeftHandIKTarget;

	UPROPERTY(BlueprintReadOnly)
	bool bLeftHandIK = false;

	UPROPERTY()
	bool bSlapped = false;

	
protected:
	


};
