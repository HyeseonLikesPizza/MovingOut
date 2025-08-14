
#include "Controller/MovingOutPlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Character/PlayerMovingOutCharacter.h"
#include "Component/InteractiveComponent.h"

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
		EnhancedInputComponent->BindAction(ThrowAction, ETriggerEvent::Started, this, &AMovingOutPlayerController::ThrowAim);
		EnhancedInputComponent->BindAction(ThrowAction, ETriggerEvent::Completed, this, &AMovingOutPlayerController::ThrowRelease);
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
		PlayerCharacter->InteractiveComponent->TryGrab();
	}
}

void AMovingOutPlayerController::Release()
{
	if (auto* PlayerCharacter = Cast<APlayerMovingOutCharacter>(GetPawn()))
	{
		PlayerCharacter->InteractiveComponent->TryRelease();
	}
}

void AMovingOutPlayerController::Jump()
{
	if (auto* PlayerCharacter = Cast<APlayerMovingOutCharacter>(GetPawn()))
	{
		PlayerCharacter->Jump();
	}
}

void AMovingOutPlayerController::StopJumping()
{
	if (auto* PlayerCharacter = Cast<APlayerMovingOutCharacter>(GetPawn()))
	{
		PlayerCharacter->StopJumping();
	}
}

void AMovingOutPlayerController::ThrowAim()
{
	if (auto* PlayerCharacter = Cast<APlayerMovingOutCharacter>(GetPawn()))
	{
		PlayerCharacter->InteractiveComponent->ThrowAim();
	}
}

void AMovingOutPlayerController::ThrowRelease()
{
	if (auto* PlayerCharacter = Cast<APlayerMovingOutCharacter>(GetPawn()))
	{
		PlayerCharacter->InteractiveComponent->ThrowRelease();
	}
}
