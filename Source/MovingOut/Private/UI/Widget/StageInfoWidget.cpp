
#include "UI/Widget/StageInfoWidget.h"

void UStageInfoWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SetIsFocusable(true);
}


FReply UStageInfoWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	const FKey Key = InKeyEvent.GetKey();

	if (Key == EKeys::SpaceBar)
	{
		OnRequestGameStart.Broadcast();
		return FReply::Handled();
	}

	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

