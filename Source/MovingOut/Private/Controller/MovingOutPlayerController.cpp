
#include "Controller/MovingOutPlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Blueprint/UserWidget.h"
#include "Character/PlayerMovingOutCharacter.h"
#include "Component/InteractiveComponent.h"
#include "UI/Subsystem/UIManagerSubsystem.h"

void AMovingOutPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (!IsLocalController()) return;
	if (ULocalPlayer* LP = GetLocalPlayer())
	{
		if (UUIManagerSubsystem* UI = LP->GetSubsystem<UUIManagerSubsystem>())
		{
			UI->ShowScreen(EUIScreen::InGame);
			UI->BindGameStateSignals();
		}
	}
}

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
		EnhancedInputComponent->BindAction(PauseAction, ETriggerEvent::Started, this, &AMovingOutPlayerController::ThrowRelease);
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
		PlayerCharacter->InteractiveComponent->GrabRelease();
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

void AMovingOutPlayerController::OnTogglePause()
{
	if (!IsLocalController()) return;
	if (ULocalPlayer* LP = GetLocalPlayer())
	{
		if (UUIManagerSubsystem* UI = LP->GetSubsystem<UUIManagerSubsystem>())
		{
			if (UUserWidget* Pause = UI->GetScreenWidget(EUIScreen::Pause))
			{
				if (Pause && Pause->IsInViewport())
				{
					UI->ShowScreen(EUIScreen::InGame);
				}
				else
				{
					UI->ShowScreen(EUIScreen::Pause);
				}
			}
		}
	}
}
