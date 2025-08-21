
#include "UI/Widget/TitleScreenWidget.h"

void UTitleScreenWidget::NativeConstruct()
{
	Super::NativeConstruct();
	SetIsFocusable(true);
}

FReply UTitleScreenWidget::NativeOnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent)
{
	const FKey Key = InKeyEvent.GetKey();

	UE_LOG(LogTemp, Warning, TEXT("Space Bar Pressed"));

	if (Key == EKeys::SpaceBar)
	{
		OnStartRequested.Broadcast();
		return FReply::Handled();
	}
	
	return Super::NativeOnKeyDown(MyGeometry, InKeyEvent);
}


