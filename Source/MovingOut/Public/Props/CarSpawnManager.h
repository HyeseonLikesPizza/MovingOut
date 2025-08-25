// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CarSpawnManager.generated.h"

UCLASS()
class MOVINGOUT_API ACarSpawnManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACarSpawnManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere)
	class UBoxComponent* SpawnPoint;
	
	UPROPERTY(VisibleAnywhere)
	TSubclassOf<AActor> CarToSpawnClass;

public:
	FTimerHandle SpawnHandle;
	float WaitTime = 10.f;

	
};
