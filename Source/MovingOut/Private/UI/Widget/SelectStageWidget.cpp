
#include "UI/Widget/SelectStageWidget.h"
#include "UI/Subsystem/UIManagerSubsystem.h"

void USelectStageWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SetIsFocusable(true);

	UE_LOG(LogTemp, Warning, TEXT("Select Stage Widget Created"));

	APlayerController* PC = GetOwningPlayer();
	
	FTimerHandle Timer;
	GetWorld()->GetTimerManager().SetTimer(Timer, [this, PC]()
	{
		UE_LOG(LogTemp, Warning, TEXT("OnRequestStageInfo Broadcasted"));

		FInputModeUIOnly M;
		
		M.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		PC->SetInputMode(M);
		PC->bShowMouseCursor = true;
		M.SetWidgetToFocus(TakeWidget());
		SetKeyboardFocus();
		SetUserFocus(PC);
		
		OnRequestStageInfo.Broadcast();
	}, 10.f, false);
}
