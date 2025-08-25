
#include "UI/Widget/MainMenuScreenWidget.h"
#include "Components/Button.h"
#include "Kismet/KismetSystemLibrary.h"

void UMainMenuScreenWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SetIsFocusable(true);
	
	if (NewGameButton)
	{
		NewGameButton->OnClicked.AddDynamic(this, &UMainMenuScreenWidget::HandleNewGameClicked);
		NewGameButton->OnHovered.AddDynamic(this, &UMainMenuScreenWidget::HandleNewGameButtonHover);
		NewGameButton->OnUnhovered.AddDynamic(this, &UMainMenuScreenWidget::HandleNewGameButtonUnhover);
	}
		

	if (EndGameButton)
	{
		EndGameButton->OnClicked.AddDynamic(this, &UMainMenuScreenWidget::HandleEndGameClicked);
		EndGameButton->OnHovered.AddDynamic(this, &UMainMenuScreenWidget::HandleEndGameButtonHover);
		EndGameButton->OnUnhovered.AddDynamic(this, &UMainMenuScreenWidget::HandleEndGameButtonUnhover);
	}
		

	if (M_Begin)
		PlayAnimation(M_Begin);
}

void UMainMenuScreenWidget::NativeDestruct()
{
	if (NewGameButton)
		NewGameButton->OnClicked.RemoveAll(this);
	
	if (EndGameButton)
		EndGameButton->OnClicked.RemoveAll(this);

	Super::NativeDestruct();
}

void UMainMenuScreenWidget::HandleNewGameClicked()
{
	PlayAnimation(M_End);
	PlayAnimation(M_Unhov_NewGame);
	
	FTimerHandle Th;
	GetWorld()->GetTimerManager().SetTimer(
		Th,
		[this]()
		{
			OnRequestIntro.Broadcast();
		},
		2.f,   // 대기 초
		false   // 반복 여부
	);
	
}

void UMainMenuScreenWidget::HandleEndGameClicked()
{
	UKismetSystemLibrary::QuitGame(GetWorld(), GetOwningPlayer(), EQuitPreference::Quit, false);
}

void UMainMenuScreenWidget::HandleEndGameButtonHover()
{
	if (M_Hov_Quit)
	{
		PlayAnimation(M_Hov_Quit);
	}
}

void UMainMenuScreenWidget::HandleEndGameButtonUnhover()
{
	if (M_Unhov_Quit)
	{
		PlayAnimation(M_Unhov_Quit);
	}
}

void UMainMenuScreenWidget::HandleNewGameButtonUnhover()
{
	if (M_Unhov_NewGame)
	{
		PlayAnimation(M_Unhov_NewGame);
	}
}

void UMainMenuScreenWidget::HandleNewGameButtonHover()
{
	if (M_Hov_NewGame)
	{
		PlayAnimation(M_Hov_NewGame);
	}
}
