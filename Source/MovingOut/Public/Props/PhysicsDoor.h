// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PhysicsDoor.generated.h"

/*
 * Door opens physically when object or player push
 */

class UBoxComponent;

UCLASS()
class MOVINGOUT_API APhysicsDoor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APhysicsDoor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
	
public:	
	UPROPERTY(VisibleAnywhere)
	USceneComponent* Root;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* DoorMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UPhysicsConstraintComponent* PhysicsConstraint;

public:
	// 문 닫히는 속도
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Strength = 100.f;

	// 감쇠
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Damping = 10.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ForceLimit = 0.f;

protected:
	// 문 MaxCount만큼 열리면 떨어져 나감
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UBoxComponent* CountCollision;

	UPROPERTY(EditAnywhere)
	int32 OpenCount = 0;

	int32 MaxCount = 5;

public:
	UFUNCTION()
	void OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
