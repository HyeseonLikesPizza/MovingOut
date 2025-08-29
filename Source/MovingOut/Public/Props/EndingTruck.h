// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EndingTruck.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTruckDeparted);

/**
 * Stage1에서 CountProps를 올려두는 트럭 클래스
 * 제한시간 끝나면 문 닫고 출발하는 로직 구현
 */
UCLASS()
class MOVINGOUT_API AEndingTruck : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEndingTruck();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	// Components
	/** Root of a truck actor */
	UPROPERTY(VisibleAnywhere)
	USceneComponent* Root;

	/** Mesh component of a truck body */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UStaticMeshComponent* Mesh;

	/**
	 * This Scene component helps set RightDoor Mesh to rotate in one axis precisely.
	 * @note RightDoor mesh itself tends to rotate in all axes without this.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USceneComponent* RightDoorSceneComp;

	/** Mesh component of a right side door */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UStaticMeshComponent* RightDoor;

	/**
	 * This Scene component helps set BackDoor Mesh to rotate in one axis precisely.
	 * @note BackDoor mesh itself tends to rotate in all axes without this.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USceneComponent* BackDoorSceneComp;

	/** Mesh component of a back side door */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UStaticMeshComponent* BackDoor;

	/**
	 * Child component to attach PropsGoalZone class.
	 * Detects CountProps within its bounds.
	 */ 
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UChildActorComponent* PropsGoalZone;

public:
	// Truck movement
	/** Time to wait after doors closed. Truck takes off after WaitTime */
	UPROPERTY(EditAnywhere, Category = TruckMovement)
	float WaitTime = 3.f;

	/** Time after departure to broadcast that the truck has moved far enough */
	//UPROPERTY(EditAnywhere, Category = TruckMovement)
	//float NotifyEndTime = 6.f;

protected:
	/** Total time of a truck moving forward */
	float CurrentTime = 0.f;

public:
	/** Truck movement speed when moving forward */
	UPROPERTY(EditAnywhere, Category = TruckMovement)
	float LerpSpeed = 0.2f;

protected:
	/**
	* Interpolation alpha used for movement lerping (0.0 to 1.0).
	* @note 트럭이 시동 걸고 서서히 출발하도록 보간
	*/
	float CurrentLerpAlpha = 0.0f;

	FVector StartLocation;
	FVector EndLocation;
	
	/** How far will the truck move */
	UPROPERTY(EditAnywhere, Category = TruckMovement)
	float MoveDistance = 1300.f;
	
	/**
	* Easing function for truck to take off naturally
	* @note For reference, see: https://easings.net/ko#easeInQuad
	*/
	float EaseInQuad(float x);
	
protected:
	// Door control
	/** If true, RightDoor and BackDoor fully closed and ready to take off. */
	bool bAllDoorsClosed = false;
	
	/**
	 * Interpolation alpha used for quaternion slerping (0.0 to 1.0).
	 * @note 문 열린 상태 -> 닫힌 상태 회전량을 보간
	 */
	float CurrentSlerpAlpha = 0.0f;

public:
	/** Speed multiplier for door rotate interpolation. */
	UPROPERTY(EditAnywhere, Category = DoorControl)
	float SlerpSpeed = 0.5f;

	/**
	 * Easing function for doors to rotate softly
	 * @note For reference, see: https://easings.net/ko#easeInOutSine
	 */
	float EaseInOutSine(float x);

public:
	// Delegate
	/**
	 * If true, Stage1 time limit has end.
	 * @note Tells this actor to close doors and take off
	 */
	bool bReadyToTakeOff = false;

	/**
	 * Triggered when the game signals that the truck should leave.
	 *
	 * Stage1 시간제한이 끝났음을 받는 함수
	 * @see AMovingOutGameState
	 */
	UFUNCTION()
	void IsReadyToLeave();

	/** Broadcast when the truck has departed from Stage1. */
	UPROPERTY(BlueprintAssignable)
	FOnTruckDeparted OnTruckDeparted;
};
