
#include "UI/Widget/PauseWidget.h"

#include "Animation/WidgetAnimation.h"
#include "Components/Button.h"
#include "Kismet/KismetSystemLibrary.h"

FReply UPauseWidget::NativeOnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent)
{
	const FKey Key = InKeyEvent.GetKey();

	UE_LOG(LogTemp, Warning, TEXT("Pause Widget On Key Down"));

	if (Key == EKeys::NumPadOne || Key == EKeys::SpaceBar)
	{
		UE_LOG(LogTemp, Warning, TEXT("1 Pressed"));
		HandleResumeClicked();
		
		return FReply::Handled();
	}
	
	return Super::NativeOnKeyDown(MyGeometry, InKeyEvent);
}

void UPauseWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SetIsFocusable(true);
	
	if (NewGameButton)
		NewGameButton->OnClicked.AddDynamic(this, &UPauseWidget::HandleNewGameClicked);

	if (EndGameButton)
		EndGameButton->OnClicked.AddDynamic(this, &UPauseWidget::HandleEndGameClicked);

	if (ResumeButton)
		ResumeButton->OnClicked.AddDynamic(this, &UPauseWidget::HandleResumeClicked);

	if (P_Begin)
	{
		PlayAnimation(P_Begin);
	}
}

void UPauseWidget::NativeDestruct()
{
	Super::NativeDestruct();
	
	

	if (NewGameButton)
		NewGameButton->OnClicked.RemoveAll(this);
	
	if (EndGameButton)
		EndGameButton->OnClicked.RemoveAll(this);

	if (ResumeButton)
		ResumeButton->OnClicked.RemoveAll(this);

	
}

void UPauseWidget::HandleNewGameClicked()
{
	OnRequestNewGame.Broadcast();
}

void UPauseWidget::HandleEndGameClicked()
{
	UKismetSystemLibrary::QuitGame(GetWorld(), GetOwningPlayer(), EQuitPreference::Quit, false);
}

void UPauseWidget::HandleResumeClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("Resume Button Clicked"));
	OnRequestResumeGame.Broadcast();
}
