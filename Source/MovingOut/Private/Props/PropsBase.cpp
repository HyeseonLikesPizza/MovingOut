// Fill out your copyright notice in the Description page of Project Settings.


#include "Props/PropsBase.h"

// Sets default values
APropsBase::APropsBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	DefaultMaterial = CreateDefaultSubobject<UMaterial>(TEXT("DefaultMaterial"));

	static ConstructorHelpers::FObjectFinder<UMaterial> MaterialRef(TEXT("/Script/Engine.Material'/Game/Assets/Furniture_Free/Materials/M_Material.M_Material'"));
	if (MaterialRef.Succeeded())
	{
		DefaultMaterial = MaterialRef.Object;
	}
	
	// Mesh Collision
	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Mesh->SetCollisionObjectType(ECC_WorldDynamic);
	Mesh->SetCollisionResponseToAllChannels((ECR_Block));

	// Physics on
	Mesh->SetSimulatePhysics(true); // 블루프린트 반영 X, 런타임 작동 O
}

// Called when the game starts or when spawned
void APropsBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void APropsBase::ChangeMaterial(UMaterial* NewMaterial)
{
	if (Mesh && NewMaterial)
	{
		Mesh->SetMaterial(0, NewMaterial);
	}
}

void APropsBase::SetDefaultMaterial()
{
	Mesh->SetMaterial(0, DefaultMaterial);
}



