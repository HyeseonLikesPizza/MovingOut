// Fill out your copyright notice in the Description page of Project Settings.


#include "Controller/MovingOutPlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

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
	}
}

void AMovingOutPlayerController::PlayerMove(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	UE_LOG(LogTemp, Warning, TEXT("Player Move Called"));

	const FRotator Rotation = GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	ACharacter* CharacterPawn = UGameplayStatics::GetPlayerCharacter(this, 0);
	CharacterPawn->AddMovementInput(ForwardDirection, MovementVector.X);
	CharacterPawn->AddMovementInput(RightDirection, MovementVector.Y);
}
