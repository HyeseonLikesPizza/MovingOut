// Fill out your copyright notice in the Description page of Project Settings.


#include "Props/PropsBase.h"
#include "MovingOut/MovingOut.h"
#include "CableComponent.h"


// Sets default values
APropsBase::APropsBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);
	DefaultMaterial = CreateDefaultSubobject<UMaterial>(TEXT("DefaultMaterial"));

	static ConstructorHelpers::FObjectFinder<UMaterial> MaterialRef(TEXT("/Script/Engine.Material'/Game/Assets/Furniture_Free/Materials/M_MaterialSlide.M_MaterialSlide'"));
	if (MaterialRef.Succeeded())
	{
		DefaultMaterial = MaterialRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UPhysicalMaterial> PhysMaterialRef(TEXT("/Script/PhysicsCore.PhysicalMaterial'/Game/Assets/Characters/Player/PM_Sticky.PM_Sticky'"));
	if (PhysMaterialRef.Succeeded())
	{
		Mesh->SetPhysMaterialOverride(PhysMaterialRef.Object);
	}
	
	// Mesh Collision
	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Mesh->SetCollisionObjectType(Props);
	Mesh->SetCollisionResponseToAllChannels((ECR_Block));

	// Physics on
	Mesh->SetSimulatePhysics(true); // 블루프린트 반영 X, 런타임 작동 O
}

// Called when the game starts or when spawned
void APropsBase::BeginPlay()
{
	Super::BeginPlay();

}



