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

	FVector LH_GoalPos_WS;
	FRotator LH_GoalRot_WS;
	FVector RH_GoalPos_WS;
	FRotator RH_GoalRot_WS;
	bool bIKActive = false;

	UPROPERTY()
	UInteractiveComponent* InteractiveComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float HandOffset = 20.f;

	UPROPERTY()
	UPhysicsHandleComponent* PhysicsHandle;

	UPROPERTY()
	UPhysicsHandleComponent* PhysicsHandle_Left;

	FQuat HandsDeltaQ;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent* LeftHandIKTarget;

	UPROPERTY(BlueprintReadOnly)
	bool bLeftHandIK = false;

	UPROPERTY()
	FVector GrabPivotWS;

	UPROPERTY()
	bool bAlignToPivot = false;

	UPROPERTY()
	float AlignTimer = 0.f;

	void CacheFootPins();

	FTransform LeftFootPin_CS;
	FTransform RightFootPin_CS;
	

	
protected:
	


};
