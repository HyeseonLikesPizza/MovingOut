
#include "Controller/MovingOutPlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Blueprint/UserWidget.h"
#include "Character/PlayerMovingOutCharacter.h"
#include "Component/InteractiveComponent.h"
#include "Kismet/GameplayStatics.h"
#include "UI/Subsystem/UIManagerSubsystem.h"
#include "UI/Widget/MainMenuScreenWidget.h"
#include "UI/Widget/TitleScreenWidget.h"

AMovingOutPlayerController::AMovingOutPlayerController()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AMovingOutPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (!IsLocalController()) return;
	if (ULocalPlayer* LP = GetLocalPlayer())
	{
		if (UUIManagerSubsystem* UI = LP->GetSubsystem<UUIManagerSubsystem>())
		{
			UI->ApplyInitialUI();
			//UI->ShowScreen(EUIScreen::Title);
		}
	}
	// 플레이어 캐릭터 설정
	PlayerCharacter = Cast<APlayerMovingOutCharacter>(GetPawn());
}

void AMovingOutPlayerController::SetupInputComponent()
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
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMovingOutPlayerController::PlayerMove);
		EnhancedInputComponent->BindAction(GrabAction, ETriggerEvent::Started, this, &AMovingOutPlayerController::Grab);
		EnhancedInputComponent->BindAction(GrabAction, ETriggerEvent::Completed, this, &AMovingOutPlayerController::Release);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AMovingOutPlayerController::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &AMovingOutPlayerController::StopJumping);
		EnhancedInputComponent->BindAction(ThrowAction, ETriggerEvent::Started, this, &AMovingOutPlayerController::ThrowAim);
		EnhancedInputComponent->BindAction(ThrowAction, ETriggerEvent::Completed, this, &AMovingOutPlayerController::ThrowRelease);
		EnhancedInputComponent->BindAction(PauseAction, ETriggerEvent::Started, this, &AMovingOutPlayerController::HandleESCPressed);
	}

}

void AMovingOutPlayerController::PlayerMove(const FInputActionValue& Value)
{
	if (PlayerCharacter)
	{
		PlayerCharacter->HandleMove(Value);
	}
}

void AMovingOutPlayerController::Grab()
{
	if (PlayerCharacter)
	{
		PlayerCharacter->InteractiveComponent->TryGrab();
	}
}

void AMovingOutPlayerController::Release()
{
	if (PlayerCharacter)
	{
		PlayerCharacter->InteractiveComponent->GrabRelease();
	}
}

void AMovingOutPlayerController::Jump()
{
	if (PlayerCharacter)
	{
		PlayerCharacter->Jump();
	}
}

void AMovingOutPlayerController::StopJumping()
{
	if (PlayerCharacter)
	{
		PlayerCharacter->StopJumping();
	}
}

void AMovingOutPlayerController::ThrowAim()
{
	if (PlayerCharacter)
	{
		PlayerCharacter->InteractiveComponent->ThrowAim();
	}
}

void AMovingOutPlayerController::ThrowRelease()
{
	if (PlayerCharacter)
	{
		PlayerCharacter->InteractiveComponent->ThrowRelease();
	}
}

void AMovingOutPlayerController::HandleESCPressed()
{
	if (!IsLocalController()) return;
	if (ULocalPlayer* LP = GetLocalPlayer())
	{
		if (UUIManagerSubsystem* UI = LP->GetSubsystem<UUIManagerSubsystem>())
		{
			FlushPressedKeys();
			UI->HandleEscPressed();
		}
	}
}
