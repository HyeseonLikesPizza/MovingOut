
#include "UI/Widget/TitleScreenWidget.h"

UTitleScreenWidget::UTitleScreenWidget(const FObjectInitializer& OI) : Super(OI)
{
	SetIsFocusable(true);
}

void UTitleScreenWidget::NativeConstruct()
{
	Super::NativeConstruct();
	SetIsFocusable(true);
	PlayAnimation(T_Begin);
	
}

FReply UTitleScreenWidget::NativeOnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent)
{
	const FKey Key = InKeyEvent.GetKey();

	if (Key == EKeys::SpaceBar && !bKeyPressed)
	{
		bKeyPressed = true;
		PlayAnimation(T_End);

		FTimerHandle Timer;
		GetWorld()->GetTimerManager().SetTimer(Timer, [this]()
		{
			OnStartRequested.Broadcast();
		},2.f, false);
		
		
		return FReply::Handled();	
	}
	
	return Super::NativeOnKeyDown(MyGeometry, InKeyEvent);
}


