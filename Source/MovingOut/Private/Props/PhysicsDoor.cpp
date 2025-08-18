// Fill out your copyright notice in the Description page of Project Settings.


#include "Props/PhysicsDoor.h"

#include "PhysicsEngine/PhysicsConstraintComponent.h"

// Sets default values
APhysicsDoor::APhysicsDoor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;
	
	DoorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorMesh"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshRef(TEXT("/Script/Engine.StaticMesh'/Game/Assets/Furniture_Free/Meshes/SM_door_001.SM_door_001'"));
	if (MeshRef.Succeeded())
	{
		DoorMesh->SetStaticMesh(MeshRef.Object);
	}
	
	DoorMesh->SetupAttachment(Root);
	DoorMesh->SetSimulatePhysics(true);
	// 충돌한 액터의 이름 반환
	DoorMesh->SetNotifyRigidBodyCollision(true);

	PhysicsConstraint = CreateDefaultSubobject<UPhysicsConstraintComponent>(TEXT("PhysicsConstraint"));
	PhysicsConstraint->SetupAttachment(Root);
	
}

// Called when the game starts or when spawned
void APhysicsDoor::BeginPlay()
{
	Super::BeginPlay();

	// Directly specify component to connect. Will update frames based on current position.
	PhysicsConstraint->SetConstrainedComponents(nullptr, NAME_None, DoorMesh, NAME_None);

	/* Door 힌지 설정 */
	// Y축 회전 제한
	PhysicsConstraint->SetAngularSwing1Limit(ACM_Limited, 100.0f);
	// Z축 회전 잠금
	PhysicsConstraint->SetAngularSwing2Limit(ACM_Locked, 0.0f);
	// X축 회전 잠금
	PhysicsConstraint->SetAngularTwistLimit(ACM_Locked, 0.0f);

	/* Door 자연스럽게 여닫힘 설정 */
	PhysicsConstraint->SetAngularDriveMode(EAngularDriveMode::TwistAndSwing);

	// 닫힌 상태의 회전값
	FRotator TargetRotation = FRotator::ZeroRotator;
	PhysicsConstraint->SetAngularOrientationTarget(TargetRotation);

	PhysicsConstraint->SetAngularDriveParams(Strength, Damping, ForceLimit);

	PhysicsConstraint->SetAngularVelocityDriveTwistAndSwing(false, false);
	PhysicsConstraint->SetAngularOrientationDrive(true, true);
}

void APhysicsDoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
}
