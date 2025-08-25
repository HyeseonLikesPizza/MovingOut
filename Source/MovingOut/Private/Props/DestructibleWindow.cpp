// Fill out your copyright notice in the Description page of Project Settings.


#include "Props/DestructibleWindow.h"
#include "Character/PlayerMovingOutCharacter.h"
#include "GeometryCollection/GeometryCollectionComponent.h"
#include "Props/PropsBase.h"

// Sets default values
ADestructibleWindow::ADestructibleWindow()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;
	
	DefaultMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DefaultMesh"));
	DefaultMesh->SetupAttachment(Root);

	GeometryCollection = CreateDefaultSubobject<UGeometryCollectionComponent>(TEXT("GeometryCollection"));
	GeometryCollection->SetupAttachment(Root);

	static ConstructorHelpers::FClassFinder<AActor> BP_MasterFieldRef(TEXT("/Script/Engine.Blueprint'/Game/Blueprints/Maps/Props/MS_MasterField.MS_MasterField_C'"));
	if (BP_MasterFieldRef.Succeeded())
	{
		MasterFieldClass = BP_MasterFieldRef.Class;
	}

	// 충돌 이벤트 설정
	GeometryCollection->SetNotifyRigidBodyCollision(true);
	GeometryCollection->SetGenerateOverlapEvents(true);
	
	GeometryCollection->SetVisibility(false);
}

// Called when the game starts or when spawned
void ADestructibleWindow::BeginPlay()
{
	Super::BeginPlay();
	
}

void ADestructibleWindow::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	GeometryCollection->OnComponentHit.AddDynamic(this, &ADestructibleWindow::OnHit);
}

void ADestructibleWindow::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	if (!OtherActor || OtherActor == this) return;
	
	if (Cast<APlayerMovingOutCharacter>(OtherActor) || Cast<APropsBase>(OtherActor))
	{
		DefaultMesh->SetVisibility(false);
		DefaultMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		GeometryCollection->SetVisibility(true);
		
		AActor* ActorToSpawn = GetWorld()->SpawnActor<AActor>(MasterFieldClass, OtherActor->GetActorLocation(), FRotator::ZeroRotator);

		GeometryCollection->SetCollisionResponseToAllChannels(ECR_Ignore);
		
		// Destroy after WaitTime
		GetWorld()->GetTimerManager().SetTimer(DestroyHandle, FTimerDelegate::CreateLambda([&]()
		{
			Destroy();
			
		}), WaitTime, false);
	}
}

