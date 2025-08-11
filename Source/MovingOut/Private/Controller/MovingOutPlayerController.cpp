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
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AMovingOutPlayerController::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &AMovingOutPlayerController::StopJumping);
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

void AMovingOutPlayerController::Jump()
{
	UE_LOG(LogTemp, Warning, TEXT("Jump Called"));
	if (auto* PlayerCharacter = Cast<APlayerMovingOutCharacter>(GetPawn()))
	{
		PlayerCharacter->Jump();
		UE_LOG(LogTemp, Warning, TEXT("Jump Called"));
	}
}

void AMovingOutPlayerController::StopJumping()
{
	UE_LOG(LogTemp, Warning, TEXT("StopJumping Called"));
	if (auto* PlayerCharacter = Cast<APlayerMovingOutCharacter>(GetPawn()))
	{
		PlayerCharacter->StopJumping();
		UE_LOG(LogTemp, Warning, TEXT("StopJumping Called"));
	}
}
