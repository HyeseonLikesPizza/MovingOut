// Fill out your copyright notice in the Description page of Project Settings.


#include "Props/PropsGoalZone.h"

#include "Components/BoxComponent.h"
#include "Components/MeshComponent.h"
#include "Game/MovingOutGameState.h"
#include "Props/CountProps.h"
#include "MovingOut/MovingOut.h"

// Sets default values
APropsGoalZone::APropsGoalZone()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	OverlapCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision"));
	Highlight = CreateDefaultSubobject<UMaterial>(TEXT("HighlightMaterial"));
	SetRootComponent((OverlapCollision));
	
	static ConstructorHelpers::FObjectFinder<UMaterial> MaterialRef(TEXT("/Script/Engine.Material'/Game/Assets/Furniture_Free/Materials/M_GlowYellow.M_GlowYellow'"));
	if (MaterialRef.Succeeded())
	{
		Highlight = MaterialRef.Object;
	}

	OverlapCollision->SetCollisionResponseToChannel(Props, ECR_Overlap);
}

// Called when the game starts or when spawned
void APropsGoalZone::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APropsGoalZone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APropsGoalZone::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	/* While in
	 * Change material to yellow
	 * TotalProps++
	 * 
	 */
	 
	 UE_LOG(LogTemp, Warning, TEXT("Name : %s"), *OtherActor->GetName());
	 
	ACountProps* InProps = Cast<ACountProps>(OtherActor);
	if (InProps)
	{
		InProps->ChangeMaterial(Highlight);
		if (AMovingOutGameState* GS = GetWorld()->GetGameState<AMovingOutGameState>())
		{
			GS->SetItemsDelivered(GS->GetItemsDelivered()+1);
		}
		TotalProps++;

		UE_LOG(LogTemp, Warning, TEXT("In! Total: %d"), TotalProps);
	}
}

void APropsGoalZone::NotifyActorEndOverlap(AActor* OtherActor)
{
	Super::NotifyActorEndOverlap(OtherActor);

	ACountProps* InProps = Cast<ACountProps>(OtherActor);
	if (InProps)
	{
		InProps->SetDefaultMaterial();
		if (AMovingOutGameState* GS = GetWorld()->GetGameState<AMovingOutGameState>())
		{
			GS->SetItemsDelivered(GS->GetItemsDelivered()-1);
		}
		TotalProps--;

		UE_LOG(LogTemp, Warning, TEXT("Out! Total: %d"), TotalProps);
	}
}



