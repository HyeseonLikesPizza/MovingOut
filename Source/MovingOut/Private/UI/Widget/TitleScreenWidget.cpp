
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
		
		OnStartRequested.Broadcast();
		return FReply::Handled();	
	}
	
	return Super::NativeOnKeyDown(MyGeometry, InKeyEvent);
}


