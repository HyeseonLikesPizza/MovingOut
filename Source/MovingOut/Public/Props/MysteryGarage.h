// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MysteryGarage.generated.h"

UCLASS()
class MOVINGOUT_API AMysteryGarage : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMysteryGarage();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	// Components
	UPROPERTY(VisibleAnywhere)
	USceneComponent* Root;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	class UStaticMeshComponent* GarageBody;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	class UStaticMeshComponent* GarageRoof;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	class UStaticMeshComponent* GarageDoor;
	
	UPROPERTY(EditAnywhere)
	class AFootBallGoalGimmick* GoalGimmick;

public:
	// Garage control
	UPROPERTY(EditAnywhere, Category = GarageControl)
	float DoorSpeed = 1.f;

	UPROPERTY(EditAnywhere, Category = GarageControl)
	float DelayTime = 2.f;
	
	bool bDoorTimeAttacking = false;

	bool bIsDoorOpening = true;
	float CurrentLerpAlpha = 0.f;

	float DoorOpenZScale = 0.1f;
	float DoorClosedZScale = 25.f;

	float CurrentTime = 0.f;
	
	UFUNCTION()
	void GarageControl();


public:
	// Launch GarageRoof
	UPROPERTY(EditAnywhere, Category = LaunchRoof)
	float LaunchSpeed = 10000.f;

	UPROPERTY(EditAnywhere, Category = LaunchRoof)
	AActor* ViewLocation;

	UPROPERTY(EditAnywhere, Category = LaunchRoof)
	float BlendTime = 0.f;

	UPROPERTY(EditAnywhere, Category = LaunchRoof)
	float LaunchDelay = 3.f;

protected:
	bool bIsFirstGoal = true;
	bool bLaunched = false;

	UPROPERTY(EditAnywhere, Category = LaunchRoof)
	float CameraReturnTime = 15.f;
	
	float TotalTime = 0.f;
	
	void LaunchRoof();
};
