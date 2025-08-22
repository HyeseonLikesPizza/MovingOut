
#include "UI/Widget/MainMenuScreenWidget.h"
#include "Components/Button.h"
#include "Kismet/KismetSystemLibrary.h"

void UMainMenuScreenWidget::NativeConstruct()
{
	Super::NativeConstruct();

	bIsFocusable = true;
	
	if (NewGameButton)
		NewGameButton->OnClicked.AddDynamic(this, &UMainMenuScreenWidget::HandleNewGameClicked);

	if (EndGameButton)
		EndGameButton->OnClicked.AddDynamic(this, &UMainMenuScreenWidget::HandleEndGameClicked);
	
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
	OnRequestNewGame.Broadcast();
}

void UMainMenuScreenWidget::HandleEndGameClicked()
{
	UKismetSystemLibrary::QuitGame(GetWorld(), GetOwningPlayer(), EQuitPreference::Quit, false);
}
