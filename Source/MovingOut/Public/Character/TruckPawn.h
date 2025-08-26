// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "GameFramework/Pawn.h"
#include "TruckPawn.generated.h"

/*
 * Moveable truck for Level Select Map
 */
UCLASS()
class MOVINGOUT_API ATruckPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ATruckPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	UPROPERTY(VisibleAnywhere)
	USceneComponent* Root;
	
	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* Mesh;

protected:
	UPROPERTY(EditAnywhere)
	class UInputMappingContext* InputMappingContext;

	UPROPERTY(EditAnywhere)
	class UInputAction* MoveForwardAction;

	UPROPERTY(EditAnywhere)
	class UInputAction* TurnAction;

	void MoveForward(const FInputActionValue& Value);
	void Turn(const FInputActionValue& Value);
	
public:
	float ForwardInput;
	float TurnInput;

	UPROPERTY(EditAnywhere)
	float MoveSpeed = 800.f;
	
	UPROPERTY(EditAnywhere)
	float TurnSpeed = 100.f;
	
};
