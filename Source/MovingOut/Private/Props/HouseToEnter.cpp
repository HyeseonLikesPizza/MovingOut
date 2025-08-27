// Fill out your copyright notice in the Description page of Project Settings.


#include "Props/HouseToEnter.h"

#include "Character/TruckPawn.h"
#include "Components/BoxComponent.h"

// Sets default values
AHouseToEnter::AHouseToEnter()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Collision = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision"));
	SetRootComponent(Collision);

	Collision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	
}

// Called when the game starts or when spawned
void AHouseToEnter::BeginPlay()
{
	Super::BeginPlay();

	Collision->OnComponentBeginOverlap.AddDynamic(this, &AHouseToEnter::OnBeginOverlap);
}

// Called every frame
void AHouseToEnter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AHouseToEnter::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == this)
	{
		return;
	}

	ATruckPawn* Truck = Cast<ATruckPawn>(OtherActor);
	if (Truck)
	{
		UE_LOG(LogTemp, Warning, TEXT("Apply widget here >_<"));
	}
	
}

