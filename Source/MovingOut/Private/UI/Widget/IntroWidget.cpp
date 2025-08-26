
#include "UI/Widget/IntroWidget.h"

void UIntroWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SetIsFocusable(true);
}


FReply UIntroWidget::NativeOnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent)
{
	Super::NativeOnKeyDown(MyGeometry, InKeyEvent);

	const FKey Key = InKeyEvent.GetKey();

	if (!bKeyPressed)
	{
		bKeyPressed = true;
		OnRequestSelectLevel.Broadcast();
		return FReply::Handled();
	}
	
	
	
	return Super::NativeOnKeyDown(MyGeometry, InKeyEvent);
}

