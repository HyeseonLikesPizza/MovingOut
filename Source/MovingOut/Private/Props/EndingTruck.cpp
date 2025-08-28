// Fill out your copyright notice in the Description page of Project Settings.


#include "Props/EndingTruck.h"

#include "FindInBlueprintManager.h"
#include "Props/PropsGoalZone.h"

// Sets default values
AEndingTruck::AEndingTruck()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(GetRootComponent());

	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshTemp(TEXT("/Script/Engine.StaticMesh'/Game/Assets/Truck/SM_SM_Truck_half.SM_SM_Truck_half'"));
	if (MeshTemp.Succeeded())
	{
		Mesh->SetStaticMesh(MeshTemp.Object);
	}

	
}

// Called when the game starts or when spawned
void AEndingTruck::BeginPlay()
{
	Super::BeginPlay();
	
	// OnMatchStopped - 무빙아웃게임스테이트 바인딩
}

// Called every frame
void AEndingTruck::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CurrentTime >= EndTime)
	{
		// 딜리게이트 쏘기
	}
	else
	{
		// 트럭 앞으로 이동

		SetActorLocation(GetActorLocation() + Speed*DeltaTime*GetActorForwardVector());

		CurrentTime += DeltaTime;
	}
}

