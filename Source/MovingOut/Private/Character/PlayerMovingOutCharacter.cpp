
#include "Character/PlayerMovingOutCharacter.h"
#include "EnhancedInputSubsystems.h"
#include "Kismet/GameplayStatics.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"

APlayerMovingOutCharacter::APlayerMovingOutCharacter()
{
	PhysicsHandle = CreateDefaultSubobject<UPhysicsHandleComponent>(TEXT("Handle"));
	
}

void APlayerMovingOutCharacter::HandleMove(const FInputActionValue& Value)
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

void APlayerMovingOutCharacter::TryGrab()
{
	UE_LOG(LogTemp, Warning, TEXT("TryGrab Called"));
}

void APlayerMovingOutCharacter::TryRelease()
{
	UE_LOG(LogTemp, Warning, TEXT("TryRelease Called"));
}
