
#include "Controller/DebugPlayerController.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Character/PlayerMovingOutCharacter.h"
#include "Component/InteractiveComponent.h"

class UEnhancedInputLocalPlayerSubsystem;

void ADebugPlayerController::BeginPlay()
{
	Super::BeginPlay();
}

void ADebugPlayerController::PlayerMove(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Warning, TEXT("Player Move"));
	if (PlayerCharacter)
	{
		PlayerCharacter->HandleMove(Value);
	}
}

void ADebugPlayerController::Grab()
{
	if (PlayerCharacter)
	{
		PlayerCharacter->InteractiveComponent->TryGrab();
	}
}

void ADebugPlayerController::Release()
{
	if (PlayerCharacter)
	{
		PlayerCharacter->InteractiveComponent->GrabRelease();
	}
}

void ADebugPlayerController::Jump()
{
	if (PlayerCharacter)
	{
		PlayerCharacter->Jump();
	}
}

void ADebugPlayerController::StopJumping()
{
	if (PlayerCharacter)
	{
		PlayerCharacter->StopJumping();
	}
}

void ADebugPlayerController::ThrowAim()
{
	if (PlayerCharacter)
	{
		PlayerCharacter->InteractiveComponent->ThrowAim();
	}
}

void ADebugPlayerController::ThrowRelease()
{
	if (PlayerCharacter)
	{
		PlayerCharacter->InteractiveComponent->ThrowRelease();
	}
}

void ADebugPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// IMC 설정
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(MappingContext, 0);
	}

	// Enhanced Input 바인딩
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ADebugPlayerController::PlayerMove);
		EnhancedInputComponent->BindAction(GrabAction, ETriggerEvent::Started, this, &ADebugPlayerController::Grab);
		EnhancedInputComponent->BindAction(GrabAction, ETriggerEvent::Completed, this, &ADebugPlayerController::Release);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ADebugPlayerController::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ADebugPlayerController::StopJumping);
		EnhancedInputComponent->BindAction(ThrowAction, ETriggerEvent::Started, this, &ADebugPlayerController::ThrowAim);
		EnhancedInputComponent->BindAction(ThrowAction, ETriggerEvent::Completed, this, &ADebugPlayerController::ThrowRelease);
		//EnhancedInputComponent->BindAction(PauseAction, ETriggerEvent::Started, this, &AMovingOutPlayerController::HandleESCPressed);
	}

}

void ADebugPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	PlayerCharacter = Cast<APlayerMovingOutCharacter>(GetPawn());
}
