// Fill out your copyright notice in the Description page of Project Settings.


#include "Props/EndingTruck.h"

#include "FindInBlueprintManager.h"
#include "Game/MovingOutGameState.h"
#include "Kismet/GameplayStatics.h"
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
	RightDoor = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RightDoor"));
	RightDoor->SetupAttachment(GetRootComponent());
	BackDoor = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BackDoor"));
	BackDoor->SetupAttachment(GetRootComponent());
	
	PropsGoalZone = CreateDefaultSubobject<UChildActorComponent>(TEXT("PropsGoalZone"));
	PropsGoalZone->SetupAttachment(GetRootComponent());

	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshTemp(TEXT("/Script/Engine.StaticMesh'/Game/Assets/Truck/SM_Truck_halfV2.SM_Truck_halfV2'"));
	if (MeshTemp.Succeeded())
	{
		Mesh->SetStaticMesh(MeshTemp.Object);
	}

	PropsGoalZone->SetChildActorClass(APropsGoalZone::StaticClass());	
}

// Called when the game starts or when spawned
void AEndingTruck::BeginPlay()
{
	Super::BeginPlay();
	
	// MovingOutGameState::OnMatchStopped - 게임 끝 딜리게이트 바인딩
	AMovingOutGameState* GameState = Cast<AMovingOutGameState>(GetWorld()->GetGameState());
	if (GameState)
	{
		GameState->OnMatchStopped.AddDynamic(this, &AEndingTruck::IsReadyToLeave);
	}
	
}

// Called every frame
void AEndingTruck::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 1. 게임이 끝나면
	if (bReadyToLeave)
	{
		if (CurrentTime >= EndTime)
		{
			// 3. 딜리게이트 쏘기
			// 결과 위젯!
			OnTruckDeparted.Broadcast();
		}
		else
		{
			// 2. 트럭 앞으로 이동
			SetActorLocation(GetActorLocation() + Speed*DeltaTime*GetActorForwardVector());

			CurrentTime += DeltaTime;
		}
	}
}

void AEndingTruck::IsReadyToLeave()
{
	bReadyToLeave = true;
}

