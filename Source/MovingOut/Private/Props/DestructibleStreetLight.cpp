// Fill out your copyright notice in the Description page of Project Settings.


#include "Props/DestructibleStreetLight.h"

#include "Character/TruckPawn.h"
#include "GeometryCollection/GeometryCollectionComponent.h"

// Sets default values
ADestructibleStreetLight::ADestructibleStreetLight()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	GeometryCollection = CreateDefaultSubobject<UGeometryCollectionComponent>(TEXT("GeometryCollection"));
	GeometryCollection->SetupAttachment(Root);

	static ConstructorHelpers::FClassFinder<AActor> BP_MasterFieldRef(TEXT("/Script/Engine.Blueprint'/Game/Blueprints/Maps/Props/MS_MasterField.MS_MasterField_C'"));
	if (BP_MasterFieldRef.Succeeded())
	{
		MasterFieldClass = BP_MasterFieldRef.Class;
	}

	GeometryCollection->SetNotifyRigidBodyCollision(true);
	GeometryCollection->SetGenerateOverlapEvents(true);
}

// Called when the game starts or when spawned
void ADestructibleStreetLight::BeginPlay()
{
	Super::BeginPlay();

	GeometryCollection->OnComponentHit.AddDynamic(this, &ADestructibleStreetLight::OnHit);
}

void ADestructibleStreetLight::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	if (!OtherActor || OtherActor == this) return;
	
	if (Cast<ATruckPawn>(OtherActor))
	{
		AActor* ActorToSpawn = GetWorld()->SpawnActor<AActor>(MasterFieldClass, OtherActor->GetActorLocation(), FRotator::ZeroRotator);

		GeometryCollection->SetCollisionResponseToAllChannels(ECR_Ignore);

		/*
		// Destroy after WaitTime
		GetWorld()->GetTimerManager().SetTimer(DestroyHandle, FTimerDelegate::CreateLambda([&]()
		{
			Destroy();
			
		}), WaitTime, false);
		*/
	}
	
}



