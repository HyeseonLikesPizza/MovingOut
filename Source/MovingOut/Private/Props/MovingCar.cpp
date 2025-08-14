// Fill out your copyright notice in the Description page of Project Settings.


#include "Props/MovingCar.h"

// Sets default values
AMovingCar::AMovingCar()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CarMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CarMesh"));
	SetRootComponent(CarMesh);
}

// Called when the game starts or when spawned
void AMovingCar::BeginPlay()
{
	Super::BeginPlay();

	SetRandomMesh();
}

// Called every frame
void AMovingCar::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector CurrentLocation = GetActorLocation();

	SetActorLocation(CurrentLocation - Speed * DeltaTime * GetActorForwardVector());
}

void AMovingCar::SetRandomMesh()
{
	if (CarMeshes.Num() > 0)
	{
		int32 RandomIndex = FMath::RandRange(0, CarMeshes.Num() - 1);
		UStaticMesh* RandomMesh = CarMeshes[RandomIndex];

		if (RandomMesh)
		{
			CarMesh->SetStaticMesh(RandomMesh);
		}
	}
}

