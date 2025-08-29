// Fill out your copyright notice in the Description page of Project Settings.


#include "Props/PropsGoalZone.h"

#include "Character/EnemyMovingOutCharacter.h"
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

	DropZone = CreateDefaultSubobject<UBoxComponent>(TEXT("DropZone"));
	Highlight = CreateDefaultSubobject<UMaterial>(TEXT("HighlightMaterial"));
	SetRootComponent((DropZone));
	
	static ConstructorHelpers::FObjectFinder<UMaterial> MaterialRef(TEXT("/Script/Engine.Material'/Game/Assets/Furniture_Free/Materials/M_GlowYellow.M_GlowYellow'"));
	if (MaterialRef.Succeeded())
	{
		Highlight = MaterialRef.Object;
	}

	DropZone->SetCollisionResponseToChannel(Props, ECR_Overlap);
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

	AMovingOutGameState* GameState = GetWorld()->GetGameState<AMovingOutGameState>();
	if (GameState)
	{
		// If other actor is CountProps :
		//		1. Change default material to Highlight
		//		2. Item count + 1
		if (ACountProps* InProps = Cast<ACountProps>(OtherActor))
		{
			InProps->ChangeMaterial(Highlight);
			TotalProps++;
			
			GameState->SetItemsDelivered(GameState->GetItemsDelivered()+1);
		}

		// If other actor is AI EnemyPawn, check additional goal to true 
		if (AEnemyMovingOutCharacter* Enemy = Cast<AEnemyMovingOutCharacter>(OtherActor))
		{
			GameState->AdditionalGoals[0].bCheck = true;
		}
	}
}

void APropsGoalZone::NotifyActorEndOverlap(AActor* OtherActor)
{
	Super::NotifyActorEndOverlap(OtherActor);

	AMovingOutGameState* GameState = GetWorld()->GetGameState<AMovingOutGameState>();
	if (GameState)
	{
		// If other actor is CountProps :
		//		1. Change Highlight to default material
		//		2. Item count - 1
		if (ACountProps* InProps = Cast<ACountProps>(OtherActor))
		{
			InProps->ChangeMaterial(Highlight);
			TotalProps--;
			
			GameState->SetItemsDelivered(GameState->GetItemsDelivered()-1);
		}

		// If other actor is AI EnemyPawn, check additional goal to false
		if (AEnemyMovingOutCharacter* Enemy = Cast<AEnemyMovingOutCharacter>(OtherActor))
		{
			GameState->AdditionalGoals[0].bCheck = false;
		}
	}
}



