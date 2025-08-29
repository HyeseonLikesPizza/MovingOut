// Fill out your copyright notice in the Description page of Project Settings.


#include "Camera/ViewTarget.h"

#include "Game/MovingOutGameState.h"

// Sets default values
AViewTarget::AViewTarget()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);
}

// Called when the game starts or when spawned
void AViewTarget::BeginPlay()
{
	Super::BeginPlay();

	// Get Stage1 time limit has end delegete from MovingOutGameState
	// 제한시간 종료 알림
	AMovingOutGameState* GameState = Cast<AMovingOutGameState>(GetWorld()->GetGameState());
	if (GameState)
	{
		GameState->OnMatchStopped.AddDynamic(this, &AViewTarget::MoveToTargetLocation);
	}
}

// Called every frame
void AViewTarget::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AViewTarget::MoveToTargetLocation()
{
	// Move player's camera to designated location
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (PC && TargetLocation)
	{
		PC->SetViewTargetWithBlend(TargetLocation, BlendTime);
	}
}

