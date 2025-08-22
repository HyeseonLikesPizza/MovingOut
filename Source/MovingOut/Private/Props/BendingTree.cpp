// Fill out your copyright notice in the Description page of Project Settings.


#include "Props/BendingTree.h"

#include "Character/PlayerMovingOutCharacter.h"
#include "Components/BoxComponent.h"

// Sets default values
ABendingTree::ABendingTree()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Collision = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision"));
	SetRootComponent(Collision);
	
	TreeMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TreeMesh"));
	TreeMesh->SetupAttachment(GetRootComponent());

	static ConstructorHelpers::FObjectFinder<UStaticMesh> TreeMeshTemp(TEXT("/Script/Engine.StaticMesh'/Game/Assets/Nature/StreetProps/Tree_Tall.Tree_Tall'"));
	if (TreeMeshTemp.Succeeded())
	{
		TreeMesh->SetStaticMesh(TreeMeshTemp.Object);
	}

	Collision->SetBoxExtent(FVector(50.f, 50.f, 50.f));
	TreeMesh->SetRelativeLocation(FVector(0, 110.f, -50.f));
}

// Called when the game starts or when spawned
void ABendingTree::BeginPlay()
{
	Super::BeginPlay();

	Collision->OnComponentBeginOverlap.AddDynamic(this, &ABendingTree::OnOverlapBegin);
	Collision->OnComponentEndOverlap.AddDynamic(this, &ABendingTree::OnOverlapEnd);
}

// Called every frame
void ABendingTree::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsBending)
	{
		// 1. 플레이어가 미는 방향
		FVector PushDirection = CharacterRef->GetActorLocation() - GetActorLocation();
		PushDirection.Normalize();

		// 2. 회전 축 결정
		// WorldUp과 PushDirection의 법선 = 벡터는 회전 축
		FVector WorldUp = FVector::UpVector;
		FVector RotationAxis = FVector::CrossProduct(WorldUp, PushDirection);
		RotationAxis.Normalize();

		// 회전 -> 쿼터니언
		FQuat DeltaQuat = FQuat(RotationAxis, FMath::DegreesToRadians(-1));
		FQuat NewQuat = DeltaQuat * GetActorQuat();
		SetActorRotation(NewQuat);
	}
}

void ABendingTree::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor || OtherActor == this)
	{
		return;
	}

	CharacterRef = Cast<APlayerMovingOutCharacter>(OtherActor);
	if (CharacterRef)
	{
		bIsBending = true;
	}
}

void ABendingTree::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	bIsBending = false;
}




