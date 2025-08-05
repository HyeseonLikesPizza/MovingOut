// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/EnemyMovingOutCharacter.h"
#include "Components/ArrowComponent.h"
#include "Character/MovingOutCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "Materials/Material.h"
#include "Engine/World.h"


AEnemyMovingOutCharacter::AEnemyMovingOutCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	Arrow = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow"));
	if(Arrow)
	{
		PrimaryActorTick.bCanEverTick = true;
		Arrow->SetupAttachment(RootComponent);
		RootComponent = Arrow;
	}
}

void AEnemyMovingOutCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	FVector Direction = FVector(1.0f, 0.0f, 0.0f);
	AddMovementInput(Direction, 1.0f);
	
}
