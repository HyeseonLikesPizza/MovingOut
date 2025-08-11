// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PropsGoalZone.generated.h"

/* PropsBase가 놓이는 목표지점 */
UCLASS()
class MOVINGOUT_API APropsGoalZone : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APropsGoalZone();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void NotifyActorBeginOverlap(AActor* OtherActor);
	virtual void NotifyActorEndOverlap(AActor* OtherActor);

public:
	// Collision for PropsBase Detection
	UPROPERTY(VisibleAnywhere)
	class UBoxComponent* OverlapCollision;

	// Highlight when prop BP in collision
	UPROPERTY(VisibleAnywhere)
	UMaterial* Highlight;

protected:
	// Count props in zone
	int32 TotalProps = 0;

public:
	int32 GetTotalProps() {return TotalProps;}
	void SetTotalProps(int32 InTotalProps) {TotalProps = InTotalProps;}
};
