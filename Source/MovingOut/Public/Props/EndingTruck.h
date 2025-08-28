// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EndingTruck.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTruckDeparted);

UCLASS()
/*
 * Stage1에 배치된 트럭 액터 :
 * Props를 가져다 놓는 곳
 * 시간 다 되면 펼쳐진 문 닫고 출발
 */

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
	// Truck mesh
	UPROPERTY(VisibleAnywhere)
	USceneComponent* Root;

	// 트럭 본체
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UStaticMeshComponent* Mesh;

	// 트럭 우측 문
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UStaticMeshComponent* RightDoor;

	// 트럭 좌측 문
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UStaticMeshComponent* BackDoor;

	// 트럭 짐칸에 PropsGoalZone 부착
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UChildActorComponent* PropsGoalZone;

public:
	// Move forward and notify end stage
	UPROPERTY(EditAnywhere)
	float EndTime = 2.f;

	UPROPERTY(EditAnywhere)
	float Speed = 400.f;

	// true면 트럭 출발
	bool bReadyToLeave = false;
	float CurrentTime = 0.f;

	UFUNCTION()
	void IsReadyToLeave();

	// Delegate
	UPROPERTY(BlueprintAssignable)
	FOnTruckDeparted OnTruckDeparted;
};
