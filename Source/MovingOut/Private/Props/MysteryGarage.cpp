// Fill out your copyright notice in the Description page of Project Settings.


#include "Props/MysteryGarage.h"

#include "FindInBlueprintManager.h"
#include "Game/MovingOutGameState.h"
#include "Props/FootBallGoalGimmick.h"

// Sets default values
AMysteryGarage::AMysteryGarage()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	GarageBody = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GarageBody"));
	GarageBody->SetupAttachment(GetRootComponent());
	GarageRoof = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GarageRoof"));
	GarageRoof->SetupAttachment(GetRootComponent());
	GarageDoor = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GarageDoor"));
	GarageDoor->SetupAttachment(GetRootComponent());
}

// Called when the game starts or when spawned
void AMysteryGarage::BeginPlay()
{
	Super::BeginPlay();

	if (GoalGimmick)
	{
		GoalGimmick->OnFootBallGoal.AddDynamic(this, &AMysteryGarage::GarageControl);
	}
}

// Called every frame
void AMysteryGarage::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bLaunched && TotalTime > CameraReturnTime)
	{
		APlayerController* PC = GetWorld()->GetFirstPlayerController();
		if (PC)
		{
			PC->SetViewTargetWithBlend(PC->GetPawn(), 1.f);
		}
		
		bLaunched = false;
	}

	if (bDoorTimeAttacking)
	{
		CurrentLerpAlpha = FMath::Clamp(CurrentLerpAlpha + DeltaTime * DoorSpeed, 0.0f, 1.0f);
		
		FVector CurrentScale = GarageDoor->GetRelativeScale3D();
		
		if (bIsDoorOpening)
		{
			float NewZScale = FMath::Lerp(CurrentScale.Z, DoorOpenZScale, CurrentLerpAlpha);

			FVector NewScale = FVector(CurrentScale.X, CurrentScale.Y, NewZScale);
			GarageDoor->SetRelativeScale3D(NewScale);
		}
		else
		{
			float NewZScale = FMath::Lerp(CurrentScale.Z, DoorClosedZScale, CurrentLerpAlpha);
			
			FVector NewScale = FVector(CurrentScale.X, CurrentScale.Y, NewZScale);
			GarageDoor->SetRelativeScale3D(NewScale);
		}
		
		if (FMath::IsNearlyEqual(CurrentLerpAlpha, 1.f, 0.001f))
		{
			if (CurrentTime > DelayTime)
			{
				CurrentLerpAlpha = 0.f;
				bIsDoorOpening = !bIsDoorOpening;

				CurrentTime = 0.f;
			}

			CurrentTime += DeltaTime;
		}
	}

	TotalTime += DeltaTime;
}

void AMysteryGarage::GarageControl()
{
	if (bIsFirstGoal)
	{
		LaunchRoof();
		
		bIsFirstGoal = false;
	}

	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateLambda([&]()
	{
		bDoorTimeAttacking = true;
	}), LaunchDelay+2.f, false);
}

void AMysteryGarage::LaunchRoof()
{
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (PC && ViewLocation)
	{
		PC->SetViewTargetWithBlend(ViewLocation, BlendTime);
	}

	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateLambda([&]()
	{
		GarageRoof->SetSimulatePhysics(true);
		FVector LaunchDirection =  GetActorUpVector() + GetActorForwardVector() * -1.f;
		LaunchDirection.Normalize();
		
		GarageRoof->AddImpulse(LaunchDirection * LaunchSpeed, NAME_None, true);

		bLaunched = true;

		AMovingOutGameState* GameState = Cast<AMovingOutGameState>(GetWorld()->GetGameState());
		if (GameState)
		{
			GameState->AdditionalGoals[1].bCheck = true;
		}
	}), LaunchDelay, false);
}

