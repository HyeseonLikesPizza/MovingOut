// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EndingTruck.generated.h"

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
	UPROPERTY(VisibleAnywhere)
	USceneComponent* Root;

	// 트럭 본체
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UStaticMeshComponent* Mesh;

	// Props가 닿으면 노란색으로 바뀜
	//UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	//class APropsGoalZone* PropsGoalZone;
	
	// 트럭 우측 날개
	//UPROPERTY(EditAnywhere, BlueprintReadWrite)

	// 트럭 좌측 날개
	//UPROPERTY(EditAnywhere, BlueprintReadWrite)

public:
	// Move forward and notify end stage
	UPROPERTY(EditAnywhere)
	float EndTime = 2.f;

	UPROPERTY(EditAnywhere)
	float Speed = 500.f;

	float CurrentTime = 0.f;

	
};
