// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FootBallGoalGimmick.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFootBallGoal);

UCLASS()
class MOVINGOUT_API AFootBallGoalGimmick : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFootBallGoalGimmick();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	// Components
	UPROPERTY(VisibleAnywhere)
	class UBoxComponent* Collision;

public:
	FOnFootBallGoal OnFootBallGoal;
	
};
