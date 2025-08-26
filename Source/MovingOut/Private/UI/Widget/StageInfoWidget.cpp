
#include "UI/Widget/StageInfoWidget.h"

void UStageInfoWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SetIsFocusable(true);
	PlayAnimation(S_Begin);
}


FReply UStageInfoWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	const FKey Key = InKeyEvent.GetKey();

	if (Key == EKeys::SpaceBar && !bKeyPressed)
	{
		PlayAnimation(S_End);
		bKeyPressed = true;
		
		FTimerHandle Timer;
		GetWorld()->GetTimerManager().SetTimer(Timer, [this]()
		{
			OnRequestGameStart.Broadcast();
			return FReply::Handled();	
		}, 1.3f, false);
		
	}

	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

