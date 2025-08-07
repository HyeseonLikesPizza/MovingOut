// Fill out your copyright notice in the Description page of Project Settings.


#include "Props/PropsGoalZone.h"

#include "Components/BoxComponent.h"
#include "Props/PropsBase.h"

#include "Components/MeshComponent.h"

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
	APropsBase* InProps = Cast<APropsBase>(OtherActor);
	if (InProps)
	{
		InProps->ChangeMaterial(Highlight);
		TotalProps++;
		
		UE_LOG(LogTemp, Warning, TEXT("In! Total: %d"), TotalProps);
	}
}

void APropsGoalZone::NotifyActorEndOverlap(AActor* OtherActor)
{
	Super::NotifyActorEndOverlap(OtherActor);

	APropsBase* InProps = Cast<APropsBase>(OtherActor);
	if (InProps)
	{
		InProps->SetDefaultMaterial();
		TotalProps--;

		UE_LOG(LogTemp, Warning, TEXT("Out! Total: %d"), TotalProps);
	}
}



