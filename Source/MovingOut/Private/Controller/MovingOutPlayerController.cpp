// Fill out your copyright notice in the Description page of Project Settings.


#include "Controller/MovingOutPlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Character/PlayerMovingOutCharacter.h"

void AMovingOutPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(MappingContext, 0);
	}

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMovingOutPlayerController::PlayerMove);
		EnhancedInputComponent->BindAction(GrabAction, ETriggerEvent::Started, this, &AMovingOutPlayerController::Grab);
		EnhancedInputComponent->BindAction(GrabAction, ETriggerEvent::Completed, this, &AMovingOutPlayerController::Release);
		
	}
}

void AMovingOutPlayerController::PlayerMove(const FInputActionValue& Value)
{
	if (APlayerMovingOutCharacter* PlayerCharacter = Cast<APlayerMovingOutCharacter>(GetPawn()))
	{
		PlayerCharacter->HandleMove(Value);
	}
}

void AMovingOutPlayerController::Grab()
{
	if (auto* PlayerCharacter = Cast<APlayerMovingOutCharacter>(GetPawn()))
	{
		PlayerCharacter->TryGrab();
	}
}

void AMovingOutPlayerController::Release()
{
	if (auto* PlayerCharacter = Cast<APlayerMovingOutCharacter>(GetPawn()))
	{
		PlayerCharacter->TryRelease();
	}
}
