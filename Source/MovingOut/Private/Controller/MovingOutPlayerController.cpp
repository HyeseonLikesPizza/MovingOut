
#include "Controller/MovingOutPlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Blueprint/UserWidget.h"
#include "Character/PlayerMovingOutCharacter.h"
#include "Component/InteractiveComponent.h"
#include "Kismet/GameplayStatics.h"
#include "UI/Subsystem/UIManagerSubsystem.h"
#include "UI/Widget/TitleScreenWidget.h"

AMovingOutPlayerController::AMovingOutPlayerController()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AMovingOutPlayerController::BeginPlay()
{
	Super::BeginPlay();

	//ShowTitle();

	if (!IsLocalController()) return;
	if (ULocalPlayer* LP = GetLocalPlayer())
	{
		if (UUIManagerSubsystem* UI = LP->GetSubsystem<UUIManagerSubsystem>())
		{
			// Title 위젯 생성
			UI->ShowScreen(EUIScreen::InGame);
			UI->BindGameStateSignals();

			/*
			if (UTitleScreenWidget* TitleWidget = Cast<UTitleScreenWidget>(UI->GetCurrentWidget()))
			{
				// 델리게이트 구독
				TitleWidget->OnStartRequested.AddDynamic(this, &AMovingOutPlayerController::HandleStartRequested);
			}
			*/
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
		//EnhancedInputComponent->BindAction(PauseAction, ETriggerEvent::Started, this, &AMovingOutPlayerController::ThrowRelease);
		//EnhancedInputComponent->BindAction(PressToStartAction, ETriggerEvent::Started, this, &AMovingOutPlayerController::HandleStartRequested);
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

void AMovingOutPlayerController::ShowTitle()
{
	if (!IsLocalController()) return;
	if (ULocalPlayer* LP = GetLocalPlayer())
	{
		if (UUIManagerSubsystem* UI = LP->GetSubsystem<UUIManagerSubsystem>())
		{
			// Title 위젯 생성
			UI->ShowScreen(EUIScreen::Title);
			
			if (UTitleScreenWidget* TitleWidget = Cast<UTitleScreenWidget>(UI->GetCurrentWidget()))
			{
				// 델리게이트 구독
				TitleWidget->OnStartRequested.AddDynamic(this, &AMovingOutPlayerController::HandleStartRequested);
			}
			
		}
	}
}

void AMovingOutPlayerController::HandleRequestNewGame()
{
	UGameplayStatics::OpenLevel(this, FName("Stage1"));
}

void AMovingOutPlayerController::HandleStartRequested()
{
	UE_LOG(LogTemp, Warning, TEXT("HandleStartRequested Called"));
	if (!IsLocalController()) return;
	if (ULocalPlayer* LP = GetLocalPlayer())
	{
		if (UUIManagerSubsystem* UI = LP->GetSubsystem<UUIManagerSubsystem>())
		{
			UI->ShowScreen(EUIScreen::MainMenu);
		}
	}
}
