// Fill out your copyright notice in the Description page of Project Settings.


#include "Props/FootBallGoalGimmick.h"

#include "Components/BoxComponent.h"
#include "Props/PropsBase.h"

// Sets default values
AFootBallGoalGimmick::AFootBallGoalGimmick()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Collision = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision"));
	SetRootComponent(Collision);
}

// Called when the game starts or when spawned
void AFootBallGoalGimmick::BeginPlay()
{
	Super::BeginPlay();

	Collision->OnComponentBeginOverlap.AddDynamic(this, &AFootBallGoalGimmick::OnBeginOverlap);
}

void AFootBallGoalGimmick::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APropsBase* Props = Cast<APropsBase>(OtherActor);
	if (Props && Props->bIsBall)
	{
		OnFootBallGoal.Broadcast();
	}
}

// Called every frame
void AFootBallGoalGimmick::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

