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

	// Initialize components 
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);
	
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(GetRootComponent());
	
	RightDoorSceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("RightDoorScene"));
	RightDoorSceneComp->SetupAttachment(GetRootComponent());
	RightDoor = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RightDoor"));
	RightDoor->SetupAttachment(RightDoorSceneComp);

	BackDoorSceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("BackDoorScene"));
	BackDoorSceneComp->SetupAttachment(GetRootComponent());
	BackDoor = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BackDoor"));
	BackDoor->SetupAttachment(BackDoorSceneComp);
	
	PropsGoalZone = CreateDefaultSubobject<UChildActorComponent>(TEXT("PropsGoalZone"));
	PropsGoalZone->SetupAttachment(GetRootComponent());

	// Find and apply truck body mesh
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
	
	// Get Stage1 time limit has end delegete from MovingOutGameState
	// 시간제한으로 게임 종료 알림
	AMovingOutGameState* GameState = Cast<AMovingOutGameState>(GetWorld()->GetGameState());
	if (GameState)
	{
		GameState->OnMatchStopped.AddDynamic(this, &AEndingTruck::IsReadyToLeave);
	}

	// Initialize movement locations
	StartLocation = GetActorLocation();
	EndLocation = StartLocation + GetActorForwardVector() * MoveDistance;
}

// Called every frame
void AEndingTruck::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (bReadyToTakeOff)
	{
		if(!bAllDoorsClosed)
		{
			// 1. Close RightDoor and BackDoor

			// Calculate interpolation alpha and apply easing
			CurrentSlerpAlpha += DeltaTime * SlerpSpeed;
			CurrentSlerpAlpha = FMath::Clamp(CurrentSlerpAlpha, 0.f, 1.f);
			
			const float EasedAlpha = EaseInOutSine(CurrentSlerpAlpha);

			// === Close RightDoor ===
			FQuat StartRollQuat = FQuat(FRotator(0, 0, 110));
			FQuat EndRollQuat = FQuat(FRotator(0, 0, 0));

			FQuat NewRollQuat = FQuat::Slerp(StartRollQuat, EndRollQuat, EasedAlpha);

			RightDoorSceneComp->SetRelativeRotation(NewRollQuat);
			
			// === Close BackDoor ===

			/*
			 [참고]
			 * Pitch는 SetRelativeRotation으로 회전을 줄 경우, -90을 넘어가지 못하고 gimbal lock 발생
			 * 쿼터니언을 이용해서 문제 해결
			 */
			FQuat StartPitchQuat = FQuat(FRotator(0, 0, 0));
			FQuat EndPitchQuat = FQuat(FRotator(-110, 0, 0));

			FQuat NewPitchQuat = FQuat::Slerp(StartPitchQuat, EndPitchQuat, EasedAlpha);

			BackDoorSceneComp->SetRelativeRotation(NewPitchQuat);

			// Check doors as fully closed
			if (FMath::IsNearlyEqual(CurrentSlerpAlpha, 1.f, 0.001f))
			{
				bAllDoorsClosed = true;
			}
		}
		else
		{
			if (CurrentTime >= WaitTime)
			{
				// 2. Truck take off
				CurrentLerpAlpha += DeltaTime * LerpSpeed;
				CurrentLerpAlpha = FMath::Clamp(CurrentLerpAlpha, 0.f, 1.f);
				
				const float EasedAlpha = EaseInQuad(CurrentLerpAlpha);
				FVector NewLocation = FMath::Lerp(StartLocation, EndLocation, EasedAlpha);

				SetActorLocation(NewLocation);

				if (FMath::IsNearlyEqual(CurrentLerpAlpha, 1.f, 0.001f))
				{
					// 3. Broadcast truck has departed
					// 결과 위젯!
					UE_LOG(LogTemp, Warning, TEXT("Open result Map!"));
					OnTruckDeparted.Broadcast();

					// Prevent broadcast every Tick
					bReadyToTakeOff = false;
				}
			}
			
			CurrentTime += DeltaTime;
		}
	}
}

void AEndingTruck::IsReadyToLeave()
{
	bReadyToTakeOff = true;
}

float AEndingTruck::EaseInOutSine(float x)
{
	return -(FMath::Cos(PI * x) - 1.f) / 2.f;
}

float AEndingTruck::EaseInQuad(float x)
{
	return x * x;
}

