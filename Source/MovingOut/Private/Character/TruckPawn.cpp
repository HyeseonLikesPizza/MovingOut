// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/TruckPawn.h"

#include <rapidjson/document.h>

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ATruckPawn::ATruckPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(GetRootComponent());

	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshTemp(TEXT("/Script/Engine.StaticMesh'/Game/Assets/Truck/SM_Truck_all.SM_Truck_all'"));
	if (MeshTemp.Succeeded())
	{
		Mesh->SetStaticMesh(MeshTemp.Object);
	}

}

// Called when the game starts or when spawned
void ATruckPawn::BeginPlay()
{
	Super::BeginPlay();
	
	APlayerController* PC = GetWorld()->GetFirstPlayerController();

	UEnhancedInputLocalPlayerSubsystem* Subsys = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer());
	if (Subsys)
	{
		Subsys->AddMappingContext(InputMappingContext, 0);
	}
}

// Called every frame
void ATruckPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector P0 = GetActorLocation();
	FVector Velocity = GetActorForwardVector() * MoveSpeed;

	SetActorLocation(P0 + ForwardInput * Velocity * DeltaTime);

	AddActorLocalRotation(FRotator(0, TurnInput*DeltaTime*TurnSpeed, 0));
}

// Called to bind functionality to input
void ATruckPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(InputComponent);
	if (EnhancedInput)
	{
		EnhancedInput->BindAction(MoveForwardAction, ETriggerEvent::Triggered, this, &ATruckPawn::MoveForward);
		EnhancedInput->BindAction(MoveForwardAction, ETriggerEvent::Completed, this, &ATruckPawn::MoveForward);

		EnhancedInput->BindAction(TurnAction, ETriggerEvent::Triggered, this, &ATruckPawn::Turn);
		EnhancedInput->BindAction(TurnAction, ETriggerEvent::Completed, this, &ATruckPawn::Turn);
	}
}

void ATruckPawn::MoveForward(const FInputActionValue& Value)
{
	float AxisValue = Value.Get<float>();
	ForwardInput = AxisValue;
}

void ATruckPawn::Turn(const FInputActionValue& Value)
{
	float AxisValue = Value.Get<float>();
	TurnInput = AxisValue;
}

