// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MovingCar.generated.h"

UCLASS()
class MOVINGOUT_API AMovingCar : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMovingCar();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<UStaticMesh*> CarMeshes;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* CarMesh;

protected:
	void SetRandomMesh();

	float Speed = 150.f;
	
};
