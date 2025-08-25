// Fill out your copyright notice in the Description page of Project Settings.


#include "Props/CarSpawnManager.h"

#include "Components/BoxComponent.h"

// Sets default values
ACarSpawnManager::ACarSpawnManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpawnPoint = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawnPoint"));
	RootComponent = SpawnPoint;
	
	static ConstructorHelpers::FClassFinder<AActor> BP_MovingCarRef(TEXT("/Script/Engine.Blueprint'/Game/Blueprints/Maps/Props/BP_MovingCar.BP_MovingCar_C'"));
	if (BP_MovingCarRef.Succeeded())
	{
		CarToSpawnClass = BP_MovingCarRef.Class;
	}
	
}

// Called when the game starts or when spawned
void ACarSpawnManager::BeginPlay()
{
	Super::BeginPlay();
	
	GetWorld()->GetTimerManager().SetTimer(SpawnHandle, FTimerDelegate::CreateLambda([&]()
	{
		AActor* ActorToSpawn = GetWorld()->SpawnActor<AActor>(CarToSpawnClass, SpawnPoint->GetComponentTransform());
	}), WaitTime, true);
}

void ACarSpawnManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	GetWorld()->GetTimerManager().ClearTimer(SpawnHandle);
}

// Called every frame
void ACarSpawnManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

	